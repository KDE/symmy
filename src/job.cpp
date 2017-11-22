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
