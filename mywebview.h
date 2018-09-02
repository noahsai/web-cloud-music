#ifndef MYQWEBVIEW_H
#define MYQWEBVIEW_H

#include <QObject>
#include <QWebEngineView>
#include <QWebEnginePage>
#include<QWebEngineProfile>
#include<QWebEngineSettings>
#include<QStandardPaths>
#include<myqwebpage.h>
#include<QWebEngineUrlRequestInterceptor>
class myQWebEngineUrlRequestInterceptor;

class myQWebview : public QWebEngineView
{
    Q_OBJECT

public:
    myQWebview (QWidget * parent = 0);
    ~myQWebview();
    QWebEngineView* newwindow();
private:
    QWebEngineView *	createWindow(QWebEnginePage::WebWindowType type);
private slots:
    void loadurl(QUrl);
    void openurl(QUrl);

signals:
    void foundmp3(QString url);
    void toreload();
};



class myQWebEngineUrlRequestInterceptor : public QWebEngineUrlRequestInterceptor
{
    Q_OBJECT

public:
    myQWebEngineUrlRequestInterceptor(QObject *parent = Q_NULLPTR);
    void interceptRequest(QWebEngineUrlRequestInfo &info);

signals:
    void foundmp3(QString url);
};


#endif // MYQWEBVIEW_H
