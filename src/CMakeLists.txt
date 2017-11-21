include_directories(${CMAKE_BINARY_DIR})

add_executable(symmy main.cpp decryptjob.cpp encryptjob.cpp job.cpp)

target_link_libraries(symmy
    Qt5::Widgets
    KF5::CoreAddons
    KF5::KIOCore
    KF5::I18n
    KF5::WidgetsAddons
    QGpgme)

install(TARGETS symmy DESTINATION ${KDE_INSTALL_BINDIR})
