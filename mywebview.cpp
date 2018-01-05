#include "mywebview.h"

myQWebview ::myQWebview(QWidget * parent ):
QWebView()
{
    settings()->setAttribute(QWebSettings::QWebSettings::JavascriptCanOpenWindows	,true);//
    settings()->setAttribute(QWebSettings::QWebSettings::JavascriptCanCloseWindows	,true);//
    myQWebPage *page = new myQWebPage;
    setPage(page);
    connect(page , SIGNAL(loadurl(QUrl)),this,SLOT(loadurl(QUrl)) );
    connect(page , SIGNAL(openurl(QUrl)),this,SLOT(openurl(QUrl)));
}

myQWebview::~myQWebview(){
}

QWebView *  myQWebview::	createWindow(QWebPage::WebWindowType type){
    QWebView *view = new QWebView();
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

void myQWebview::loadurl(QUrl u){
    this->load(u);
}
void myQWebview::openurl(QUrl u){
    QWebView *view = createWindow(QWebPage::WebBrowserWindow);
    view->load(u);
    view->show();
}
