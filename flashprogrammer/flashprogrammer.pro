#-------------------------------------------------
#
# Project created by QtCreator 2012-10-26T13:26:25
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = flashprogrammer
TEMPLATE = app

include(./qextserialport/qextserialport.pri)

SOURCES += main.cpp\
        mainwindow.cpp \
    cm3updater.cpp \

HEADERS  += mainwindow.h \
    cm3updater.h \

FORMS    += mainwindow.ui

OTHER_FILES += \
    myico.rc \

LIBS += -lsetupapi

RESOURCES += \
    img.qrc

RC_FILE += myico.rc
