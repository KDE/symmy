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
#include "symmydebug.h"
#include "symmyversion.h"

#include <KAboutData>
#include <KLocalizedString>

#include <QApplication>
#include <QCommandLineParser>

#include <iostream>

int main(int argc, char **argv)
{
    QApplication app {argc, argv};
    auto aboutData = KAboutData {QStringLiteral("symmy"), i18nc("display name for 'symmy' binary", "GPG Symmetric Encryption Frontend"), QStringLiteral(SYMMY_VERSION_STRING),
                                 i18n("Encrypt/decrypt one ore more files using GPG symmetric encryption."), KAboutLicense::GPL, i18n("(c) 2017 Elvis Angelaccio")};
    aboutData.addAuthor(i18n("Elvis Angelaccio"), {}, QStringLiteral("elvis.angelaccio@kde.org"));
    KAboutData::setApplicationData(aboutData);

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument(QStringLiteral("files"), i18n("List of files to encrypt or decrypt."));
    parser.addOption(QCommandLineOption {QStringList {QStringLiteral("e"), QStringLiteral("encrypt")},
                                         i18n("Encrypt the given list of files (default if no options).")});
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
