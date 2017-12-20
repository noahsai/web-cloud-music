#include "lrcdesktop.h"
#include "ui_lrcdesktop.h"

//不知为何，只有放cpp里才能通过编译
#include <X11/extensions/shape.h>
#include <QtX11Extras/QX11Info>

lrcdesktop::lrcdesktop(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::lrcdesktop)
{
    ui->setupUi(this);
    this->setAutoFillBackground(false);
    setWindowFlags(Qt::FramelessWindowHint|Qt::SplashScreen|Qt:: WindowStaysOnTopHint);
    this->setAttribute(Qt::WA_TranslucentBackground, true);
    lrctext = "网页云音乐";
    nowtext =0;//默认只有一行，所以0
    locked = false;
    entered = false;
    setting = false;
    nowcolor = "#ffffff";
    nextcolor = "#000000";
    bgcolor = "#aaaaaa";
    alpha = 100;
    ui->setbar->move(0,0);
    ui->setbar->hide();
    readcfg();
}

lrcdesktop::~lrcdesktop()
{
    savecfg();
    delete ui;
}


void lrcdesktop::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    QPen nowpen(QColor(nowcolor),1);
    QPen nextpen(QColor(nextcolor),1);
    p.setRenderHint(QPainter::Antialiasing,true);
        QColor bg(bgcolor);
        bg.setAlpha(alpha);
       p.setBrush(QBrush(bg));
        p.setPen(Qt::NoPen);
        p.drawRect(this->rect());
    QFont fon;
    fon.setPixelSize(this->height()/3);
    p.setFont(fon);
    QTextOption opt(Qt::AlignCenter);
    opt.setWrapMode(QTextOption::NoWrap	);
//===================[绘制边框效果，但需要自己处理文字位置，暂时不用]=======================
    //QPainterPath path;
   //path.addText(0,30,fon,lrc);
//    p.strokePath(path,QPen(QColor("#ffffff"),4));
   // p.drawPath(path);
   // p.fillPath(path, QBrush("#000000"));
//===================[简单的歌词右下阴影，之前未分歌词颜色时用的，暂时不用了]=======================
//    p.save();
//    p.translate(1,1);
//    p.setPen(QPen(QColor("#aaaaaa"),1));
//    p.drawText(this->rect() ,lrctext , opt);
//    p.restore();
    //==================[歌词]===================
    if(nowtext != 0){
        QStringList list = lrctext.split('\n');
        if( entered && (!ui->setbar->isVisible()) ) {
           nowpen.setColor(QColor(nowcolor).lighter(120));
           nextpen.setColor(QColor(nextcolor).lighter(120));
        }
       else {
            nowpen.setColor(QColor(nowcolor));
            nextpen.setColor(QColor(nextcolor));
        }
       //p.drawText(this->rect(),"lrca地方四大发送方打算地\n四大发送地方外人田",QTextOption(Qt::AlignCenter));
        if( nowtext ==1)  p.setPen(nowpen);
        else p.setPen(nextpen);
        p.drawText(this->rect().x(),this->rect().y(),this->rect().width(),this->rect().height()/2 ,Qt::AlignCenter,list.at(0) );
        if( nowtext ==2 )  p.setPen(nowpen);
        else  p.setPen(nextpen);
        p.drawText(this->rect().x(),this->rect().y()+this->rect().height()/2,this->rect().width(),this->rect().height()/2 ,Qt::AlignCenter,list.at(1) );
    }
    else{
        p.setPen(nowpen);
        p.drawText(this->rect(),Qt::AlignCenter,lrctext );
    }
}

void lrcdesktop::mouseDoubleClickEvent(QMouseEvent * event){
    if(event->button()==Qt::LeftButton )
    {
         //   qDebug()<<windowFlags();
        if(windowFlags()==(Qt::Window|Qt::WindowTitleHint|Qt::WindowSystemMenuHint|Qt::WindowCloseButtonHint|Qt:: WindowStaysOnTopHint))
        {
            setting  = false;
            if(  tmptosetting.indexOf('\t') < 1)    {
                lrctext = "网页云音乐";
                nowtext = 0;
            }
            else {
                nowtext = 1;
                lrctext = tmptosetting;
            }
            ui->setbar->hide();
            setWindowFlags(Qt::FramelessWindowHint|Qt::SplashScreen|Qt:: WindowStaysOnTopHint);
            this->show();
        }
        else {
            setting  = true;
            nowtext = 1;
            lrctext = "网易版云音乐\n云音乐网页版";
            ui->setbar->show();
            this->setWindowTitle("歌词设置");
            setWindowFlags((Qt::Window|Qt::WindowTitleHint|Qt::WindowSystemMenuHint|Qt::WindowCloseButtonHint|Qt:: WindowStaysOnTopHint));
            this->show();
        }
        event->accept();
    }
    else QWidget::mouseReleaseEvent(event);
}


