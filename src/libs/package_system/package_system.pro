include(../../../novel-downloader.pri)
QT += widgets

DEFINES += PACKAGE_SYSTEM_LIBRARY

TARGET = package_system
TEMPLATE = lib
CONFIG += shared dll

DLLDESTDIR = $$build_bin_dir

win32{
    DESTDIR = $$build_base_dir/lib
}

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        package_manager.cpp \
    package_spec.cpp \
    package_interface.cpp \
    novel_website_package.cpp \
    novel_output_package.cpp

HEADERS += \
        package_manager.h \
    package_system_global.h \
    package_spec.h \
    package_interface.h \
    novel_website_package.h \
    novel_output_package.h
