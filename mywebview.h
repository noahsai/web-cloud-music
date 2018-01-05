#ifndef MYQWEBVIEW_H
#define MYQWEBVIEW_H

#include <QObject>
#include<QWebView>
#include<QStandardPaths>
#include<QNetworkCookieJar>
#include<QNetworkCookie>
#include<mycookiejar.h>
#include<myqwebpage.h>
class myQWebview : public QWebView
{
    Q_OBJECT

public:
    myQWebview (QWidget * parent = 0);
    ~myQWebview();
    QWebView* newwindow();
private:
    QWebView *	createWindow(QWebPage::WebWindowType type);
private slots:
    void loadurl(QUrl);
    void openurl(QUrl);
    void mvcookie();
};
#endif // MYQWEBVIEW_H
