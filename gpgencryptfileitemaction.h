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

#ifndef GPGENCRYPTFILEITEMACTION_H
#define GPGENCRYPTFILEITEMACTION_H

#include "gpgfileitemaction.h"

#include <QUrl>

class QAction;
class QWidget;

class GpgEncryptFileItemAction : public GpgFileItemActionPlugin
{

Q_OBJECT

public:
    GpgEncryptFileItemAction(QObject *parent, const QVariantList &args);

    virtual QList<QAction*> actions(const KFileItemListProperties &fileItemInfos, QWidget *parentWidget) override;

private slots:
    void slotEncrypt(const QList<QUrl> &urls);

private:
    static void encrypt(const QString &fileName);
    static bool isAsciiArmor(gpgme_data_t cipherText);
};

#endif
