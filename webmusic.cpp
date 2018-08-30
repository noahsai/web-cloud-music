#include "webmusic.h"
#include "ui_webmusic.h"


//setchecked()会自动出发连接的槽函数！！

webmusic::webmusic(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::webmusic)
{
    ui->setupUi(this);
    isresize = false;
    webview = new myQWebview(this);

    datapath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) +"/web-cloud-music/webdata";
    QDir().mkpath(datapath);
    ui->horizontalLayout->addWidget(webview);
   // justhide = false;
    playing = false;
    //======播放记录========
    cachemanager = new cache(this);
    cachemanager->setWindowFlags(Qt::Window);
    connect( cachemanager , SIGNAL(pathset(QString&)) , this ,SLOT(gotsavecachepath(QString&)));
    connect( cachemanager , SIGNAL(cleanlist()) , this ,SLOT(cleanlist()));
    //=========歌词=========
    lrcshow = new lrcdesktop();//这里千万不能show！！！会导致增大尺寸时无法拖动！！！不知为何！！！
    //==========托盘========必须在歌词之后，trayinit里要用到歌词
    QIcon icon = QIcon(":/icon.svg");
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(icon);
    connect(trayIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(trayiconactive(QSystemTrayIcon::ActivationReason)));
    trayinit();
    trayIcon->show();
    //=========缓存===========
    QDir().mkpath("/tmp/web-cloud-music");
    webview->page()->profile()->setCachePath("/tmp/web-cloud-music");

    //========本地数据===========
    webview->settings()->setAttribute(QWebEngineSettings::PluginsEnabled,true);
    webview->settings()->setAttribute(QWebEngineSettings::JavascriptCanAccessClipboard,true);
    webview->page()->profile()->setPersistentStoragePath(datapath);

    //==========定时器===========
    timer.setParent(this);
    timer.setInterval(200);
    timer.setSingleShot(false);
    connect(&timer , SIGNAL(timeout()),this,SLOT(timeout()));
    //======找到mp3地址====
    connect(webview , SIGNAL(foundmp3(QString)) , this ,SLOT(gotmp3url(QString)));
    setWindowTitle("网页云音乐");
    setWindowIcon(QIcon(":/icon.svg"));
    connect( webview ,SIGNAL(loadStarted()),this,SLOT(setslottoweb()));//加载高音质或其他脚本
    readcfg();//加载脚本要刷新网页，所以放最后吧
    webview->load(QUrl("https://music.163.com"));
}

webmusic::~webmusic()
{

    savecfg();
    delete ui;
}

void webmusic::setslottoweb()//connect在readcfg之后执行enablejs(bool b)
{
    QString name  = QApplication::applicationDirPath()+"/gao.js";
    QFile file(":/gao.js");
    if(file.open(QIODevice::ReadOnly)){
        QString js;
        js = file.readAll();
        file.close();
        webview->page()->runJavaScript(js);
       qDebug()<<name;
    }
    else qDebug()<<"js open error";
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
    QString cmd = "\
                                                                    a = document.getElementsByClassName('j-flag z-sel')[0].innerText ;\
                                                                     b = document.getElementsByClassName('j-flag z-sel')[0].nextSibling ;\
                                                                    if( b == null ) a +'\\t';\
                                                                    else  a+'\\t'+b.innerText;\
                                                                   ";    //strparam是运行js函数可以带的参数
   // qDebug()<<ret.toString();
   webview->page()->runJavaScript(cmd, [=](const QVariant &v) {
       //qDebug() << v.toString();
       QString text = v.toString();
       if(gotlrc != text) {
           gotlrc = text;
           lrcshow->setlrc(text);
       }
   });

}



void webmusic::trayiconactive(QSystemTrayIcon::ActivationReason reason)
{
    if(reason == QSystemTrayIcon::Context ||reason == QSystemTrayIcon::Unknown ) return;
    else {
        if(isActiveWindow()) {
            if(showlrc->isChecked()) this->close();
            else this->hide();
        }
        else {
        showNormal();
        activateWindow();
        //无奈做法，用于解决界面卡死问题
        if(!isresize)   resize(size()+QSize(1,1));
        else   resize(size()-QSize(1,1));
        isresize = !isresize;
        }
    }
}


void webmusic:: playstop(){
    webview->page()->runJavaScript("document.getElementsByClassName('ply j-flag')[0].click()");//将会触发timerstart();
}

void webmusic::pre(){
    webview->page()->runJavaScript("document.getElementsByClassName('prv')[0].click()");
}

void webmusic::next(){
    webview->page()->runJavaScript("document.getElementsByClassName('nxt')[0].click()");
}

