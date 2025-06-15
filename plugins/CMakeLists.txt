# SPDX-License-Identifier: BSD-3-Clause
# SPDX-FileCopyrightText: 2017 Elvis Angelaccio <elvis.angelaccio@kde.org>

kcoreaddons_add_plugin(encryptfileitemaction
    SOURCES encryptfileitemaction.cpp
    INSTALL_NAMESPACE "kf6/kfileitemaction")

target_link_libraries(encryptfileitemaction
    KF6::KIOWidgets
    KF6::I18n)

kcoreaddons_add_plugin(decryptfileitemaction
    SOURCES decryptfileitemaction.cpp
    INSTALL_NAMESPACE "kf6/kfileitemaction")

target_link_libraries(decryptfileitemaction
    KF6::KIOWidgets
    KF6::I18n)
