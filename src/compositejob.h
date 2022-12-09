/*
 * SPDX-FileCopyrightText: 2017 Elvis Angelaccio <elvis.angelaccio@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 */

#pragma once

#include <KCompositeJob>

#include <QDialog>
#include <QPointer>

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
    void slotPercent(KJob *, unsigned long percent);

private:
    void startSubjob();
    QStringList filenames() const;
    Task task() const;

    QStringList m_filenames;
    QStringList m_failedDecryptions;
    Task m_task;
    QPointer<QDialog> m_passwordDialog;
};

}
