#ifndef MYCOOKIEJAR_H
#define MYCOOKIEJAR_H

#include <QObject>
#include<QNetworkCookieJar>
#include<QNetworkCookie>


class mycookiejar : public QNetworkCookieJar
{
    Q_OBJECT

public:
    mycookiejar (QObject * parent = 0);
    ~mycookiejar();
    QList<QNetworkCookie>  getallCookies();
};
#endif // MYCOOKIEJAR_H
