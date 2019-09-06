# Qt应用部署与版本信息处理
发布Qt应用程序的时候，需要设置应用程序的版本号和应用信息，最终的解决方案有如下期望：
- 版本号入口统一，改一个地方，windows/mac都生效
- 方便在程序中进行版本号判断
- 支持中文
- 线上项目最好可以通过git tag自动读取版本号

## Qt对应设置程序信息的支持

在windows上，Qt对应设置程序信息的支持分为两种方式，完全使用qmake变量的方式和原生配置文件的方式，两种方式是互斥的，
如果指定了使用原生配置文件的方式，则qmake变量方式相关的变量都不再生效。

在mac上，只有一个qmake变量ICON来指定应用程序图标，没有提供更多其他信息相关的qmake变量，想要设置更多信息，只能通过QMAKE_INFO_PLIST来指定自定义的plist文件来添加更多描述信息。

### windows

#### 完全使用qmake变量来设置

[参考博客](https://blog.csdn.net/liang19890820/article/details/52702523)

在windows下，如果pro文件中指定了VERSION或RC_ICONS变量，则qmake会自动生成res文件链接到应用程序中，还会使用其它几个变量的值来填充res文件中的其它信息，如下所示：
```
# 统一版本号入口,只修改这一个地方即可
VERSION_MAJOR = 2
VERSION_MINOR = 0
VERSION_PATCH = 4

# qmake变量的方式定义版本号
VERSION = $${VERSION_MAJOR}.$${VERSION_MINOR}.$${VERSION_PATCH}

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

#### 通过指定原生配置文件来设置（windows的rc文件）

qmake有两个系统变量RC_FILE和RES_FILE，它们直接指向外部创建的.rc或.res文件。通过设置其中一个变量，指定的文件将链接到EXE或DLL。设置外部rc或者res文件后，上面使用qmake变量设置应用程序信息的方式就不再生效了。

```
RC_FILE = $$PWD/ApplicationVersion.rc
```
为了版本号入口统一，在qmake中将版本号通过宏定义的方式再次定义，rc文件可以引用qmake中的宏

- pro中：
```
# 统一版本号入口,只修改这一个地方即可
VERSION_MAJOR = 2
VERSION_MINOR = 0
VERSION_PATCH = 4

# qmake变量的方式定义版本号
VERSION = $${VERSION_MAJOR}.$${VERSION_MINOR}.$${VERSION_PATCH}

win32 {
# 2. 通过rc的方式的话，上面的变量就都没有效果了
# 定义宏方便rc中使用
DEFINES += VERSION_MAJOR=$${VERSION_MAJOR}
DEFINES += VERSION_MINOR=$${VERSION_MINOR}
DEFINES += VERSION_PATCH=$${VERSION_PATCH}
DEFINES += VERSION_RC_STR=\\\"$${VERSION_MAJOR}.$${VERSION_MINOR}.$${VERSION_PATCH}\\\"
# 指定rc
RC_FILE = $$PWD/version/ApplicationVersion.rc
}
```

- rc中：
```
#include "winres.h"

// 图标
IDI_ICON1       ICON      "ApplicationVersion.ico"

// 版本信息
VS_VERSION_INFO VERSIONINFO
 FILEVERSION VERSION_MAJOR,VERSION_MINOR,VERSION_PATCH
 PRODUCTVERSION VERSION_MAJOR,VERSION_MINOR,VERSION_PATCH
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
            VALUE "CompanyName", "bytedance"
            VALUE "FileDescription", "文件描述"
            VALUE "FileVersion", VERSION_RC_STR
            VALUE "LegalCopyright", "rankun copyright"
            VALUE "ProductName", "ApplicationVersion"
            VALUE "ProductVersion", VERSION_RC_STR
        END
    END
    BLOCK "VarFileInfo"
    BEGIN
        VALUE "Translation", 0x804, 1200
    END
END
```

### macos
在mac上，只有一个qmake变量ICON来指定应用程序图标，没有提供更多其他信息相关的qmake变量，想要设置更多信息，只能通过QMAKE_INFO_PLIST来指定自定义的plist文件来添加更多描述信息。下面是一个plist模版：
```
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
	<key>CFBundleDevelopmentRegion</key>
    <string>zh-Hans</string>
	<key>CFBundleExecutable</key>
    <string>@EXECUTABLE@</string>
    <key>CFBundleIconFile</key>
    <string>@ICON@</string>
	<key>CFBundleIdentifier</key>
	<string>rankun.AudioStream</string>
    <key>CFBundleGetInfoString</key>
    <string>Created by rankun</string>
	<key>CFBundleInfoDictionaryVersion</key>
	<string>6.0</string>
	<key>CFBundleName</key>
	<string>AudioStream</string>
	<key>CFBundlePackageType</key>
	<string>APPL</string>
	<key>CFBundleShortVersionString</key>
    <string>@SHORT_VERSION@</string>
	<key>CFBundleSupportedPlatforms</key>
	<array>
		<string>MacOSX</string>
	</array>
	<key>CFBundleVersion</key>
	<string>1</string>
	<key>LSMinimumSystemVersion</key>
	<string>10.14</string>
	<key>NSHumanReadableCopyright</key>
	<string>Copyright © 2019 rankun. All rights reserved.</string>
	<key>NSMainStoryboardFile</key>
	<string>Main</string>
    <key>NSPrincipalClass</key>
    <string>NSApplication</string>
    <key>NSSupportsAutomaticGraphicsSwitching</key>
    <true/>
</dict>
</plist>

```
可以看到plist模版中有几个@@包围的变量，这是qmake在plist中的内置变量，实际处理plist的时候会替换为实际值（但这几个变量远远不够）

[qmake在plist中的内置变量](https://doc.qt.io/archives/qt-5.6/qmake-variable-reference.html#QMAKE_INFO_PLIST)说明：
- @ICON@ 就是对应的qmake变量ICON
- @EXECUTABLE@ 可执行程序名称
- @SHORT_VERSION@ 短版本号
- @LIBRARY@ 库名称
- @TYPEINFO@ 类型信息

[plist关键字段说明](http://www.cocoachina.com/articles/17611)：
- CFBundleDevelopmentRegion 本地化语言
- CFBundleExecutable 可执行程序
- CFBundleIconFile 图标
- CFBundleIdentifier 应用唯一标识，采用类似java包名的方式
- CFBundleInfoDictionaryVersion plist文件的版本，如果plist文件有修改，通过这个版本号告知mac
- CFBundleName 简称
- CFBundlePackageType 类型 APPL表示应用程序
- CFBundleGetInfoString 应用简介
- CFBundleShortVersionString 应用发布版本号
- CFBundleVersion 编译版本号，格式和发布版本号一样
- CFBundleSupportedPlatforms 支持的平台
- LSMinimumSystemVersion 最小系统要求
- NSHumanReadableCopyright 版权信息
- CFBundleDisplayName 安装后显示名称
- NSMainStoryboardFile 主Storyboard文件名称，干啥用的？
- NSPrincipalClass 主类的名字


显然这几个变量是不够用的，我们可能还需要增加自己的键值对，而且@SHORT_VERSION@是短版本号，只有两位，所以很多时候我们需要自己修改plist中对键值对，
这就需要[使用QMAKE_EXTRA_TARGETS自定义目标](https://doc.qt.io/archives/qt-5.6/qmake-advanced-usage.html#adding-custom-targets)了：
```
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
```

qt默认plist模版 /Users/bytedance/Qt5.12.4/5.12.4/clang_64/mkspecs/macx-clang/Info.plist.app

### 版本号判断
可以直接用Qt提供的QVersionNumber类
```
    // windows下通过qmake VERSION变量或者rc设置版本号和应用名称后，这里可以直接拿到
    // mac下拿到的是CFBundleVersion的值
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

[Qt for windows - Deployment](https://doc.qt.io/qt-5/windows-deployment.html)

### macos

[apple开发文档](https://developer.apple.com/documentation/)

[qt默认plist目录](https://wiki.qt.io/MacOS_application_without_menu_bar)

[Qt for macOS - Deployment](https://doc.qt.io/qt-5.9/osx-deployment.html)

[Xcode中的Info.plist字段列表详解](http://www.cocoachina.com/articles/17611)

[plist苹果官方文档](https://developer.apple.com/library/archive/documentation/General/Reference/InfoPlistKeyReference/Articles/AboutInformationPropertyListFiles.html#//apple_ref/doc/uid/TP40009254-SW4)

[ios info.plist 详解](http://www.voidcn.com/article/p-ceuvzvzi-mw.html)

[借助QMAKE_EXTRA_TARGETS修改plist方法1](https://forum.qt.io/topic/80339/how-can-i-set-and-get-the-application-version/7)

[借助QMAKE_EXTRA_TARGETS修改plist方法2](https://forum.qt.io/topic/6176/qmake-qmake_info_plist-and-typeinfo/2)





