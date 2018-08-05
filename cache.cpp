#include "cache.h"
#include "ui_cache.h"

cache::cache(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::cache)
{
    ui->setupUi(this);
    reply = NULL;
    now = 0;
    all = 0;
    working = false;
    cancel = false;
    ui->progressBar->hide();
    this->setWindowTitle("播放记录");
    ui->tableWidget->horizontalHeader()->setVisible(true);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    //ui->tableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    ui->tableWidget->setColumnWidth(2,30);
    //ui->tableWidget->horizontalHeader()->setSortIndicatorShown(false);

}

cache::~cache()
{
    delete ui;
}

bool cache::isworking()
{
    return working;
}

void cache :: setlist(QMap<QString, QString> &map)
{
    if( working ) return;
    //qDebug()<<"map"<<map;
    ui->tableWidget->clearContents();//不清表头
    tasklist.clear();//清除列表
    cancel = false;
    now = 0;
    all = 0;
    ui->progressBar->setMaximum(0);
    ui->progressBar->setValue(0);
    QMap<QString , QString> ::Iterator it;
     QTableWidgetItem *item ;
    int i=0;
    int row = map.size();
    ui->tableWidget->setRowCount(row);//
    for( it = map.begin() ; it!=map.end() ; it++)
    {
        //qDebug()<<"map";
       item = new QTableWidgetItem();
       QString key = it.key();
        item->setText(key.split('-').at(0));
        ui->tableWidget->setItem(i,0,item);

        item = new QTableWidgetItem();        
        item->setText(key.split('-').at(1));
        ui->tableWidget->setItem(i,1,item);

        item = new QTableWidgetItem();
        item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled|Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Unchecked);
        item->setData(Qt::UserRole, it.value());
        ui->tableWidget->setItem(i,2,item);
        i++;//手动计数
    }
    ui->tableWidget->resizeColumnToContents(2);//重新调整选择框那列的大小

}

void cache::on_savecache_clicked()
{
    if(working) {
        cancel = true;
       if(reply)  reply->abort();
       ui->savecache->setText("提取");
       ui->progressBar->hide();
       working = false;
       return;
    }

    QString p = QFileDialog::getExistingDirectory(this , "选择文件夹", path );
    if(p.isEmpty())  return;
    path = p;
    working = true;
    emit pathset(p);
    qDebug()<<"set path:"<<p;
    //开始下载
    QTableWidgetItem *item ;
    for(int i= 0;i<ui->tableWidget->rowCount() ; i++)
    {
        item = ui->tableWidget->item(i , 2);
        if( item->checkState() == Qt::Checked ){
            QString task =  ui->tableWidget->item(i,0)->text()  + "\n" +   item->data(Qt::UserRole).toString();
            tasklist.append(task);
        }
    }
    all = tasklist.length();
    now = 0;
    ui->progressBar->setMaximum(all);
    ui->progressBar->show();
    ui->savecache->setText("取消");

    //qDebug()<<"tasklist:"<<tasklist;
    checkfin();

}

void cache:: setpath(QString&  p){
    if(QDir(p).exists())    path = p;
    else path = QStandardPaths::writableLocation(QStandardPaths::MusicLocation);
}

void cache::checkfin()
{
    ui->progressBar->setValue(now);
    QString text;
    QString name , url ;
    while( now < all && (!cancel)){
        ui->progressBar->setValue(now);
        text = tasklist.at(now);
        name  = text.split('\n').at(0);
        url = text.split('\n').at(1);

        QNetworkRequest request;
        request.setRawHeader(QByteArray("User-Agent"), "Mozilla/5.0 (Windows NT 5.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/39.0.2171.99 Safari/537.36");
        request.setRawHeader(QByteArray("Referer"), "http://music.163.com");
        request.setUrl(QUrl(url) );
        reply = manager.get(request);
        connect(reply , SIGNAL(finished()),this, SLOT(getdata()));
        return;
    }
    ui->progressBar->hide();
    ui->savecache->setText("提取");
    working = false;
}

void cache::getdata(){
    if( cancel ) {
        working = false;
        cancel = false ;
        qDebug()<<"canceled";
        return ;
    }

    QString text;
    QString name , url ;
    text = tasklist.at(now);
    name  = text.split('\n').at(0);
    url = text.split('\n').at(1);
    name = path + "/" +name +"." + url.split('.').last();
    qDebug()<<"cache file name:"<<name;
    QFile file(name);
    if( file.open(QIODevice::WriteOnly)){
        qDebug()<<"wirte file";
        QDataStream data(&file);
        QByteArray m = reply->readAll();
        data<<m;
        qDebug()<<"file wrote";
    }
    reply->deleteLater();
    reply = NULL;
    file.close();

    now++;
    checkfin();
}

void cache::on_selectall_clicked(bool checked)
{
    QTableWidgetItem* item;
    for( int i=0;i<ui->tableWidget->rowCount();i++)
    {
        item = ui->tableWidget->item(i , 2);
        if(checked) item->setCheckState(Qt::Checked);
        else item->setCheckState(Qt::Unchecked);
    }

}

void cache::on_cleancache_clicked()
{
    emit cleanlist();
}

void cache:: closeEvent(QCloseEvent* event){
    hide();//保存后在隐藏
    event->ignore();
}
