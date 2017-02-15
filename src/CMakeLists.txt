add_executable(symmy main.cpp decryptjob.cpp encryptjob.cpp job.cpp)

target_link_libraries(symmy
    Qt5::Widgets
    KF5::CoreAddons
    KF5::KIOCore
    KF5::I18n
    QGpgme)

install(TARGETS symmy DESTINATION ${KDE_INSTALL_BINDIR})
