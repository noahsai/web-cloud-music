#include "myqwebpage.h"

myQWebPage ::myQWebPage(QObject * parent ):
QWebPage(parent)
{
}

myQWebPage::~myQWebPage(){
}


bool myQWebPage::acceptNavigationRequest(QWebFrame *frame, const QNetworkRequest &request, NavigationType type){
    if(type==0){//如果是用户点击
        if(frame!=mainFrame()){ //如果不是在本窗口的连接
            emit openurl(request.url());
        }else{
            emit  loadurl(request.url());
        }

    }
    return QWebPage::acceptNavigationRequest(frame, request, type);
}

