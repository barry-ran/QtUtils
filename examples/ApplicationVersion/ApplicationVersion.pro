#-------------------------------------------------
#
# Project created by QtCreator 2019-09-03T20:45:10
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ApplicationVersion
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
        main.cpp \
        widget.cpp

HEADERS += \
        widget.h

FORMS += \
        widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


# 统一版本号入口,只修改这一个地方即可
VERSION_MAJOR = 2
VERSION_MINOR = 0
VERSION_PATCH = 4

# qmake变量的方式定义版本号
VERSION = $${VERSION_MAJOR}.$${VERSION_MINOR}.$${VERSION_PATCH}

win32 {
# 1. 通过qmake变量来设置信息（额外pri文件为了shi使用中文）
# 应用程序相关信息如果有中文的话，在windows平台下应该使用gbk编码，否则会乱码，所以放到单独的pri文件，
# 这样pro文件依然是utf8编码，不影响跨平台特性
include($$PWD/version/ApplicationVersion_win.pri)

# 2. 通过rc的方式的话，上面的变量就都没有效果了
# 定义宏方便rc中使用
DEFINES += VERSION_MAJOR=$${VERSION_MAJOR}
DEFINES += VERSION_MINOR=$${VERSION_MINOR}
DEFINES += VERSION_PATCH=$${VERSION_PATCH}
DEFINES += VERSION_RC_STR=\\\"$${VERSION_MAJOR}.$${VERSION_MINOR}.$${VERSION_PATCH}\\\"
# 指定rc
# RC_FILE = $$PWD/version/ApplicationVersion.rc
}

mac {
ICON = $$PWD/version/ApplicationVersion.icns
QMAKE_INFO_PLIST = $$PWD/version/Info.plist

# 定义目标命令（修改版本号字段）
plistupdate.commands = /usr/libexec/PlistBuddy -c \"Set :CFBundleShortVersionString $$VERSION\" \
-c \"Set :CFBundleVersion $$VERSION\" \
$$QMAKE_INFO_PLIST

# 增加额外目标
QMAKE_EXTRA_TARGETS += plistupdate
# 设置为前置依赖
PRE_TARGETDEPS += plistupdate
}

