#//////////////////////////////////////////
# тестовая работа к занятиям https://youtu.be/cx9WUIS-y9I & https://youtu.be/PLXcsioQA9M
#//////////////////////////////////////////

QT       += core gui widgets

TARGET = ogltest11-12
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

CONFIG += c++17

SOURCES += \
        engine3d/eye.cpp \
        engine3d/material.cpp \
        engine3d/materiallibrary.cpp \
        engine3d/object3d.cpp \
        engine3d/object3delement.cpp \
        engine3d/object3dgroup.cpp \
        engine3d/skybox.cpp \
        engine3d/transformational.cpp \
        main.cpp \
        oglwidget.cpp

HEADERS += \
        engine3d/eye.h \
        engine3d/material.h \
        engine3d/materiallibrary.h \
        engine3d/object3d.h \
        engine3d/object3delement.h \
        engine3d/object3dgroup.h \
        engine3d/skybox.h \
        engine3d/transformational.h \
        oglwidget.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    models.qrc \
    shaders.qrc \
    textures.qrc

win32-g* {
LIBS += -lopengl32
}

win32-msvc* {
LIBS += lopengl32.lib
}

DISTFILES +=
