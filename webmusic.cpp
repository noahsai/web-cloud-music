#include "webmusic.h"
#include "ui_webmusic.h"

webmusic::webmusic(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::webmusic)
{
    ui->setupUi(this);
    datapath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) +"/web-cloud-music/webdata";
    QDir().mkpath(datapath);
    ui->horizontalLayout->addWidget(&webview);
   // justhide = false;
    playing = false;
    cachemanager = NULL;
    //=========歌词=========
    lrcshow = new lrcdesktop();
    lrcshow->show();
    //==========托盘========必须在歌词之后，trayinit里要用到歌词
    QIcon icon = QIcon(":/icon.svg");
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(icon);
    connect(trayIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(trayiconactive(QSystemTrayIcon::ActivationReason)));
    trayinit();
    trayIcon->show();
    //=========缓存===========
    QDir().mkpath("/tmp/web-cloud-music");
    diskCache = new mycache(this);
    diskCache->setCacheDirectory("/tmp/web-cloud-music");
    diskCache->setMaximumCacheSize(1024*1024*1024);//1G
    connect(diskCache , SIGNAL(mp3url(QString&)),this , SLOT(gotmp3url(QString&)));
    webview.page()->networkAccessManager()->setCache(diskCache);
    //ebview.page()->networkAccessManager()->setCookieJar(jar);


    //========本地数据===========
    webview.settings()->enablePersistentStorage(datapath);
    webview.settings()->setAttribute(QWebSettings::LocalStorageEnabled,true);
    webview.settings()->setLocalStoragePath(datapath);
//    webview.settings()->setAttribute(QWebSettings::OfflineStorageDatabaseEnabled,true);
//    webview.settings()->setAttribute(QWebSettings::OfflineWebApplicationCacheEnabled,true);
//    webview.settings()->setOfflineStoragePath(datapath);
//    webview.settings()->setOfflineWebApplicationCachePath(datapath);
    webview.settings()->setAttribute(QWebSettings::PluginsEnabled,true);//flash
    webview.settings()->setAttribute(QWebSettings::QWebSettings::JavascriptCanOpenWindows	,true);//
    webview.settings()->setAttribute(QWebSettings::QWebSettings::JavascriptCanCloseWindows	,true);//
  // webview.page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);//不需要了

    //==========定时器===========
    timer.setParent(this);
    timer.setInterval(200);
    timer.setSingleShot(false);
    connect(&timer , SIGNAL(timeout()),this,SLOT(timeout()));
    //lrc.setText("[ 歌 词 ]");
   // lrc.resize(300,30);
   // lrc.show();

    jar = new mycookiejar();//必须放在readcookie前
    webview.page()->networkAccessManager()->setCookieJar(jar);
    //connect(webview.page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()),   this, SLOT(setslottoweb()));//qt推荐的，不过这样用貌似也没事
    connect( &webview ,SIGNAL(loadFinished(bool)),this,SLOT(setslottoweb()));
    connect( &webview ,SIGNAL(loadFinished(bool)),this,SLOT(savecookie()));
    connect(&webview , SIGNAL(loadProgress(int)),ui->loading,SLOT(setValue(int)));
    connect(&webview , SIGNAL(linkClicked(QUrl)),this , SLOT(linkclicked(QUrl)));

    setWindowTitle("网页云音乐");
    setWindowIcon(QIcon(":/icon.svg"));
    readcfg();
    readcookie();
    ui->loading->setMaximumWidth(this->width()*15/16);//需要放在readcfg之后
    webview.hide();
    webview.load(QUrl("http://music.163.com"));
}

webmusic::~webmusic()
{
    savecookie();
    savecfg();
    delete ui;
}

