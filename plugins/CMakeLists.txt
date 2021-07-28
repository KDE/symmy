# SPDX-License-Identifier: BSD-3-Clause
# SPDX-FileCopyrightText: 2017 Elvis Angelaccio <elvis.angelaccio@kde.org>

kcoreaddons_add_plugin(encryptfileitemaction
    SOURCES encryptfileitemaction.cpp
    INSTALL_NAMESPACE "kf5/kfileitemaction")

target_link_libraries(encryptfileitemaction
    KF5::KIOWidgets
    KF5::I18n)

kcoreaddons_add_plugin(decryptfileitemaction
    SOURCES decryptfileitemaction.cpp
    INSTALL_NAMESPACE "kf5/kfileitemaction")

target_link_libraries(decryptfileitemaction
    KF5::KIOWidgets
    KF5::I18n)
