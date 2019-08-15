#include <QDebug>
#include <QStyleOption>
#include <QPainter>

#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    // 不影响标题栏拖拽
    ui->titleWidget->setWindowFlag(Qt::WindowTransparentForInput);
    // 不影响缩放（也可以在左右下留出borderWidth大小的边距）
    ui->contentWidget->setWindowFlag(Qt::WindowTransparentForInput);

    //设置窗口背景透明 为了圆角
    //setAttribute(Qt::WA_TranslucentBackground);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_minBtn_clicked()
{
    showMinimized();
}

void Widget::on_maxBtn_clicked()
{
    if (isMaximized()) {
        showNormal();
    } else {
        showMaximized();
    }
}

void Widget::on_closeBtn_clicked()
{
    close();
}
