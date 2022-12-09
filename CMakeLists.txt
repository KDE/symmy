# SPDX-License-Identifier: BSD-3-Clause
# SPDX-FileCopyrightText: 2017 Elvis Angelaccio <elvis.angelaccio@kde.org>

cmake_minimum_required (VERSION 3.16 FATAL_ERROR)
set (CMAKE_CXX_STANDARD 11)

set(SYMMY_VERSION 1.0.70)
project(symmy VERSION ${SYMMY_VERSION})

set(KF5_MIN_VERSION 5.71.0)
set(GPGME_MIN_VERSION 1.7.1)

find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMSetupVersion)
include(ECMQtDeclareLoggingCategory)
include(FeatureSummary)

find_package(Qt5 CONFIG REQUIRED
    Core
    Gui
    Widgets)

find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS
    DocTools
    KIO
    I18n
    WidgetsAddons)

find_package(Gpgmepp ${GPGME_MIN_VERSION} CONFIG REQUIRED)
find_package(QGpgme ${GPGME_MIN_VERSION} CONFIG REQUIRED)

ecm_setup_version(PROJECT
    VARIABLE_PREFIX SYMMY
    VERSION_HEADER symmyversion.h)

add_definitions(-DTRANSLATION_DOMAIN=\"symmy\")

add_subdirectory(desktop)
add_subdirectory(src)
add_subdirectory(plugins)

add_subdirectory(doc)
kdoctools_install(po)

ki18n_install(po)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
