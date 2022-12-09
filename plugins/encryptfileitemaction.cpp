/*
 * SPDX-FileCopyrightText: 2017 Elvis Angelaccio <elvis.angelaccio@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 */

#include "encryptfileitemaction.h"

#include <QAction>
#include <QApplication>
#include <QMimeDatabase>
#include <QUrl>

#include <KDialogJobUiDelegate>
#include <KFileItemListProperties>
#include <KIO/ApplicationLauncherJob>
#include <KLocalizedString>
#include <KPluginFactory>
#include <KRun>
#include <KService>

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
        if (mimeType == QLatin1String {"application/pgp-encrypted"} || mimeType == QLatin1String {"inode/directory"}) {
            return {};
        }
    }

    const auto icon = QIcon::fromTheme(QStringLiteral("document-encrypt"));

    auto encryptAction = new QAction {icon, i18nc("@action:inmenu Encrypt action in Dolphin context menu", "Encrypt"), parentWidget};
    connect(encryptAction, &QAction::triggered, this, [=]() {
        KService::Ptr service {new KService {QGuiApplication::applicationDisplayName(), QStringLiteral("symmy --encrypt %F"), QApplication::windowIcon().name()} };
        auto job = new KIO::ApplicationLauncherJob {service, parentWidget};
        job->setUrls(urls);
        job->setUiDelegate(new KDialogJobUiDelegate(KJobUiDelegate::AutoHandlingEnabled, parentWidget));
        job->start();
    });

    if (!fileItemInfos.supportsWriting() || !fileItemInfos.isLocal()) {
        encryptAction->setEnabled(false);
    }

    return {encryptAction};
}

#include "encryptfileitemaction.moc"
