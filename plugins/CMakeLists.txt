kcoreaddons_add_plugin(encryptfileitemaction
    SOURCES encryptfileitemaction.cpp
    JSON encryptfileitemaction.json
    INSTALL_NAMESPACE "kf5/kfileitemaction")

target_link_libraries(encryptfileitemaction
    KF5::KIOWidgets
    KF5::I18n)

kcoreaddons_add_plugin(decryptfileitemaction
    SOURCES decryptfileitemaction.cpp
    JSON decryptfileitemaction.json
    INSTALL_NAMESPACE "kf5/kfileitemaction")

target_link_libraries(decryptfileitemaction
    KF5::KIOWidgets
    KF5::I18n)