void webmusic:: clickweblrc(bool b){
    // document.getElementsByClassName('icn icn-list')[0].onclick = Function(\"a = document.getElementById('g_playlist');b=(a!=null);webcloudmusic.toshowlrc(!b);\");
    QString cmd ="document.getElementsByClassName('lytit')[0].innerText";    //
    //qDebug()<<ret.toString();
    webview->page()->runJavaScript(cmd, [=](const QVariant &v) {
    bool now =  !(v.toString().isEmpty()) ;//现在网页歌词的状态
    if( b^ now)    webview->page()->runJavaScript("document.getElementsByClassName('icn icn-list')[0].click()");//使用异或，不同时执行，相同时忽略
    });
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

    QAction *cachemg=new QAction(tr("播放记录"),this);
    connect(cachemg , SIGNAL(triggered()) , this , SLOT(opencache()));

    gaojs=new QAction(tr("高奸商"),this);
    gaojs->setToolTip("可到程序目录下自行修理奸商^_^");
    gaojs ->setCheckable(true);
    connect(gaojs , SIGNAL(toggled(bool)) , this , SLOT(enablejs(bool)));

    QAction* quitAction = new QAction(tr("退出"), this);
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));//若触发了退出就退出程序

    trayIconMenu = new QMenu(this);//菜单
    trayIconMenu->addAction(playstoop);//
    trayIconMenu->addAction(presong);
    trayIconMenu->addAction(nextsong);
    trayIconMenu->addAction(showlrc);
    trayIconMenu->addAction(tolock);
    trayIconMenu->addAction(cachemg);
    trayIconMenu->addAction(gaojs);
    trayIconMenu->addAction(quitAction);//把退出加到入菜单项
    trayIcon->setContextMenu(trayIconMenu);//设置托盘上下文菜单为trayIconMenu

}


void webmusic::toshowlrc(bool b)//隐藏歌词时不要影响网页的歌词了
{
    qDebug()<<"toshowlrc";
    if( b )  {
        lrcshow->show();
        //别调用showlrc ->setChecked(true);//setchecked()会自动出发连接的槽函数！！
        clickweblrc(true);
        timer.start();
    }
    else {
        //别调用showlrc ->setChecked(false);//setchecked()会自动出发连接的槽函数！！
        lrcshow->hide();
        timer.stop();
    }
   savecfg();

}

void webmusic:: closeEvent(QCloseEvent* event){
    QString cmd ="document.getElementsByClassName('pas')[0].innerText";    //
    webview->page()->runJavaScript(cmd, [=](const QVariant &v) {
    QString text = v.toString();
    if(!text.isEmpty() && showlrc->isChecked()) {
       //  qDebug()<<"toshowlrc"<<ret.toString();
         showlrc ->setChecked(true);//里面会
        timer.start();
    }
    else timer.stop();
    savecfg();
    hide();//保存后在隐藏
    });
    event->ignore();
}


void webmusic::savecfg()
{
    QSettings settings("web-cloud-music", "webmusic");
    settings.setValue("showlrc",showlrc->isChecked());
    settings.setValue("tolock",tolock->isChecked());
    settings.setValue("enablejs",gaojs->isChecked());
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
    bool js =  settings.value("enablejs",true).toBool();
    setGeometry(settings.value("geometry",QRect((QApplication::desktop()->width()-this->width())/2 ,
                                                QApplication::desktop()->height()-100,
                               1100 ,600)
                               ).toRect());
    savecachepath = settings.value("savecachepath",QString("")).toString();
//先设置置顶，再设置显示状态
    tolock->setChecked(lock);//setchecked()会自动出发连接的槽函数！！
    //lrcshow->tolock(lock);
    showlrc ->setChecked(lrc);//setchecked()会自动出发连接的槽函数！！
    tolock->setEnabled(lrc);
    //gaojs->setChecked(js);//setchecked()会自动出发连接的槽函数！！

}

void webmusic:: gotmp3url(QString url)
{
   QString cmd ="document.getElementsByClassName('f-thide name fc1 f-fl')[0].innerText+'\\t'+document.getElementsByClassName('by f-thide f-fl')[0].innerText";    //strparam是运行js函数可以带的参数
   // qDebug()<<"toshowlrc"<<ret.toString();
    webview->page()->runJavaScript(cmd, [=](const QVariant &v) {
        QString name;
     name = v.toString();
     //qDebug()<<"name"<<name;
    trayIcon->setToolTip(name.replace('\t','-'));
    mp3list[name]=url;
    cachemanager->setlist(mp3list);
     });
     qDebug()<<"found a mp3:"<<url;
}


void webmusic::opencache(){
    qDebug()<<"webmusic::opencache()";

    if(! cachemanager->isworking()){
        cachemanager->setpath(savecachepath);//防止下载途中修改了路径
    }
    cachemanager->show();
    cachemanager->activateWindow();
}

void webmusic::gotsavecachepath(QString & p)//下次默认打开该地址
{
    savecachepath = p;
    savecfg();
}


void webmusic:: refresh(){
webview->load(QUrl("http://music.163.com/#"));
}


void webmusic:: cleanlist(){
    mp3list.clear();
    cachemanager->setlist(mp3list);

}

void webmusic::enablejs(bool b){
    qDebug()<<"!!!!!!!!!!!!!!!!!!enablejs";
//    if(b) connect( webview ,SIGNAL(loadStarted()),this,SLOT(setslottoweb()));//加载高音质或其他脚本
//    else disconnect( webview ,SIGNAL(loadStarted()),this,SLOT(setslottoweb()));//不加载脚本
    webview->page()->deleteLater();
    myQWebPage *page = new myQWebPage(this);
    myQWebEngineUrlRequestInterceptor *webInterceptor = new myQWebEngineUrlRequestInterceptor();
    page->profile()->setRequestInterceptor(webInterceptor);
    connect(webInterceptor , SIGNAL(foundmp3(QString)) , webview , SIGNAL(foundmp3(QString)));
    webview->setPage(page);
    connect( webview ,SIGNAL(loadStarted()),this,SLOT(setslottoweb()));//加载高音质或其他脚本
    webview->load(QUrl("https://music.163.com"));
}



