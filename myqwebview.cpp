#include "myqwebview.h"

myQWebview ::myQWebview(QWidget * parent ):
QWebView()
{
    settings()->setAttribute(QWebSettings::QWebSettings::JavascriptCanOpenWindows	,true);//
    settings()->setAttribute(QWebSettings::QWebSettings::JavascriptCanCloseWindows	,true);//
}

myQWebview::~myQWebview(){
}

QWebView *  myQWebview::	createWindow(QWebPage::WebWindowType type){
    QWebView *view = new QWebView;
   view-> settings()->setAttribute(QWebSettings::QWebSettings::JavascriptCanOpenWindows	,true);//
    view->settings()->setAttribute(QWebSettings::QWebSettings::JavascriptCanCloseWindows	,true);//
    mycookiejar *jar = new mycookiejar;
    view->page()->networkAccessManager()->setCookieJar(jar);
    connect(view,SIGNAL(loadFinished(bool)),this,SLOT(mvcookie()));
    connect(view->page(),SIGNAL(windowCloseRequested()),view,SLOT(deleteLater()));
    return view;
}

void myQWebview::mvcookie(){
    QWebView *view = qobject_cast<QWebView *>(sender());
    mycookiejar * jar =( mycookiejar * ) (view->page()->networkAccessManager()->cookieJar() );
    QList  <QNetworkCookie  >   list =jar->getallCookies();
  //  qDebug()<<list.length()<<list;
    foreach (QNetworkCookie one, list) {
        this->page()->networkAccessManager()->cookieJar()->insertCookie(one);
    }
}
QWebView* myQWebview::newwindow(){
    return createWindow(QWebPage::WebBrowserWindow);
}
