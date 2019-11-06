#-------------------------------------------------
#
# Project created by QtCreator 2017-02-08T15:57:48
#
#-------------------------------------------------

QT       += core gui sql xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ship
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


SOURCES += main.cpp\
        mainwindow.cpp \
    model.cpp \
    dialogcods.cpp \
    tabwidget.cpp \
    formship.cpp \
    formret.cpp \
    models.cpp \
    formpartel.cpp \
    modelreadonly.cpp \
    tableview.cpp \
    modelwarehouse.cpp \
    formwarehouse.cpp \
    presencewidget.cpp \
    modelpresence.cpp \
    formpresenceel.cpp \
    db/dbdelegate.cpp \
    db/dblogin.cpp \
    db/dbmapper.cpp \
    db/dbtablemodel.cpp \
    db/dbviewer.cpp

HEADERS  += mainwindow.h \
    model.h \
    dialogcods.h \
    tabwidget.h \
    formship.h \
    formret.h \
    models.h \
    formpartel.h \
    modelreadonly.h \
    tableview.h \
    modelwarehouse.h \
    formwarehouse.h \
    presencewidget.h \
    modelpresence.h \
    formpresenceel.h \
    db/dbdelegate.h \
    db/dblogin.h \
    db/dbmapper.h \
    db/dbtablemodel.h \
    db/dbviewer.h

FORMS    += mainwindow.ui \
    dialogcods.ui \
    formship.ui \
    formret.ui \
    formpartel.ui \
    formwarehouse.ui \
    presencewidget.ui \
    formpresenceel.ui \
    db/dblogin.ui

RESOURCES += \
    res.qrc

RC_FILE = ico.rc

INCLUDEPATH+=/usr/include/xlslib
LIBS+= -lxls

#INCLUDEPATH+=C:\Qt_progs\xlslib
#LIBS+= C:\Qt_progs\xlslib\libxlslib_static.a

DISTFILES += \
    ico.rc

