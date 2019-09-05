# Qt应用部署与版本信息处理
发布Qt应用程序的时候，设置应用程序的版本号和应用信息
- 版本号入口统一，改一个地方，windows/mac都生效
- 方便在程序中进行版本号判断

## Qt对应设置程序信息的支持

Qt对应设置程序信息的支持分为两种方式，完全使用qmake变量的方式和原生配置文件的方式，两种方式是互斥的，
如果指定了使用原生配置文件的方式，则qmake变量方式相关的变量都不再生效。

### 完全使用qmake变量来设置

#### windows

[参考博客](https://blog.csdn.net/liang19890820/article/details/52702523)

在windows下，如果pro文件中指定了VERSION或RC_ICONS变量，则qmake会自动生成res文件链接到应用程序中，还会使用其它几个变量的值来填充res文件中的其它信息，如下所示：
```
# 版本号
VERSION = 1.0.2
RC_ICONS = app.ico
QMAKE_TARGET_PRODUCT = 产品名称
QMAKE_TARGET_COMPANY = 公司
QMAKE_TARGET_DESCRIPTION = 文件描述
QMAKE_TARGET_COPYRIGHT = 版权
```
以下是引用[Qt文档描述](https://doc.qt.io/qt-5.9/qmake-platform-notes.html#windows):
```
通过设置至少一个系统变量VERSION和RC_ICONS来触发qmake自动生成适当填充的.rc文件。生成的.rc文件会自动编译和链接。添加到.rc文件的元素由系统变量QMAKE_TARGET_COMPANY，QMAKE_TARGET_DESCRIPTION，QMAKE_TARGET_COPYRIGHT，QMAKE_TARGET_PRODUCT，RC_CODEPAGE，RC_ICONS，RC_LANG[详细说明](https://docs.microsoft.com/zh-cn/windows/win32/intl/language-identifier-constants-and-strings)和VERSION定义。

如果这些元素不够，qmake有两个系统变量RC_FILE和RES_FILE，它们直接指向外部创建的.rc或.res文件。通过设置其中一个变量，指定的文件将链接到EXE或DLL。

注意：如果设置了RC_FILE或RES_FILE，则阻止qmake生成.rc文件。在这种情况下，qmake不会对给定的.rc文件或.res或.o文件进行进一步更改; 与.rc文件生成有关的变量无效。
```

但是这种方式有一个缺点，QMAKE_TARGET_PRODUC等相关变量的值不能有中文，否则会乱码，除非设置pro文件编码为gbk，这又影响了pro文件的跨平台。所以折中的办法是在windows下将QMAKE_TARGET_PRODUC等相关变量放在单独的pri文件中，并编码为gbk，在pro文件中通过include引入。

如果Qt提供的这几个变量不能满足你的需求，你也可以通过指定原生配置文件来设置

#### macos

### 通过指定原生配置文件来设置（windows的rc文件，mac的plist文件）

#### windows

qmake有两个系统变量RC_FILE和RES_FILE，它们直接指向外部创建的.rc或.res文件。通过设置其中一个变量，指定的文件将链接到EXE或DLL。设置外部rc或者res文件后，上面使用qmake变量设置应用程序信息的方式就不再生效了。

```
RC_FILE = $$PWD/ApplicationVersion.rc
```
为了版本号入口统一，将版本号通过宏定义的方式放到version.h中，rc文件可以引用version.h中的宏

- pro中：
```
# 统一版本号入口
VERSION_MAJOR = 2
VERSION_MINOR = 0
VERSION_PATCH = 1

# 定义宏方便rc中使用
DEFINES += VERSION_MAJOR=$${VERSION_MAJOR}
DEFINES += VERSION_MINOR=$${VERSION_MINOR}
DEFINES += VERSION_PATCH=$${VERSION_PATCH}
```

- version.h中：
```
#ifndef VERSION_H
#define VERSION_H

#define _TOSTR(x)   #x
#define TOSTR(x)  _TOSTR(x)

// 字符串形式版本号，用于版本显示
/* the following are compile time version */
/* C++11 requires a space between literal and identifier */
#define VERSION_STR TOSTR(VERSION_MAJOR) "." TOSTR(VERSION_MINOR) "." TOSTR(VERSION_PATCH)

// rc形式版本号，用于windos rc文件中指定版本号
#define VERSION_RES VERSION_MAJOR,VERSION_MINOR,VERSION_PATCH

#endif // VERSION_H

```

- rc中：
```
#include "winres.h"
#include "version.h"

// 图标
IDI_ICON1       ICON      "ApplicationVersion.ico"

// 版本信息
VS_VERSION_INFO VERSIONINFO
 FILEVERSION VERSION_RES
 PRODUCTVERSION VERSION_RES
 FILEFLAGSMASK 0x3fL
#ifdef _DEBUG
 FILEFLAGS 0x1L
#else
 FILEFLAGS 0x0L
#endif
 FILEOS 0x40004L
 FILETYPE 0x1L
 FILESUBTYPE 0x0L
BEGIN
    BLOCK "StringFileInfo"
    BEGIN
        BLOCK "080404b0"
        BEGIN
            VALUE "CompanyName", "RanKun"
            VALUE "FileDescription", "文件描述"
            VALUE "FileVersion", VERSION_STR
            VALUE "LegalCopyright", "Copyright (C) RanKun 2018-2028. All rights reserved."
            VALUE "ProductName", "ApplicationVersion"
            VALUE "ProductVersion", VERSION_STR
        END
    END
    BLOCK "VarFileInfo"
    BEGIN
        VALUE "Translation", 0x804, 1200
    END
END

```

### 版本号判断
可以直接用Qt提供的QVersionNumber类
```
    // 通过qmake VERSION变量或者rc设置版本号和应用名称后，这里可以直接拿到
    qDebug() << a.applicationVersion();
    qDebug() << a.applicationName();

    QVersionNumber version = QVersionNumber::fromString(a.applicationVersion());
    qDebug() << version.majorVersion();
    qDebug() << version.minorVersion();
    qDebug() << version.microVersion();

    QVersionNumber versionTest1(2, 0, 3);
    QVersionNumber versionTest2(2, 0, 4);
    QVersionNumber versionTest3(2, 0, 5);

    qDebug() << version << versionTest1 << QVersionNumber::compare(version, versionTest1);
    qDebug() << version << versionTest2 << QVersionNumber::compare(version, versionTest2);
    qDebug() << version << versionTest3 << QVersionNumber::compare(version, versionTest3);

    // normalized去掉末尾的0来比较
    qDebug() << version.normalized() << versionTest1.normalized() << QVersionNumber::compare(version.normalized(), versionTest1.normalized());
    qDebug() << version.normalized() << versionTest2.normalized() << QVersionNumber::compare(version.normalized(), versionTest2.normalized());
    qDebug() << version.normalized() << versionTest3.normalized() << QVersionNumber::compare(version.normalized(), versionTest3.normalized());

    // 可以直接用>,<,>=,<=来比较
    if (versionTest3.normalized() > version.normalized()) {
        qDebug() << "find new version";
    }
```

## 扩展

### 根据git tag设置版本号

[参考文档](https://www.everythingfrontend.com/posts/app-version-from-git-tag-in-qt-qml.html)

## 参考文档

[qmake变量](https://doc.qt.io/qt-5/qmake-variable-reference.html)

[qt平台相关](https://doc.qt.io/qt-5.9/qmake-platform-notes.html)

[Qt设置应用程序图标](https://doc.qt.io/qt-5/appicon.html)

[apple开发文档](https://developer.apple.com/documentation/)

[Qt for macOS - Deployment](https://doc.qt.io/qt-5.9/osx-deployment.html)

[Qt for windows - Deployment](https://doc.qt.io/qt-5/windows-deployment.html)



