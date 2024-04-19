QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    addsensore.cpp \
    clickableframe.cpp \
    editsensore.cpp \
    main.cpp \
    mainwindow.cpp \
    sensore.cpp \
    simulationsettings.cpp \
    temperatura.cpp \
    umidita.cpp \
    antincendio.cpp

HEADERS += \
    addsensore.h \
    clickableframe.h \
    editsensore.h \
    mainwindow.h \
    sensore.h \
    simulationsettings.h \
    temperatura.h \
    umidita.h \
    antincendio.h

FORMS += \
    addsensore.ui \
    editsensore.ui \
    mainwindow.ui \
    simulationsettings.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    sensori.json

RESOURCES += \
    rsc.qrc
