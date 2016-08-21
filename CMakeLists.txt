project(encryption-plugin)

cmake_minimum_required (VERSION 2.8.12 FATAL_ERROR)

find_package(ECM REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(FeatureSummary)

find_package(Qt5 CONFIG REQUIRED
  Core
  Gui
  Widgets)

find_package(KF5 REQUIRED COMPONENTS
  KIO
  I18n)

find_package(Gpgme REQUIRED)

# required by gpgme
add_definitions(-D_FILE_OFFSET_BITS=64)

kcoreaddons_add_plugin(gpgencryptfileitemaction
    SOURCES gpgencryptfileitemaction.cpp gpgfileitemaction.cpp
    JSON gpgencryptfileitemaction.json
    INSTALL_NAMESPACE "kf5/kfileitemaction")

target_link_libraries(gpgencryptfileitemaction
    KF5::KIOWidgets
    KF5::I18n
    ${GPGME_VANILLA_LIBRARIES})

kcoreaddons_add_plugin(gpgdecryptfileitemaction
    SOURCES gpgdecryptfileitemaction.cpp gpgfileitemaction.cpp
    JSON gpgdecryptfileitemaction.json
    INSTALL_NAMESPACE "kf5/kfileitemaction")

target_link_libraries(gpgdecryptfileitemaction
    KF5::KIOWidgets
    KF5::I18n
    ${GPGME_VANILLA_LIBRARIES})


feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
