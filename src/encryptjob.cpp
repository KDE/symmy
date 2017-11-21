/*
 * Copyright (C) 2017 Elvis Angelaccio <elvis.angelaccio@kde.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#include "encryptjob.h"

#include <KIO/CopyJob>
#include <KLocalizedString>
#include <KPasswordDialog>

#include <QDebug>
#include <QTemporaryFile>
#include <QTimer>

#include <QGpgME/EncryptJob>
#include <QGpgME/Protocol>

#include <gpgme++/key.h>

using namespace GpgME;
using namespace QGpgME;

namespace Symmy
{

EncryptJob::EncryptJob(const QString &plaintextFilename) : Job {}
{
    m_plaintext = std::make_shared<QFile>(plaintextFilename);
    m_ciphertext = std::make_shared<QTemporaryFile>(ciphertextFilename());
}

EncryptJob::~EncryptJob()
{
}

QString EncryptJob::ciphertextFilename() const
{
    return plaintextFilename() + QLatin1String {".gpg"};
}

QString EncryptJob::plaintextFilename() const
{
    return m_plaintext->fileName();
}

char *EncryptJob::getPassphrase(const char *, const char *, bool previousWasBad, bool &)
{
    qDebug() << "Previously failed to retrieve passphrase?" << previousWasBad;

    char *ret;
    gpgrt_asprintf(&ret, "%s", qUtf8Printable(passphrase()));
    return ret;
}

void EncryptJob::doWork()
{
    if (not m_plaintext->open(QIODevice::ReadOnly | QIODevice::Unbuffered)) {
        qDebug() << "Could not open plaintext file" << m_plaintext->fileName();
        emitResult();
        return;
    }

    if (not m_ciphertext->open()) {
        qDebug() << "Could not open ciphertext file" << m_ciphertext->fileName();
        emitResult();
        return;
    }

    auto encryptJob = openpgp()->encryptJob(false, false);
    auto context = QGpgME::Job::context(encryptJob);
    context->setPassphraseProvider(this);
    context->setPinentryMode(GpgME::Context::PinentryLoopback);

    setJob(encryptJob);

    auto passwordDialog = new KPasswordDialog {};
    passwordDialog->setPrompt(i18n("Please supply a password or passphrase to be used as encryption key."));

    connect(passwordDialog, &QDialog::accepted, this, &EncryptJob::slotAccepted);
    connect(passwordDialog, &QDialog::rejected, this, &EncryptJob::slotRejected);
    connect(passwordDialog, &QDialog::finished, passwordDialog, &QObject::deleteLater);
    connect(passwordDialog, &KPasswordDialog::gotPassword, this, &Job::setPassphrase);

    connect(encryptJob, &QGpgME::EncryptJob::result, this, &EncryptJob::slotResult);
    connect(encryptJob, &QGpgME::EncryptJob::progress, this, [this](const QString &, int current, int total) {
        const auto ratio = static_cast<float>(current) / total;
        setPercent(static_cast<unsigned long>(100 * ratio));
    });

    emit description(this, i18nc("description of an encryption job", "Encrypting"),
                     qMakePair(i18nc("Filename used as input of the encryption algorithm", "Plaintext"), plaintextFilename()),
                     qMakePair(i18nc("Filename created by the encryption algorithm", "Ciphertext"), ciphertextFilename()));

    passwordDialog->open();
}

void EncryptJob::slotAccepted()
{
    qDebug() << "Got a passhprase, starting encryption job...";
    qobject_cast<QGpgME::EncryptJob*>(job())->start({}, m_plaintext, m_ciphertext, Context::None);
}

void EncryptJob::slotRejected()
{
    qDebug() << "Passphrase dialog rejected.";
    setError(KilledJobError);
    emitResult();
}

void EncryptJob::slotResult(const EncryptionResult &, const QByteArray &, const QString &, const Error &)
{
    qDebug() << "Encryption job finished, ciphertext size:" << m_ciphertext->size();

    if (m_ciphertext->size() == 0) {
        setError(KilledJobError);
        emitResult();
        return;
    }

    auto job = KIO::move(QUrl::fromLocalFile(m_ciphertext->fileName()), QUrl::fromLocalFile(ciphertextFilename()));
    connect(job, &KJob::result, this, &EncryptJob::emitResult);
}

}
