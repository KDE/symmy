/*
 * SPDX-FileCopyrightText: 2017 Elvis Angelaccio <elvis.angelaccio@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 */

#include "decryptfileitemaction.h"

#include <QAction>
#include <QApplication>
#include <QUrl>

#include <KDialogJobUiDelegate>
#include <KFileItemListProperties>
#include <KIO/ApplicationLauncherJob>
#include <KLocalizedString>
#include <KPluginFactory>
#include <KService>

K_PLUGIN_FACTORY_WITH_JSON(DecryptFileItemActionFactory, "decryptfileitemaction.json", registerPlugin<DecryptFileItemAction>();)

DecryptFileItemAction::DecryptFileItemAction(QObject *parent, const QVariantList&)
    : KAbstractFileItemActionPlugin {parent}
{}

QList<QAction*> DecryptFileItemAction::actions(const KFileItemListProperties &fileItemInfos, QWidget *parentWidget)
{
    const auto icon = QIcon::fromTheme(QStringLiteral("document-decrypt"));

    auto decryptionAction = new QAction {icon, i18nc("@action:inmenu Decrypt action in Dolphin context menu", "Decrypt"), parentWidget};
    connect(decryptionAction, &QAction::triggered, this, [=]() {
        KService::Ptr service {new KService {QGuiApplication::applicationDisplayName(), QStringLiteral("symmy --decrypt %F"), QApplication::windowIcon().name()} };
        auto job = new KIO::ApplicationLauncherJob {service, parentWidget};
        job->setUrls(fileItemInfos.urlList());
        job->setUiDelegate(new KDialogJobUiDelegate(KJobUiDelegate::AutoHandlingEnabled, parentWidget));
        job->start();
    });

    if (!fileItemInfos.supportsWriting() || !fileItemInfos.isLocal()) {
        decryptionAction->setEnabled(false);
    }

    return {decryptionAction};
}

#include "decryptfileitemaction.moc"
