#-------------------------------------------------
#
# Project created by QtCreator 2019-09-09T09:04:16
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CppEmbedPython
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

CONFIG += c++11

SOURCES += \
        cpppython.cpp \
        main.cpp \
        widget.cpp

HEADERS += \
        cpppython.h \
        widget.h

FORMS += \
        widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32 {
    # pyhton 路径
    PYTHON_PATH = "D:/Program Files (x86)/python37/"
    # 头文件
    INCLUDEPATH += $${PYTHON_PATH}include
    # 库文件
    LIBS += \
            -L$${PYTHON_PATH}libs -lpython37

    # 复制依赖库
    WIN_DEMO = \"$${PWD}/demo_test.py\"
    WIN_PYTHON = \"$${PYTHON_PATH}python37.dll\"
    WIN_DEST = $$OUT_PWD

    WIN_DEMO ~= s,/,\\,g
    WIN_PYTHON ~= s,/,\\,g
    WIN_DEST ~= s,/,\\,g

    QMAKE_POST_LINK += $$quote($$QMAKE_COPY $$WIN_PYTHON $$WIN_DEST $$escape_expand(\n\t))
    QMAKE_POST_LINK += $$quote($$QMAKE_COPY $$WIN_DEMO $$WIN_DEST $$escape_expand(\n\t))
}

DISTFILES += \
    demo_test.py
