/*
 * SPDX-FileCopyrightText: 2017 Elvis Angelaccio <elvis.angelaccio@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 */

#pragma once

#include <KJob>

#include <QPointer>
#include <QUrl>

#include <gpgme++/interfaces/passphraseprovider.h>

namespace QGpgME
{
class Job;
}

namespace Symmy
{

/**
 * Appends the ".gpg" suffix to the given @p plaintextFilename, assuming it doesn't end with ".gpg" already.
 * @return The generated filename, suitable for an encrypted file.
 */
QString ciphertextFilenameFrom(const QString &plaintextFilename);

/**
 * Removes any "encryption" extension (e.g. ".gpg") to the given @p ciphertextFilename.
 * Returns @p ciphertextFilename as-is if it doesn't end with an "encryption" extension.
 * @return The generated filename, suitable for a plaintext file.
 */
QString plaintextFilenameFrom(const QString &ciphertextFilename);

class Job : public KJob,  public GpgME::PassphraseProvider
{
    Q_OBJECT

public:
    explicit Job(const QString &passphrase);
    ~Job() override;

    char *getPassphrase(const char *, const char *, bool, bool &) override;
    void start() override;

    /**
     * @return The url of the file that will be created by this job.
     */
    virtual QUrl destinationUrl() const = 0;

    /**
     * @return The filename of the ciphertext handled by this job.
     */
    virtual QString ciphertextFilename() const = 0;

    /**
     * @return The filename of the plaintext handled by this job.
     */
    virtual QString plaintextFilename() const = 0;

    QString passphrase() const;

protected:
    bool doKill() override;
    void setJob(QGpgME::Job *job);

private slots:
    virtual void doWork() = 0;

private:
    QPointer<QGpgME::Job> m_job;
    QString m_passphrase;
};

}
