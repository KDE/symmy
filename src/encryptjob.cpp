/*
 * SPDX-FileCopyrightText: 2017 Elvis Angelaccio <elvis.angelaccio@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
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
    if (!m_plaintext->open(QIODevice::ReadOnly | QIODevice::Unbuffered)) {
        qCDebug(SYMMY) << "Could not open plaintext file" << m_plaintext->fileName();
        emitResult();
        return;
    }

    if (!m_ciphertext->open()) {
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
        setError(UserDefinedError);
        emitResult();
        return;
    }

    auto ciphertextPath = m_ciphertext->fileName();
    // Drop the ".XXXXXX" suffix of QTemporaryFile.
    ciphertextPath.chop(7);
    qCDebug(SYMMY) << "Moving temporary file" << QUrl::fromLocalFile(m_ciphertext->fileName()) << "to" << QUrl::fromLocalFile(ciphertextPath);
    auto job = KIO::move(QUrl::fromLocalFile(m_ciphertext->fileName()), QUrl::fromLocalFile(ciphertextPath));
    connect(job, &KJob::result, this, &EncryptJob::emitResult);
}

}
