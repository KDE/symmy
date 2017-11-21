cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)
set (CMAKE_CXX_STANDARD 11)

set(SYMMY_VERSION 0.70.0)
project(symmy VERSION ${SYMMY_VERSION})

find_package(ECM REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})


include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMSetupVersion)
include(ECMQtDeclareLoggingCategory)
include(FeatureSummary)

# ecm >= 5.31 disables alternative operators, but we use them.
string(REPLACE "-fno-operator-names" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

find_package(Qt5 CONFIG REQUIRED
    Core
    Gui
    Widgets)

find_package(KF5 REQUIRED COMPONENTS
    KIO
    I18n
    WidgetsAddons)

find_package(Gpgmepp "1.7.1" CONFIG REQUIRED)
find_package(QGpgme "1.7.1" CONFIG REQUIRED)

ecm_setup_version(PROJECT
    VARIABLE_PREFIX SYMMY
    VERSION_HEADER symmyversion.h)

add_subdirectory(src)
add_subdirectory(plugins)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
