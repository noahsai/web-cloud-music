#ifndef WEBMUSIC_H
#define WEBMUSIC_H

#include <QMainWindow>
#include<QTimer>
#include<QLabel>
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
#include<mywebview.h>
#include<QWebChannel>
#include <QWebEnginePage>
#include<myqwebpage.h>
#include<QWebEngineScriptCollection>
#include <QWebEngineScript>

namespace Ui {
class webmusic;
}

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
    void playstop();
    void pre();
    void next();
    void clickweblrc(bool);
    void trayiconactive(QSystemTrayIcon::ActivationReason);
    void trayinit();
    void setslottoweb();
    void gotmp3url(QString);
    void gotsavecachepath(QString&);

  //  void linkclicked(QUrl);//不需要了
    void refresh();
    void cleanlist();
    void enablejs(bool);
    void reload();

private:
    Ui::webmusic *ui;
    void closeEvent(QCloseEvent *event);
    void savecfg();
    void readcfg();
    QTimer timer;
    myQWebview  *webview;
    QString datapath;
  // QLabel lrc;

    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
    QAction*  showlrc;
    QAction*  tolock;
    QAction*  gaojs;
    lrcdesktop * lrcshow;
    //bool  justhide;
    bool playing;
    QString  gotlrc;
    QMap<QString , QString>  mp3list;
    cache *cachemanager;
     QString savecachepath;
     QWebChannel *channel;
     bool isresize; //无奈添加该变量，用于解决界面卡死问题
};



#endif // WEBMUSIC_H
