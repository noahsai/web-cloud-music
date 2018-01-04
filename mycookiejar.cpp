#include "mycookiejar.h"



//===================================[mycookiejar]====================================================
mycookiejar ::mycookiejar(QObject * parent ):
QNetworkCookieJar( parent )
{
}

mycookiejar::~mycookiejar(){
}

QList<QNetworkCookie> mycookiejar::getallCookies(){
    QList<QNetworkCookie> list = allCookies();
    return list;
}


