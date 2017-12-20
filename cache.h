#ifndef CACHE_H
#define CACHE_H

#include <QWidget>
#include<QHeaderView>
#include<QNetworkDiskCache>
#include<QNetworkAccessManager>
#include<QNetworkReply>
#include<QNetworkRequest>
#include<QDataStream>
#include<QFile>
#include<QFileDialog>
#include<QStandardPaths>
#include<QTableWidgetItem>
#include<QDebug>
namespace Ui {
class cache;
}

class cache : public QWidget
{
    Q_OBJECT

public:
    explicit cache(QWidget *parent = 0);
    ~cache();
    void setlist(QMap<QString , QString> &);
    void setpath(QString&);
    void setcache(QNetworkDiskCache*);
    bool isworking();

signals:
    void pathset(QString& );

private slots:
    void on_savecache_clicked();
    void getdata();

    void on_cleancache_clicked();

    void on_selectall_clicked(bool checked);

private:
    void checkfin();

    Ui::cache *ui;
    QNetworkDiskCache *diskcache;
    QNetworkReply *reply;
    QNetworkAccessManager manager;
    int now , all;
    QStringList tasklist;
    QString path;
    bool working , cancel;
};

#endif // CACHE_H
