/*
 * SPDX-FileCopyrightText: 2017 Elvis Angelaccio <elvis.angelaccio@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 */

#include "compositejob.h"
#include "symmydebug.h"
#include "symmyversion.h"

#include <KAboutData>
#include <KLocalizedString>

#include <QApplication>
#include <QCommandLineParser>

#include <iostream>

int main(int argc, char **argv)
{
    KLocalizedString::setApplicationDomain("symmy");

    QApplication app {argc, argv};
    auto aboutData = KAboutData {QStringLiteral("symmy"), i18nc("display name for 'symmy' binary", "GPG Symmetric Encryption Frontend"), QStringLiteral(SYMMY_VERSION_STRING),
                                 i18n("Encrypt/decrypt one ore more files using GPG symmetric encryption."), KAboutLicense::GPL, i18n("(c) 2017 Elvis Angelaccio")};
    aboutData.addAuthor(i18n("Elvis Angelaccio"), {}, QStringLiteral("elvis.angelaccio@kde.org"));
    KAboutData::setApplicationData(aboutData);

    QCommandLineParser parser;
    parser.addPositionalArgument(QStringLiteral("files"), i18n("List of files to encrypt or decrypt."));
    parser.addOption(QCommandLineOption {QStringList {QStringLiteral("e"), QStringLiteral("encrypt")},
                                         i18n("Encrypt the given list of files (this is the default if there are no other options).")});
    parser.addOption(QCommandLineOption {QStringList {QStringLiteral("d"), QStringLiteral("decrypt")},
                                         i18n("Decrypt the given list of files.")});

    app.setQuitOnLastWindowClosed(false);

    aboutData.setupCommandLine(&parser);
    parser.process(app);
    aboutData.processCommandLine(&parser);

    if (parser.isSet(QStringLiteral("encrypt")) and parser.isSet(QStringLiteral("decrypt"))) {
        std::cerr << qPrintable(i18nc("--encrypt and --decrypt are CLI args, don't translate them", "Error: either --encrypt or --decrypt, not both.")) << '\n';
        parser.showHelp(-1);
    }

    if (parser.positionalArguments().isEmpty()) {
        parser.showHelp(-1);
    }

    const auto files = parser.positionalArguments();

    if (parser.isSet(QStringLiteral("encrypt")) or not parser.isSet(QStringLiteral("decrypt"))) {
        qCDebug(SYMMY) << "Going to encrypt:" << files;
        auto job = new Symmy::CompositeJob {files, Symmy::CompositeJob::Task::Encryption};
        QObject::connect(job, &KJob::result, &app, &QCoreApplication::quit, Qt::QueuedConnection);
        job->start();
    } else {
        qCDebug(SYMMY) << "Going to decrypt:" << files;
        auto job = new Symmy::CompositeJob {files, Symmy::CompositeJob::Task::Decryption};
        QObject::connect(job, &KJob::result, &app, &QCoreApplication::quit, Qt::QueuedConnection);
        job->start();
    }

    qCDebug(SYMMY) << "Starting event loop...";
    return app.exec();
}
