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

#include "compositejob.h"
#include "decryptjob.h"
#include "encryptjob.h"
#include "symmydebug.h"

#include <KIO/JobTracker>
#include <KJobTrackerInterface>
#include <KLocalizedString>
#include <KMessageBox>
#include <KPasswordDialog>

#include <QTimer>

namespace Symmy
{

CompositeJob::CompositeJob(const QStringList &filenames, Task task)
    : KCompositeJob {}
    , m_filenames {filenames}
    , m_task {task}
{
    setCapabilities(Killable);
    KIO::getJobTracker()->registerJob(this);
}

CompositeJob::~CompositeJob()
{
}

void CompositeJob::start()
{
    QTimer::singleShot(0, this, &CompositeJob::slotStart);
}

bool CompositeJob::doKill()
{
    if (!hasSubjobs()) {
        return false;
    }

    return subjobs().at(0)->kill();
}

void CompositeJob::slotResult(KJob *job)
{
    if (job->error() and task() == Task::Decryption) {
        qCDebug(SYMMY) << "Job failed with code" << job->error() << "and error" << job->errorText();
        auto decryptJob = qobject_cast<Symmy::DecryptJob*>(job);

        if (decryptJob and decryptJob->error() != KJob::KilledJobError) {
            qCDebug(SYMMY) << "Subjob failed to decrypt" << decryptJob->ciphertextFilename();
            m_failedDecryptions << decryptJob->ciphertextFilename();
        }
    }

    removeSubjob(job);

    if (hasSubjobs()) {
        qCDebug(SYMMY) << "Starting next subjob...";
        startSubjob();
        return;
    }

    qCDebug(SYMMY) << "Composite job finished";

    if (!m_failedDecryptions.isEmpty()) {
        if (m_failedDecryptions.size() == 1) {
            KMessageBox::error(nullptr, xi18nc("@info", "Could not decrypt the following ciphertext:<nl/><filename>%1</filename>", m_failedDecryptions.at(0)));
        } else {
            KMessageBox::errorList(nullptr, i18n("Could not decrypt the following ciphertexts:"), m_failedDecryptions);
        }

        // Nothing was decrypted, mark the composite job as failed.
        if (m_failedDecryptions.size() == filenames().size()) {
            setError(KJob::UserDefinedError);
            setErrorText(i18n("Wrong decryption key."));
        }
    }

    emitResult();
}

void CompositeJob::slotAccepted()
{
    for (const auto &filename : filenames()) {
        if (task() == Task::Encryption) {
            addSubjob(new Symmy::EncryptJob(m_passwordDialog->password(), filename));
        } else {
            addSubjob(new Symmy::DecryptJob(m_passwordDialog->password(), filename));
        }
    }

    qCDebug(SYMMY) << "Got a passphrase, starting first subjob...";
    startSubjob();
}

void CompositeJob::slotRejected()
{
    qCDebug(SYMMY) << "Passphrase dialog rejected.";
    setError(KilledJobError);
    emitResult();
}

void CompositeJob::slotStart()
{
    if (filenames().isEmpty()) {
        emitResult();
        return;
    }

    qCDebug(SYMMY) << "Starting composite job...";
    emit description(this, i18n("Asking Passphrase"));

    m_passwordDialog = new KPasswordDialog {};
    if (task() == Task::Encryption) {
        m_passwordDialog->setPrompt(i18n("Please supply a password or passphrase to be used as encryption key."));
    } else {
        m_passwordDialog->setPrompt(i18n("Please supply a password or passphrase to be used as decryption key."));
    }

    connect(m_passwordDialog, &QDialog::accepted, this, &CompositeJob::slotAccepted);
    connect(m_passwordDialog, &QDialog::rejected, this, &CompositeJob::slotRejected);
    connect(m_passwordDialog, &QDialog::finished, m_passwordDialog, &QObject::deleteLater);

    m_passwordDialog->open();
}

void CompositeJob::startSubjob()
{
    auto job = qobject_cast<Symmy::Job*>(subjobs().at(0));

    if (task() == Task::Encryption) {
        emit description(this, i18nc("description of an encryption job", "Encrypting"),
                         qMakePair(i18nc("File used as input of the encryption algorithm", "Plaintext"), job->plaintextFilename()),
                         qMakePair(i18nc("File created by the encryption algorithm", "Ciphertext"), job->ciphertextFilename()));
    } else {
        emit description(this, i18nc("description of a decryption job", "Decrypting"),
                         qMakePair(i18nc("File used as input of the decryption algorithm", "Ciphertext"), job->ciphertextFilename()),
                         qMakePair(i18nc("File created by the decryption algorithm", "Plaintext"), job->plaintextFilename()));
    }

    job->start();
}

QStringList CompositeJob::filenames() const
{
    return m_filenames;
}

CompositeJob::Task CompositeJob::task() const
{
    return m_task;
}

}
