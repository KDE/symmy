/*
 * SPDX-FileCopyrightText: 2017 Elvis Angelaccio <elvis.angelaccio@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 */

#pragma once

#include "job.h"

#include <gpgme++/error.h>
#include <gpgme++/encryptionresult.h>

class QFile;
class QTemporaryFile;

namespace Symmy
{

class EncryptJob : public Job
{
    Q_OBJECT

public:
    explicit EncryptJob(const QString &passphrase, const QString &plaintextFilename);
    ~EncryptJob() override;

    QUrl destinationUrl() const override;
    QString ciphertextFilename() const override;
    QString plaintextFilename() const override;

private slots:
    void doWork() override;
    void slotResult(const GpgME::EncryptionResult &, const QByteArray &, const QString & = {}, const GpgME::Error & = {});

private:
    std::shared_ptr<QFile> m_plaintext;
    std::shared_ptr<QTemporaryFile> m_ciphertext;
};

}