void webmusic::setslottoweb()
{
   // qDebug()<<"setslottoweb ed";
    webview.page()->mainFrame()->addToJavaScriptWindowObject("webcloudmusic",this);
    webview.page()->mainFrame()->evaluateJavaScript(" \
document.getElementsByClassName('ply')[0].onclick = Function(\"a = document.getElementsByClassName('pas')[0];b=(a!=null);webcloudmusic.timerstart(!b);\");\
a = document.getElementsByClassName('m-nav')[0] ; a.children[4].style.display='none'; a.children[5].style.display='none';\
b = a.children[3];\
c = b.getElementsByTagName('em')[0]; c.innerText='查看缓存';\
b.onclick = Function(\"webcloudmusic.opencache();\");\
d = b.getElementsByTagName('a')[0]; d.href=\"javascript:;\";d.target=\"\"; \
                                                    ");
    //这里不写点击网页里的关闭歌词界面触发关闭桌面歌词，以免关闭界面后又要手动打开桌面歌词
    ui->loading->hide();
    disconnect(&webview , SIGNAL(loadProgress(int)),ui->loading,SLOT(setValue(int)));
    webview.show();
}

void webmusic::timerstart(bool b){//用处不大，也就停止播放时打开歌词（timer时停止的），再播放时需要。
    //qDebug()<<"timerstart";
    playing = b;
    if( lrcshow->isVisible()){
        if(b) timer.start();
        else timer.stop();
    }
}

void webmusic::timeout()
{
    QVariant ret = webview.page()->mainFrame()->evaluateJavaScript("\
                                                                    a = document.getElementsByClassName('j-flag z-sel')[0].innerText ;\
                                                                     b = document.getElementsByClassName('j-flag z-sel')[0].nextSibling ;\
                                                                    if( b == null ) a +'\\t';\
                                                                    else  a+'\\t'+b.innerText;\
                                                                   ");    //strparam是运行js函数可以带的参数
   // qDebug()<<ret.toString();
    QString text = ret.toString();
    if(gotlrc != text) {
        gotlrc = text;
        lrcshow->setlrc(text);
    }
}
void webmusic::savecookie()
{
  //  jar =  webview.page()->networkAccessManager()->cookieJar();
   QList  <QNetworkCookie  >   list =jar->getallCookies();
 //  qDebug()<<list.length()<<list;
   QFile file(datapath+"/cookie.save");
   file.open(QIODevice::WriteOnly);
   QTextStream  data(&file);
   foreach (QNetworkCookie one, list) {
       data<<one.toRawForm()<<endl;
   }
   //data<<list;
   file.close();
}

void webmusic::readcookie()
{
   QFile file(datapath+"/cookie.save");
   //qDebug()<<file.fileName()<<endl;

  if( file.open(QIODevice::ReadOnly)){
        QTextStream  data(&file);
        QString t;
        QStringList list;
        while(!data.atEnd())
        {
            t = data.readLine();
            list = t.split("; ");
            QNetworkCookie cookie;
            for(int i=0;i<list.length();i++){
                QString name = list.at(i);
                 QString value;
                QStringList l = name.split("=");
                for(int j =0;j<l.length();j++){
                    if(j==0) name = l.at(0);
                    if(j==1) value  = l.at(1);
                }
                if(name=="domain") cookie.setDomain(value);
                else    if(name=="expires") cookie.setExpirationDate(QDateTime().fromString(value));
                else      if(name=="path") cookie.setPath(value);
                else  if(name == "HttpOnly") cookie.setHttpOnly( true);
               else {
                    cookie.setName( name.toLatin1());
                     cookie.setValue( value.toLatin1());
                //   qDebug()<<"zidingyi"<<name<<value<<cookie;
                }
            }
           // qDebug()<<cookie<<endl;
            jar->insertCookie(cookie);
          //  qDebug()<<"readcookie:"<<list;
        }
        file.close();
    }
}



void webmusic::trayiconactive(QSystemTrayIcon::ActivationReason reason)
{
    if(reason == QSystemTrayIcon::Context ||reason == QSystemTrayIcon::Unknown ) return;
    else {
        if(isActiveWindow()) this->hide();
        else {
        showNormal();
        activateWindow();
        }
    }
}


