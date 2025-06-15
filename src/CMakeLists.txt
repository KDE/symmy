# SPDX-License-Identifier: BSD-3-Clause
# SPDX-FileCopyrightText: 2017 Elvis Angelaccio <elvis.angelaccio@kde.org>

include_directories(${CMAKE_BINARY_DIR})

set(symmy_SRCS
    main.cpp
    compositejob.cpp
    decryptjob.cpp
    encryptjob.cpp
    job.cpp)

ecm_qt_declare_logging_category(symmy_SRCS
    HEADER symmydebug.h
    IDENTIFIER SYMMY
    CATEGORY_NAME org.kde.symmy)

add_executable(symmy ${symmy_SRCS})

target_link_libraries(symmy
    Qt6::Widgets
    KF6::CoreAddons
    KF6::KIOCore
    KF6::I18n
    KF6::WidgetsAddons
    QGpgmeQt6
    gpg-error)

install(TARGETS symmy DESTINATION ${KDE_INSTALL_BINDIR})
