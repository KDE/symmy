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
    Qt5::Widgets
    KF5::CoreAddons
    KF5::KIOCore
    KF5::I18n
    KF5::WidgetsAddons
    QGpgme
    gpg-error)

install(TARGETS symmy DESTINATION ${KDE_INSTALL_BINDIR})
