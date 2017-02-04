/*
 * Copyright (C) 2016 Elvis Angelaccio <elvis.angelaccio@kde.org>
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

#include "gpgdecryptfileitemaction.h"

#include <QAction>
#include <QDebug>
#include <QMimeDatabase>
#include <QSaveFile>
#include <QUrl>

#include <KFileItemListProperties>
#include <KLocalizedString>
#include <KPluginFactory>

K_PLUGIN_FACTORY_WITH_JSON(GpgDecryptFileItemActionFactory, "gpgdecryptfileitemaction.json", registerPlugin<GpgDecryptFileItemAction>();)

GpgDecryptFileItemAction::GpgDecryptFileItemAction(QObject *parent, const QVariantList&)
    : GpgFileItemActionPlugin(parent)
{}

QList<QAction*> GpgDecryptFileItemAction::actions(const KFileItemListProperties &fileItemInfos, QWidget *parentWidget)
{
    const auto icon = QIcon::fromTheme(QStringLiteral("document-decrypt"));

    auto decryptionAction = new QAction(icon, i18nc("@action:inmenu Decrypt action in Dolphin context menu", "Decrypt"), parentWidget);
    connect(decryptionAction, &QAction::triggered, this, [=]() {
        slotDecrypt(fileItemInfos.urlList());
    });

    if (not fileItemInfos.supportsWriting() or not fileItemInfos.isLocal()) {
        decryptionAction->setEnabled(false);
    }

    return {decryptionAction};
}

void GpgDecryptFileItemAction::slotDecrypt(const QList<QUrl> &urls)
{
    if (not initGpgme()) {
        return;
    }

    for (const auto &url : urls) {
        decrypt(url.toLocalFile());
    }
}

void GpgDecryptFileItemAction::decrypt(const QString &fileName)
{
    QFile file(fileName);
    if (not file.open(QIODevice::ReadOnly | QIODevice::Unbuffered)) {
        qDebug() << "Could not open ciphertext file" << fileName;
        return;
    }

    gpgme_ctx_t context;
    gpgme_new(&context);

    gpgme_data_t cipherText;
    gpgme_data_new(&cipherText);

    gpgme_data_t plainText;
    gpgme_data_new(&plainText);

    if (context and plainText and cipherText) {
        if (not fileToData(file, cipherText)) {
            qDebug() << "Failed to read ciphertext data from file" << file.fileName();
        } else if (gpgme_op_decrypt(context, cipherText, plainText)) {
            qDebug() << "Decryption failed";
        } else if (not dataToFile(plainText, plaintextFilename(fileName))) {
            qDebug() << "Failed to save ciphertext to disk";
        }
    }

    gpgme_data_release(cipherText);
    gpgme_data_release(plainText);
    gpgme_release(context);
}

QString GpgDecryptFileItemAction::plaintextFilename(const QString &ciphertextFilename)
{
    auto fileName = ciphertextFilename;
    const auto pgpSuffixes = QMimeDatabase().mimeTypeForName(QStringLiteral("application/pgp-encrypted")).suffixes();
    for (const auto &suffix : pgpSuffixes) {
        if (fileName.endsWith(suffix)) {
            fileName.chop(suffix.length() + 1); // dot is not included in the suffix
            break;
        }
    }

    return fileName;
}

#include "gpgdecryptfileitemaction.moc"
