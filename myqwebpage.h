#ifndef myQWebPage_H
#define myQWebPage_H

#include <QObject>
#include <QWebPage>
#include<QNetworkRequest>
class myQWebPage : public QWebPage
{
    Q_OBJECT

public:
    myQWebPage (QObject * parent = 0);
    ~myQWebPage();
signals:
    void openurl(QUrl);
    void loadurl(QUrl);
private:
    bool acceptNavigationRequest(QWebFrame *frame, const QNetworkRequest &request, NavigationType type);


};
#endif // myQWebPage_H
