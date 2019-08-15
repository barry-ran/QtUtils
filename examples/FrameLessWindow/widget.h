#ifndef WIDGET_H
#define WIDGET_H

#include <QScreen>
#include <QWidget>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();    

private slots:
    void on_minBtn_clicked();

    void on_maxBtn_clicked();

    void on_closeBtn_clicked();

private:
    Ui::Widget *ui;
};

#endif // WIDGET_H
