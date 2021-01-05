#! /usr/bin/env sh

# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: 2017 Elvis Angelaccio <elvis.angelaccio@kde.org>

$XGETTEXT $(find . -name "*.cpp") -o $podir/symmy.pot
