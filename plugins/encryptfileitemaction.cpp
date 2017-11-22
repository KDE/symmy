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

#include "encryptfileitemaction.h"

#include <QAction>
#include <QMimeDatabase>
#include <QUrl>

#include <KFileItemListProperties>
#include <KLocalizedString>
#include <KRun>
#include <KPluginFactory>

K_PLUGIN_FACTORY_WITH_JSON(EncryptFileItemActionFactory, "encryptfileitemaction.json", registerPlugin<EncryptFileItemAction>();)

EncryptFileItemAction::EncryptFileItemAction(QObject *parent, const QVariantList&)
    : KAbstractFileItemActionPlugin {parent}
{}

QList<QAction*> EncryptFileItemAction::actions(const KFileItemListProperties &fileItemInfos, QWidget *parentWidget)
{
    const auto urls = fileItemInfos.urlList();
    QMimeDatabase mimeDb;
    for (const auto &url : urls) {
        const auto mimeType = mimeDb.mimeTypeForUrl(url).name();
        if (mimeType == QLatin1String {"application/pgp-encrypted"} or mimeType == QLatin1String {"inode/directory"}) {
            return {};
        }
    }

    const auto icon = QIcon::fromTheme(QStringLiteral("document-encrypt"));

    auto encryptAction = new QAction {icon, i18nc("@action:inmenu Encrypt action in Dolphin context menu", "Encrypt"), parentWidget};
    connect(encryptAction, &QAction::triggered, this, [=]() {
        KRun::run(QStringLiteral("symmy --encrypt %F"), urls, parentWidget);
    });

    if (not fileItemInfos.supportsWriting() or not fileItemInfos.isLocal()) {
        encryptAction->setEnabled(false);
    }

    return {encryptAction};
}

#include "encryptfileitemaction.moc"
