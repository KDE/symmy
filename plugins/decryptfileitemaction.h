/*
 * SPDX-FileCopyrightText: 2017 Elvis Angelaccio <elvis.angelaccio@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 */

#pragma once

#include <KAbstractFileItemActionPlugin>

#include <QUrl>

class QAction;
class QWidget;

class DecryptFileItemAction : public KAbstractFileItemActionPlugin
{
    Q_OBJECT

public:
    DecryptFileItemAction(QObject *parent, const QVariantList &args);

    QList<QAction*> actions(const KFileItemListProperties &fileItemInfos, QWidget *parentWidget) override;
};
