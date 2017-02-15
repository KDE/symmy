project(symmy)

cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)
set (CMAKE_CXX_STANDARD 11)

find_package(ECM REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(ECMMarkNonGuiExecutable)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(FeatureSummary)

# ecm >= 5.31 disables alternative operators, but we use them.
string(REPLACE "-fno-operator-names" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

find_package(Qt5 CONFIG REQUIRED
  Core
  Gui
  Widgets)

find_package(KF5 REQUIRED COMPONENTS
  KIO
  I18n)

find_package(Gpgmepp "1.7.1" CONFIG REQUIRED)
find_package(QGpgme "1.7.1" CONFIG REQUIRED)

add_subdirectory(src)
add_subdirectory(plugins)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
