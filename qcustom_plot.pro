#-------------------------------------------------
#
# Project created by QtCreator 2017-08-07T09:59:48
#
#-------------------------------------------------

QT       += core gui opengl quick
greaterThan(QT_MAJOR_VERSION, 4): QT += printsupport

TARGET = qcustom_plot
TEMPLATE = lib
CONFIG += qt staticlib debug_and_release build_all

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS QCUSTOMPLOT_USE_OPENGL QCUSTOMPLOT_COMPILE_LIBRARY

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        qcustomplot.cpp

HEADERS += \
        qcustomplot.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
