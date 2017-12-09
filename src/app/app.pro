include(../../novel-downloader.pri)

QT       += core gui widgets concurrent

TARGET = novel_downloader
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


INCLUDEPATH += ../libs
LIBS +=  -L$${build_base_dir}/lib -lpackage_system

DESTDIR = $$build_bin_dir

RC_ICONS = ./assert/icon/book.ico

#LIBS += -L$$build_lib_dir -lprogress_util

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    util.cpp \
    download_manager.cpp \
    download_task.cpp

HEADERS += \
        mainwindow.h \
    util.h \
    download_manager.h \
    download_task.h

FORMS += \
        mainwindow.ui

RESOURCES += \
    media.qrc
