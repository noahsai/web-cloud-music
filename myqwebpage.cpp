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
        if( url.toString().indexOf("ad.")!=-1) return false;
//        if( url.toString().indexOf("opencache")!=-1) {
//            opencache();
//            qDebug()<<"emit opencache";
//            return false;
//        }//不知为何无效
        if(isMainFrame){ //如果不是在本窗口的连接
            //qDebug()<<"新窗口打开";
            emit openurl(url);
        }else{
            //qDebug()<<"直接打开";
            emit  loadurl(url);
        }
        return false;

    }
    else    return  QWebEnginePage::acceptNavigationRequest(url,  type,  isMainFrame);
}