void webmusic:: playstop(){
    webview.page()->mainFrame()->evaluateJavaScript("document.getElementsByClassName('ply j-flag')[0].click()");//将会触发timerstart();
}

void webmusic::pre(){
    webview.page()->mainFrame()->evaluateJavaScript("document.getElementsByClassName('prv')[0].click()");
}

void webmusic::next(){
    webview.page()->mainFrame()->evaluateJavaScript("document.getElementsByClassName('nxt')[0].click()");
}

void webmusic:: clickweblrc(bool b){
    // document.getElementsByClassName('icn icn-list')[0].onclick = Function(\"a = document.getElementById('g_playlist');b=(a!=null);webcloudmusic.toshowlrc(!b);\");
    QVariant ret = webview.page()->mainFrame()->evaluateJavaScript("document.getElementsByClassName('lytit')[0].innerText");    //
    //qDebug()<<ret.toString();
    bool now =  !(ret.toString().isEmpty()) ;//现在网页歌词的状态
    if( b^ now)    webview.page()->mainFrame()->evaluateJavaScript("document.getElementsByClassName('icn icn-list')[0].click()");//使用异或，不同时执行，相同时忽略
}

void webmusic::trayinit()
{
    QAction *playstoop=new QAction(tr("开始/暂停"),this);
    connect(playstoop,SIGNAL(triggered()),this,SLOT(playstop()));

    QAction *presong=new QAction(tr("上一首"),this);
    connect(presong,SIGNAL(triggered()),this,SLOT(pre()));

    QAction *nextsong=new QAction(tr("下一首"),this);
    connect(nextsong,SIGNAL(triggered()),this,SLOT(next()));

    showlrc=new QAction(tr("桌面歌词"),this);
    showlrc ->setCheckable(true);
     connect(showlrc , SIGNAL(toggled(bool)) , this , SLOT(toshowlrc(bool)));
    connect(lrcshow,SIGNAL(closed(bool)),showlrc,SLOT(setChecked(bool)));

    tolock=new QAction(tr("锁定歌词"),this);
    tolock->setCheckable(true);
     connect(tolock , SIGNAL(toggled(bool)) , lrcshow , SLOT(tolock(bool)));
    connect(showlrc , SIGNAL(toggled(bool)),tolock , SLOT(setEnabled(bool)));

    QAction* quitAction = new QAction(tr("退出"), this);
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));//若触发了退出就退出程序

    trayIconMenu = new QMenu(this);//菜单
    trayIconMenu->addAction(playstoop);//
    trayIconMenu->addAction(presong);
    trayIconMenu->addAction(nextsong);
    trayIconMenu->addAction(showlrc);
    trayIconMenu->addAction(tolock);
    trayIconMenu->addAction(quitAction);//把退出加到入菜单项
    trayIcon->setContextMenu(trayIconMenu);//设置托盘上下文菜单为trayIconMenu

}

void webmusic::toshowlrc(bool b)//隐藏歌词时不要影响网页的歌词了
{
    qDebug()<<"toshowlrc";
    if( b )  {
        lrcshow->setVisible(true);
        showlrc ->setChecked(true);
        clickweblrc(true);
        timer.start();
    }
    else {
        showlrc ->setChecked(false);
        lrcshow->setVisible(false);
        timer.stop();
    }
   if(ui->loading->isHidden())  savecfg();
}

void webmusic:: closeEvent(QCloseEvent* event){
    QVariant ret = webview.page()->mainFrame()->evaluateJavaScript("document.getElementsByClassName('pas')[0].innerText");    //
    QString text = ret.toString();
    if(!text.isEmpty() && showlrc->isChecked()) {
       //  qDebug()<<"toshowlrc"<<ret.toString();
         toshowlrc(true);//里面会
        timer.start();
    }
    else timer.stop();
    savecfg();
    event->ignore();
    hide();//保存后在隐藏
}


