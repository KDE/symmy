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

#include "decryptfileitemaction.h"

#include <QAction>
#include <QDebug>
#include <QUrl>

#include <KFileItemListProperties>
#include <KLocalizedString>
#include <KPluginFactory>
#include <KRun>

K_PLUGIN_FACTORY_WITH_JSON(DecryptFileItemActionFactory, "decryptfileitemaction.json", registerPlugin<DecryptFileItemAction>();)

DecryptFileItemAction::DecryptFileItemAction(QObject *parent, const QVariantList&)
    : KAbstractFileItemActionPlugin {parent}
{}

QList<QAction*> DecryptFileItemAction::actions(const KFileItemListProperties &fileItemInfos, QWidget *parentWidget)
{
    const auto icon = QIcon::fromTheme(QStringLiteral("document-decrypt"));

    auto decryptionAction = new QAction {icon, i18nc("@action:inmenu Decrypt action in Dolphin context menu", "Decrypt"), parentWidget};
    connect(decryptionAction, &QAction::triggered, this, [=]() {
        KRun::run(QStringLiteral("symmy --decrypt %f"), fileItemInfos.urlList(), parentWidget);
    });

    if (not fileItemInfos.supportsWriting() or not fileItemInfos.isLocal()) {
        decryptionAction->setEnabled(false);
    }

    return {decryptionAction};
}

#include "decryptfileitemaction.moc"
