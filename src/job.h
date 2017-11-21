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
    explicit Job();
    ~Job() override;

    char *getPassphrase(const char *, const char *, bool, bool &) override;
    void start() override;
    virtual QString ciphertextFilename() const = 0;
    virtual QString plaintextFilename() const = 0;
    QString passphrase() const;

public slots:
    void setPassphrase(const QString &passphrase);

protected:
    bool doKill() override;
    void setJob(QGpgME::Job *job);
    QGpgME::Job *job();

private slots:
    virtual void doWork() = 0;

private:
    QPointer<QGpgME::Job> m_job;
    QString m_passphrase;
};

}
