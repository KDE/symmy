/*
 * Copyright (C) 2016 Elvis Angelaccio <elvis.angelaccio@kdemail.net>
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

#include "gpgfileitemaction.h"

#include <QDebug>
#include <QSaveFile>

GpgFileItemActionPlugin::GpgFileItemActionPlugin(QObject *parent)
    : KAbstractFileItemActionPlugin(parent)
{}

bool GpgFileItemActionPlugin::dataToFile(gpgme_data_t data, const QString &fileName)
{
    if (not data) {
        return false;
    }

    QSaveFile saveFile(fileName);
    if (not saveFile.open(QIODevice::WriteOnly | QIODevice::Unbuffered)) {
        qDebug() << "Could not open save file";
        return false;
    }

    // Ensure cursor is at initial position
    gpgme_data_seek(data, 0, SEEK_SET);

    char buffer[4096];
    while (auto bytes = gpgme_data_read(data, buffer, sizeof(buffer) / sizeof(buffer[0]))) {
        if (saveFile.write(buffer, bytes) != bytes) {
            qDebug() << saveFile.errorString();
            saveFile.cancelWriting();
            return false;
        }
    }

    if (not saveFile.commit()) {
        qDebug() << saveFile.errorString();
        return false;
    }

    return true;
}

bool GpgFileItemActionPlugin::fileToData(QFile &file, gpgme_data_t data)
{
    if (not data) {
        return false;
    }

    char buffer[4096];
    while (auto bytes = file.read(buffer, sizeof(buffer) / sizeof(buffer[0]))) {
        if (bytes == -1 or gpgme_data_write(data, buffer, bytes) != bytes) {
            qWarning() << "Write error for plaintext of" << file.fileName();
            return false;
        }
    }

    // Set current position back to the beginning
    gpgme_data_seek(data, 0, SEEK_SET);

    return true;
}

bool GpgFileItemActionPlugin::initGpgme()
{
    gpgme_check_version(nullptr);

    if (gpgme_engine_check_version(GPGME_PROTOCOL_OpenPGP)) {
        qDebug() << "OpenPGP engine not available, check your installation.";
        return false;
    }

    return true;
}