void  lrcdesktop::setlrc(QString  t){

    if( setting ){
        tmptosetting = t;
        return;//设置时不接收歌词
    }
    if(  t.indexOf('\t') < 1)    {
        lrctext = "网页云音乐";
        nowtext = 0;
    }
    else  {
        t = t.replace("\n","  ");
        t = t.replace('\t','\n');

        QStringList list = t.split('\n');
        if( lrctext.indexOf('\n') == -1 ) { //如果之前是“网页云音乐”，就直接赋值
            lrctext = t;
            nowtext = 1;
        }
        else {
            QStringList old = lrctext.split('\n');
            list = t.split('\n');
            if(list.at(0) ==old.at(1) ){ //使下句歌词位置不变
              old.replace(0,list.at(1));
               nowtext =2;
                lrctext = old.join('\n');
            }
            else {
                nowtext = 1;
                lrctext = t;
            }
        }
    }
    update();
}

void lrcdesktop::enterEvent(QEvent *event){
    entered = true;
    update();
    event->accept();
}
void lrcdesktop::leaveEvent(QEvent *event){
    entered = false ;
    update();
    event->accept();
}

void lrcdesktop::tolock(bool tolock)
{
    if(!isVisible()) return;
    if( ! tolock)//取消置顶
    {
        XRectangle* myrect = new XRectangle;
        myrect->x = 0;
        myrect->y = 0;
        myrect->width = width();
        myrect->height = height();
        XShapeCombineRectangles(QX11Info::display(), winId(), ShapeInput,0,0, myrect, 1, ShapeSet, YXBanded);
        qDebug()<<"unlocked";
        show();
    }
    else//置顶穿透
    {
        ui->setbar->hide();
        setWindowFlags(Qt::FramelessWindowHint|Qt::Tool|Qt:: WindowStaysOnTopHint);
        XShapeCombineRectangles(QX11Info::display(), winId(), ShapeInput,0,0, NULL, 0, ShapeSet, YXBanded);
        qDebug()<<"locked";
        show();
    }
}

void lrcdesktop::closeEvent(QCloseEvent *event){
    emit closed(true);
    hide();
    event->accept();
}

void lrcdesktop::mousePressEvent(QMouseEvent* event)
{
    if(event->button()==Qt::LeftButton )
    {
        oldpos=event->globalPos()-this->pos();
        setCursor(Qt::ClosedHandCursor);
    }
}

void lrcdesktop::mouseMoveEvent(QMouseEvent * event){

        move(event->globalPos()-oldpos);//貌似linux要这样
        event->accept();

}

void lrcdesktop::mouseReleaseEvent(QMouseEvent * event){
    if(event->button()==Qt::LeftButton)
    {
        int x=this->x();
        int y=this->y();
//        qDebug()<<x<<endl<<y;

        if(this->pos().x()<0) x=0;
        else if(QApplication::desktop()->width()-x<this->width()){
            x=QApplication::desktop()->width()-this->width();
        }

        if(this->pos().y()<0) y=0;
        else if(QApplication::desktop()->height()-y<this->height())
        {
            y=QApplication::desktop()->height()-this->height();
        }
//        qDebug()<<x<<endl<<y;
        move(x,y);
        setCursor(Qt::ArrowCursor);
        event->accept();
        savecfg();
    }
  //  update();
}

void lrcdesktop::savecfg()
{
    QSettings settings("web-cloud-music", "lrcdesktop");
    settings.setValue("nowcolor",nowcolor);
    settings.setValue("nextcolor",nextcolor);
    settings.setValue("bgcolor",bgcolor);
    settings.setValue("alpha",alpha);
    settings.setValue("geometry", this->geometry());
  //  qDebug()<<"save:"<<this->geometry();
}

void lrcdesktop::readcfg()
{
    QSettings settings("web-cloud-music", "lrcdesktop");
    //qDebug()<<settings.fileName();
    nowcolor=settings.value("nowcolor",QString("#ffffff")).toString();
    nextcolor=settings.value("nextcolor",QString("#000000")).toString();
    bgcolor=settings.value("bgcolor",QString("#000000")).toString();
    alpha = settings.value("alpha",30).toInt();
    setGeometry(settings.value("geometry",QRect((QApplication::desktop()->width()-this->width())/2 ,
                                                QApplication::desktop()->height()-100,
                               500 ,100)
                               ).toRect());
    ui->setalpha->setValue(alpha);
    ui->settext->setToolTip(nowcolor);
    ui->nexttext->setToolTip(nextcolor);
    ui->setbg->setToolTip(bgcolor);
}

void lrcdesktop::on_settext_clicked()
{
   QColorDialog * selectcolor = new QColorDialog(this);
    QColor  tmp  = selectcolor->getColor(QColor(ui->settext->toolTip()), this);
    if(tmp.isValid())
    {
           nowcolor = tmp.name();
           ui->settext->setToolTip(nowcolor);
    }
    update();
}

void lrcdesktop::on_setbg_clicked()
{
    QColorDialog* selectcolor = new QColorDialog(this);
    QColor tmp  = selectcolor->getColor(QColor(ui->setbg->toolTip()), this);
    if(tmp.isValid())
    {
           bgcolor = tmp.name();
           ui->setbg->setToolTip(bgcolor);
    }
    update();
}

void lrcdesktop::on_setalpha_valueChanged(int value)
{
    alpha = value ;
    update();
}

void lrcdesktop::on_nexttext_clicked()
{
       QColorDialog * selectcolor = new QColorDialog(this);
        QColor  tmp  = selectcolor->getColor(QColor(ui->nexttext->toolTip()), this);
        if(tmp.isValid())
        {
               nextcolor = tmp.name();
               ui->nexttext->setToolTip(nextcolor);
        }
        update();
}
