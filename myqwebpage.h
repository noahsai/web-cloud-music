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
    void toreload();

private:

protected:
 bool acceptNavigationRequest(const QUrl &url, NavigationType type, bool isMainFrame);
};
#endif // myQWebPage_H
