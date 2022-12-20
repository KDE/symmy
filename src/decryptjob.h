/*
 * SPDX-FileCopyrightText: 2017 Elvis Angelaccio <elvis.angelaccio@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 */

#pragma once

#include "job.h"

#include <gpgme++/error.h>
#include <gpgme++/decryptionresult.h>

class QFile;
class QTemporaryFile;

namespace Symmy
{

class DecryptJob : public Job
{
    Q_OBJECT

public:
    explicit DecryptJob(const QString &passphrase, const QString &ciphertextFilename);
    ~DecryptJob() override;

    QUrl destinationUrl() const override;
    QString ciphertextFilename() const override;
    QString plaintextFilename() const override;

private slots:
    void doWork() override;
    void slotResult(const GpgME::DecryptionResult &, const QByteArray &, const QString & = {}, const GpgME::Error & = {});

private:
    std::shared_ptr<QFile> m_ciphertext;
    std::shared_ptr<QTemporaryFile> m_plaintext;
};

}
