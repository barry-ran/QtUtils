#include "widget.h"
#include <QApplication>
#include <QVersionNumber>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

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

    Widget w;
    w.show();

    return a.exec();
}
