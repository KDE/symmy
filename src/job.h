/*
 * SPDX-FileCopyrightText: 2017 Elvis Angelaccio <elvis.angelaccio@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 */

#pragma once

#include <KJob>

#include <QPointer>

#include <gpgme++/interfaces/passphraseprovider.h>

namespace QGpgME
{
class Job;
}

namespace Symmy
{

class Job : public KJob,  public GpgME::PassphraseProvider
{
    Q_OBJECT

public:
    explicit Job(const QString &passphrase);
    ~Job() override;

    char *getPassphrase(const char *, const char *, bool, bool &) override;
    void start() override;
    virtual QString ciphertextFilename() const = 0;
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
