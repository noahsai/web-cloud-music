#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{

    ui->setupUi(this);
    ui->webView->settings()->setAttribute(QWebSettings::JavascriptEnabled, true);
   ui->webView->settings()->setAttribute(QWebSettings::LocalStorageEnabled, true);
    ui->webView->settings()->setAttribute(QWebSettings::PluginsEnabled, true);//
    timer.setInterval(1000);
    timer.setSingleShot(false);
    timer.start();
    connect(&timer,SIGNAL(timeout()),this,SLOT(timeout()));
}

Widget::~Widget()
{
    delete ui;
}


void Widget::timeout()
{
    QDateTime time = QDateTime::currentDateTime();//获取系统现在的时间
    QString update = time.toString("hh:mm:ss"); //设置显
    QString result;
    QWebFrame* frame=ui->webView->page()->mainFrame();
    QWebElement e = frame->findFirstElement(".z-sel");
    result = e.evaluateJavaScript("this.value").toString();
    if(result.isEmpty()) result=update+":empty";
    ui->label->setText(result);
}
