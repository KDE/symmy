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

#include "decryptjob.h"

#include <KIO/CopyJob>

#include <QDebug>
#include <QMimeDatabase>
#include <QTemporaryFile>
#include <QTimer>

#include <QGpgME/DecryptJob>
#include <QGpgME/Protocol>

#include <gpgme++/key.h>

using namespace GpgME;
using namespace QGpgME;

namespace Symmy
{

DecryptJob::DecryptJob(const QString &ciphertextFilename) : Job {}
{
    m_ciphertext = std::make_shared<QFile>(ciphertextFilename);
    m_plaintext = std::make_shared<QTemporaryFile>(plaintextFilename());
}

DecryptJob::~DecryptJob()
{
    qDebug() << Q_FUNC_INFO;
}

QString DecryptJob::ciphertextFilename() const
{
    return m_ciphertext->fileName();
}

QString DecryptJob::plaintextFilename() const
{
    auto filename = ciphertextFilename();
    const auto pgpSuffixes = QMimeDatabase().mimeTypeForName(QStringLiteral("application/pgp-encrypted")).suffixes();
    for (const auto &suffix : pgpSuffixes) {
        if (filename.endsWith(suffix)) {
            filename.chop(suffix.length() + 1); // dot is not included in the suffix
            break;
        }
    }

    return filename;
}

void DecryptJob::doWork()
{
    if (not m_ciphertext->open(QIODevice::ReadOnly | QIODevice::Unbuffered)) {
        qDebug() << "Could not open ciphertext file" << ciphertextFilename();
        emitResult();
        return;
    }

    if (not m_plaintext->open()) {
        qDebug() << "Could not open plaintext file" << m_plaintext->fileName();
        emitResult();
        return;
    }

    auto decryptJob = openpgp()->decryptJob();
    setJob(decryptJob);

    connect(decryptJob, &QGpgME::DecryptJob::result, this, &DecryptJob::slotResult);
    connect(decryptJob, &QGpgME::Job::progress, this, [this](const QString &, int current, int total) {
        const auto ratio = static_cast<float>(current) / total;
        setPercent(static_cast<unsigned long>(100 * ratio));
    });

    qDebug() << "Starting decryption job...";
    decryptJob->start(m_ciphertext, m_plaintext);
}

void DecryptJob::slotResult(const DecryptionResult &, const QByteArray &, const QString &, const Error &)
{
    qDebug() << "Decryption job finished, plaintext size:" << m_plaintext->size();

    if (m_plaintext->size() == 0) {
        emitResult();
        return;
    }

    auto job = KIO::move(QUrl::fromLocalFile(m_plaintext->fileName()), QUrl::fromLocalFile(plaintextFilename()));
    connect(job, &KJob::result, this, &DecryptJob::emitResult);
}

}
