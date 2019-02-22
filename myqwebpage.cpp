#include "myqwebpage.h"

myQWebPage ::myQWebPage(QObject * parent ):
QWebEnginePage(parent)
{
}

myQWebPage::~myQWebPage(){
}


bool myQWebPage::acceptNavigationRequest(const QUrl &url, NavigationType type, bool isMainFrame){
    //qDebug()<<type<<"===============url"<<url;
    if(type==QWebEnginePage::NavigationTypeLinkClicked){//如果是用户点击
        QString u = url.toString();
        if( u.indexOf("ad.")!=-1) return false;

        return  QWebEnginePage::acceptNavigationRequest(url,  type,  isMainFrame);
    }
    else if(type==QWebEnginePage::NavigationTypeReload){
        //qDebug()<<"reload singal";
        emit toreload();
        return false;
    }
    else    return  QWebEnginePage::acceptNavigationRequest(url,  type,  isMainFrame);
}


