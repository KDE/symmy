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

#include <KCompositeJob>

class KPasswordDialog;

namespace Symmy
{

class CompositeJob : public KCompositeJob
{
    Q_OBJECT

public:

    enum class Task
    {
        Encryption,
        Decryption
    };

    explicit CompositeJob(const QStringList &filenames, Task task);
    ~CompositeJob() override;

    void start() override;

protected:
    bool doKill() override;

protected slots:
    void slotResult(KJob *job) override;

private slots:
    void slotAccepted();
    void slotRejected();
    void slotStart();

private:
    void startSubjob();
    QStringList filenames() const;
    Task task() const;

    QStringList m_filenames;
    QStringList m_failedDecryptions;
    Task m_task;
    KPasswordDialog *m_passwordDialog = nullptr;
};

}