void webmusic::savecfg()
{
    QSettings settings("web-cloud-music", "webmusic");
    settings.setValue("showlrc",showlrc->isChecked());
    settings.setValue("tolock",tolock->isChecked());
    if(  (!isFullScreen())  && (!isHidden()) &&(!isMaximized())) settings.setValue("geometry", this->geometry());
    settings.setValue("savecachepath", savecachepath);
    //qDebug()<<"save:"<<this->geometry();
}

void webmusic::readcfg()
{
    QSettings settings("web-cloud-music", "webmusic");
   // qDebug()<<settings.fileName();
    bool lrc =  settings.value("showlrc",true).toBool();
    bool lock =  settings.value("tolock",false).toBool();
    setGeometry(settings.value("geometry",QRect((QApplication::desktop()->width()-this->width())/2 ,
                                                QApplication::desktop()->height()-100,
                               1100 ,600)
                               ).toRect());
    savecachepath = settings.value("savecachepath",QString("")).toString();
//先设置置顶，再设置显示状态
    tolock->setChecked(lock);
    lrcshow->tolock(lock);
    showlrc ->setChecked(lrc);//这样设置时会触发信号的所以无需toshowlrc了
    tolock->setEnabled(lrc);
    toshowlrc(lrc);
    //qDebug()<<"read:"<<this->geometry();

}

void webmusic:: gotmp3url(QString &url)
{
    QVariant ret = webview.page()->mainFrame()->evaluateJavaScript("document.getElementsByClassName('f-thide name fc1 f-fl')[0].innerText+'\\t'+document.getElementsByClassName('by f-thide f-fl')[0].innerText");    //strparam是运行js函数可以带的参数
   // qDebug()<<"toshowlrc"<<ret.toString();
    QString name = ret.toString();
    trayIcon->setToolTip(name.replace('\t','-'));
    mp3list[name]=url;
    qDebug()<<"found a music cache:"<<name;//<<url;
}

void webmusic::opencache(){
    if(  !cachemanager ) {
        cachemanager = new cache(this);
        cachemanager->setWindowFlags(Qt::Window);
        connect( cachemanager , SIGNAL(pathset(QString&)) , this ,SLOT(gotsavecachepath(QString&)));
    }
    if(! cachemanager->isworking()){
        cachemanager->setpath(savecachepath);
        cachemanager->setcache((QNetworkDiskCache*)(diskCache));
        cachemanager->setlist(mp3list);
    }
    cachemanager->show();
}

void webmusic::gotsavecachepath(QString & p)//下次默认打开该地址
{
    savecachepath = p;
    savecfg();
}

//已经继承qwebview和qwebpage重写函数了，无需在使用改函数了
//void webmusic:: linkclicked(QUrl u){
//    //qDebug()<<u;
//    if(u.toString().indexOf("music.163.com/api/")!=-1) {
//        QWebView *view =  webview.newwindow();
//        view->setAttribute(Qt::WA_DeleteOnClose);
//        connect(view->page() , SIGNAL(windowCloseRequested()) , view ,SLOT(close()));
//        connect(view , SIGNAL(destroyed(QObject*)) , this ,SLOT(refresh()));
//        view->load(u);
//        view->show();
//    }
//    else webview.load(u);
//}

void webmusic:: refresh(){
webview.load(QUrl("http://music.163.com/#"));
}



//===================================[mycache]=======================================================

mycache:: mycache(QObject *parent):
    QNetworkDiskCache(parent)
{
}

mycache::~mycache()
{
}

  QIODevice *	mycache::prepare(const QNetworkCacheMetaData &metaData){
    QString url = metaData.url().toString();
    if(url.indexOf(".mp3") != -1 )
    {
       // qDebug()<<"found a mp3:"<<url;
        emit mp3url( url );
    }
    return QNetworkDiskCache :: prepare(metaData) ;
}


