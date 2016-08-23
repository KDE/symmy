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

#include "gpgencryptfileitemaction.h"

#include <QAction>
#include <QDebug>
#include <QMimeDatabase>
#include <QSaveFile>
#include <QUrl>

#include <KFileItemListProperties>
#include <KLocalizedString>
#include <KPluginFactory>

K_PLUGIN_FACTORY_WITH_JSON(GpgEncryptFileItemActionFactory, "gpgencryptfileitemaction.json", registerPlugin<GpgEncryptFileItemAction>();)

GpgEncryptFileItemAction::GpgEncryptFileItemAction(QObject *parent, const QVariantList&)
    : GpgFileItemActionPlugin(parent)
{}

QList<QAction*> GpgEncryptFileItemAction::actions(const KFileItemListProperties &fileItemInfos, QWidget *parentWidget)
{
    const auto urls = fileItemInfos.urlList();
    for (const auto &url : urls) {
        const auto mimeType = QMimeDatabase().mimeTypeForUrl(url).name();
        if (mimeType == QLatin1String("application/pgp-encrypted") or mimeType == QLatin1String("inode/directory")) {
            return {};
        }
    }

    const auto icon = QIcon::fromTheme(QStringLiteral("document-encrypt"));

    auto encryptAction = new QAction(icon, i18nc("@action:inmenu Encrypt action in Dolphin context menu", "Encrypt"), parentWidget);
    connect(encryptAction, &QAction::triggered, this, [=]() {
        slotEncrypt(urls);
    });

    if (not fileItemInfos.supportsWriting() or not fileItemInfos.isLocal()) {
        encryptAction->setEnabled(false);
    }

    return {encryptAction};
}

void GpgEncryptFileItemAction::slotEncrypt(const QList<QUrl> &urls)
{
    if (not initGpgme()) {
        return;
    }

    for (const auto &url : urls) {
        encrypt(url.toLocalFile());
    }
}

void GpgEncryptFileItemAction::encrypt(const QString &fileName)
{
    QFile file(fileName);
    if (not file.open(QIODevice::ReadOnly | QIODevice::Unbuffered)) {
        qDebug() << "Could not open plaintext file" << fileName;
        return;
    }

    gpgme_ctx_t context;
    gpgme_new(&context);

    gpgme_data_t plainText;
    gpgme_data_new(&plainText);

    gpgme_data_t cipherText;
    gpgme_data_new(&cipherText);

    if (context and plainText and cipherText) {
        if (not fileToData(file, plainText)) {
            qDebug() << "Failed to read plaintext data from file" << file.fileName();
        } else if (gpgme_op_encrypt(context, nullptr, static_cast<gpgme_encrypt_flags_t>(0), plainText, cipherText)) {
            qDebug() << "Encryption failed";
        } else {
            auto ciphertextFilename = fileName;
            // FIXME: why does gpgme_get_armor(context) return false if 'armor' is set in gpg.conf?
            ciphertextFilename += gpgme_get_armor(context) ? QLatin1String(".asc") : QLatin1String(".gpg");

            if (not dataToFile(cipherText, ciphertextFilename)) {
                qDebug() << "Failed to save ciphertext to disk";
            }
        }
    }

    gpgme_data_release(plainText);
    gpgme_data_release(cipherText);
    gpgme_release(context);
}

#include "gpgencryptfileitemaction.moc"
