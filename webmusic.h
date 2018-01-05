#ifndef WEBMUSIC_H
#define WEBMUSIC_H

#include <QMainWindow>
#include <QWebFrame>
#include <QWebView>
#include<QTimer>
#include<QLabel>
#include<QNetworkCookie>
#include<QNetworkCookieJar>
#include<QFile>
#include<QTextStream>
#include<QStringList>
#include<QNetworkDiskCache>
#include<QSystemTrayIcon>
#include<lrcdesktop.h>
#include<QMenu>
#include<QSettings>
#include<QDir>
#include<cache.h>
#include<mywebview.h>>
#include<mycookiejar.h>
namespace Ui {
class webmusic;
}
class mycache;

class webmusic : public QMainWindow
{
    Q_OBJECT

public:
    explicit webmusic(QWidget *parent = 0);
    ~webmusic();
public slots:
    void toshowlrc(bool);
    void timerstart(bool);
    void opencache();

private slots:
    void timeout();
    void savecookie();
    void readcookie();
    void playstop();
    void pre();
    void next();
    void clickweblrc(bool);
    void trayiconactive(QSystemTrayIcon::ActivationReason);
    void trayinit();
    void setslottoweb();
    void gotmp3url(QString&);
    void gotsavecachepath(QString&);
  //  void linkclicked(QUrl);//不需要了
    void refresh();

private:
    Ui::webmusic *ui;
    void closeEvent(QCloseEvent *event);
    void savecfg();
    void readcfg();
    QTimer timer;
    myQWebview  webview;
    mycookiejar *jar;
    QString datapath;
  // QLabel lrc;

    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
    QAction*  showlrc;
     QAction*  tolock;
    lrcdesktop * lrcshow;
    //bool  justhide;
    bool playing;
    QString  gotlrc;
    QMap<QString , QString>  mp3list;
    cache *cachemanager;
     mycache * diskCache;
     QString savecachepath;
};



class mycache :public QNetworkDiskCache
{
    Q_OBJECT

public :
    mycache (QObject *parent = 0);
    ~mycache();
    virtual QIODevice *	prepare(const QNetworkCacheMetaData & metaData);

signals:
    void mp3url( QString& );
};

#endif // WEBMUSIC_H
