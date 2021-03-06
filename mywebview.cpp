#include "mywebview.h"

myQWebview ::myQWebview(QWidget * parent ):
    QWebEngineView(parent)
{
    settings()->setAttribute(QWebEngineSettings::JavascriptCanAccessClipboard,true);
    settings()->setAttribute(QWebEngineSettings::PluginsEnabled,true);//flash
    myQWebPage *page = new myQWebPage(this);
    //因为reload要换page的原因，下面改为由外面设置了
    myQWebEngineUrlRequestInterceptor *webInterceptor = new myQWebEngineUrlRequestInterceptor();
    page->profile()->setRequestInterceptor(webInterceptor);
    connect(webInterceptor , SIGNAL(foundmp3(QString)) , this , SIGNAL(foundmp3(QString)));
    setPage(page);
    connect(page , SIGNAL(toreload()),this,SIGNAL(toreload()));
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this ,SIGNAL(customContextMenuRequested(QPoint)),this , SLOT(mkmune(QPoint)));
}

myQWebview::~myQWebview(){
}

QWebEngineView *  myQWebview::	createWindow(QWebEnginePage::WebWindowType type){
    myQWebview *view = new myQWebview();
    view->setAttribute(Qt::WA_DeleteOnClose);
    connect(view->page(),SIGNAL(windowCloseRequested()),view,SLOT(deleteLater()));
    view->show();
    return view;
}

QWebEngineView* myQWebview::newwindow(){
    return createWindow(QWebEnginePage::WebBrowserWindow);
}


void myQWebview::mkmune(QPoint p){
    QMenu* menu = new QMenu(this);
    QAction *a = page()->action(QWebEnginePage::Copy);
    a->setText("复制");
    menu->addAction(a);
    if(page()->contextMenuData().isContentEditable()){
        QAction *b = page()->action(QWebEnginePage::Paste);
        b->setText("粘贴");
        menu->addAction(b);
    }
    else {
        QAction *d = page()->action(QWebEnginePage::Back);
        d->setText("返回");
        QAction *e = page()->action(QWebEnginePage::Forward);
        e->setText("前进");
        QAction *f = page()->action(QWebEnginePage::Reload);
        f->setText("刷新");

        menu->addAction(d);
        menu->addAction(e);
        menu->addAction(f);
    }

    menu->popup(QCursor::pos());
}


//=======================
myQWebEngineUrlRequestInterceptor::myQWebEngineUrlRequestInterceptor(QObject *parent)
    : QWebEngineUrlRequestInterceptor(parent)
{
}

void myQWebEngineUrlRequestInterceptor::interceptRequest(QWebEngineUrlRequestInfo &info)
{
    QString url = info.requestUrl().toString();
    //qDebug() << info.resourceType()<<url;
    if(url.indexOf(".mp3")!=-1 || url.indexOf(".m4a")!=-1){
       // qDebug() << "mp3:"<<url;
        emit foundmp3(url);
    }
}


