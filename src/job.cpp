/*
 * SPDX-FileCopyrightText: 2017 Elvis Angelaccio <elvis.angelaccio@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 */

#include "job.h"
#include "symmydebug.h"

#include <QTimer>

#include <QGpgME/Job>

namespace Symmy
{

Job::Job(const QString &passphrase)
    : KJob {}
    , m_passphrase {passphrase}
{
}

Job::~Job()
{
}

char *Job::getPassphrase(const char *, const char *, bool, bool &)
{
    char *ret;
    gpgrt_asprintf(&ret, "%s", qUtf8Printable(passphrase()));
    return ret;
}

void Job::start()
{
    QTimer::singleShot(0, this, &Job::doWork);
}

bool Job::doKill()
{
    if (m_job.isNull()) {
        return false;
    }

    qCDebug(SYMMY) << "Going to stop" << m_job->metaObject()->className();
    m_job->slotCancel();
    return true;
}

void Job::setJob(QGpgME::Job *job)
{
    m_job = job;
}

QString Job::passphrase() const
{
    return m_passphrase;
}

}
