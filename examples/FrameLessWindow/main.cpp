#include "widget.h"
#include <QApplication>

#ifdef Q_OS_WIN
#include "framelesswindowhelper.h"
#endif

int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication a(argc, argv);

#ifdef Q_OS_WIN
    FramelessWindowHelper<Widget> w;
    w.setResizeable(false);
#else
    Widget w;
#endif
    w.show();

    return a.exec();
}
