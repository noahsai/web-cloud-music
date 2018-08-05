#ifndef myQWebPage_H
#define myQWebPage_H

#include <QObject>
#include <QWebEnginePage>
#include<QNetworkRequest>
class myQWebPage : public QWebEnginePage
{
    Q_OBJECT

public:
    myQWebPage (QObject * parent = 0);
    ~myQWebPage();

signals:
    void openurl(QUrl);
    void loadurl(QUrl);
    //void opencache();//不知为何无效

private:
 bool acceptNavigationRequest(const QUrl &url, NavigationType type, bool isMainFrame);

};
#endif // myQWebPage_H
