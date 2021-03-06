#-------------------------------------------------
#
# Project created by QtCreator 2019-02-08T23:03:42
#
#-------------------------------------------------

QT       += core gui sql svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = Cicadia
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

PRECOMPILED_HEADER = precomp.h

CONFIG += c++11 precompile_header

SOURCES += \
        main.cpp \
        UI/mainwindow.cpp \
    UI/plotdialog.cpp \
    UI/datatab.cpp \
    UI/optionsdialog.cpp \
    Models/datatablemodel.cpp \
    Models/subjectstreemodel.cpp \
    Models/statisticstablemodel.cpp \
    Algorithms/algorithmbase.cpp \
    Algorithms/cosinor.cpp \
    Algorithms/compare.cpp \
    Algorithms/populationcosinor.cpp \
    database.cpp \
    dataset.cpp \
    UI/qcustomplot.cpp \
    Algorithms/histogram.cpp

HEADERS += \
        UI/mainwindow.h \
    UI/plotdialog.h \
    UI/datatab.h \
    UI/optionsdialog.h \
    Models/datatablemodel.h \
    Models/subjectstreemodel.h \
    Models/statisticstablemodel.h \
    Algorithms/algorithmbase.h \
    Algorithms/cosinor.h \
    Algorithms/compare.h \
    Algorithms/populationcosinor.h \
    database.h \
    dataset.h \
    UI/qcustomplot.h \
    precomp.h \
    Algorithms/histogram.h

FORMS += \
        UI/mainwindow.ui \
    UI/plotdialog.ui \
    UI/datatab.ui \
    UI/optionsdialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    README.md \
    LICENSE
