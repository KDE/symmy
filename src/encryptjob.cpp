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
#include "symmydebug.h"

#include <KIO/CopyJob>

#include <QTemporaryFile>

#include <QGpgME/EncryptJob>
#include <QGpgME/Protocol>

#include <gpgme++/key.h>

using namespace GpgME;
using namespace QGpgME;

namespace Symmy
{

EncryptJob::EncryptJob(const QString &passphrase, const QString &plaintextFilename) : Job {passphrase}
{
    m_plaintext = std::make_shared<QFile>(plaintextFilename);
    m_ciphertext = std::make_shared<QTemporaryFile>(ciphertextFilename());
}

EncryptJob::~EncryptJob()
{
}

QString EncryptJob::ciphertextFilename() const
{
    return QStringLiteral("%1.gpg").arg(plaintextFilename());
}

QString EncryptJob::plaintextFilename() const
{
    return m_plaintext->fileName();
}

void EncryptJob::doWork()
{
    if (not m_plaintext->open(QIODevice::ReadOnly | QIODevice::Unbuffered)) {
        qCDebug(SYMMY) << "Could not open plaintext file" << m_plaintext->fileName();
        emitResult();
        return;
    }

    if (not m_ciphertext->open()) {
        qCDebug(SYMMY) << "Could not open ciphertext file" << m_ciphertext->fileName();
        emitResult();
        return;
    }

    auto encryptJob = openpgp()->encryptJob(false, false);
    auto context = QGpgME::Job::context(encryptJob);
    context->setPassphraseProvider(this);
    context->setPinentryMode(GpgME::Context::PinentryLoopback);

    setJob(encryptJob);

    connect(encryptJob, &QGpgME::EncryptJob::result, this, &EncryptJob::slotResult);
    connect(encryptJob, &QGpgME::EncryptJob::progress, this, [this](const QString &, int current, int total) {
        const auto ratio = static_cast<float>(current) / total;
        setPercent(static_cast<unsigned long>(100 * ratio));
    });


    encryptJob->start({}, m_plaintext, m_ciphertext, Context::None);
}

void EncryptJob::slotResult(const EncryptionResult &, const QByteArray &, const QString &, const Error &)
{
    qCDebug(SYMMY) << "Encryption job finished, ciphertext size:" << m_ciphertext->size();

    if (m_ciphertext->size() == 0) {
        setError(KilledJobError);
        emitResult();
        return;
    }

    auto job = KIO::move(QUrl::fromLocalFile(m_ciphertext->fileName()), QUrl::fromLocalFile(ciphertextFilename()));
    connect(job, &KJob::result, this, &EncryptJob::emitResult);
}

}
