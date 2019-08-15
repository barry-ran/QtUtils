#include "widget.h"
#include <QApplication>

#include "framelesswindowhelper.h"

int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication a(argc, argv);    

    FramelessWindowHelper<Widget> w;
    //Widget w;
    //w.setResizeable(true);
    //w.setTitleHeight(300);
    w.setResizeable(false);
    w.show();

    return a.exec();
}
