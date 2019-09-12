#include <QMessageBox>

#include "widget.h"
#include "ui_widget.h"

#include "qhttp.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_testBtn_clicked()
{
    QHttp * qhttp = QHttp::getInstance();

    //qhttp->testHttp();

    QHttp::RequestParam data;
    data.url = "https://www.baidu.com/";

    //HeaderPair pair;
    //pair.first = "Accept-Encoding";
    //pair.second = "gzip, deflate";
    //data.headerList.append(pair);
    qhttp->sendRequest(data, [this](bool success, const QByteArray& data, const QString &error){
        if (success) {
            QMessageBox::warning(this, tr("My Application"),data, QMessageBox::Ok);
        } else {
            qDebug() << error;
        }
    });
}
