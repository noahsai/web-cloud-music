#ifndef CACHE_H
#define CACHE_H

#include <QWidget>
#include<QHeaderView>
#include<QNetworkAccessManager>
#include<QNetworkReply>
#include<QNetworkRequest>
#include<QDataStream>
#include<QFile>
#include<QFileDialog>
#include<QStandardPaths>
#include<QTableWidgetItem>
#include<QDebug>
#include<QCloseEvent>
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
    bool isworking();

signals:
    void pathset(QString& );
    void cleanlist();

private slots:
    void on_savecache_clicked();
    void getdata();


    void on_selectall_clicked(bool checked);

    void on_cleancache_clicked();

private:
    void checkfin();
    void closeEvent(QCloseEvent *event);

    Ui::cache *ui;
    QNetworkReply *reply;
    QNetworkAccessManager manager;
    int now , all;
    QStringList tasklist;
    QString path;
    bool working , cancel;
};

#endif // CACHE_H
