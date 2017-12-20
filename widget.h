#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include<QTimer>
#include<QWebElement>
#include<QWebPage>
#include<QWebFrame>
namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

public slots:
    void timeout();

private:
    Ui::Widget *ui;
    QTimer timer;
};

#endif // WIDGET_H
