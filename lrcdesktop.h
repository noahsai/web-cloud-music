#ifndef LRCDESKTOP_H
#define LRCDESKTOP_H

#include <QWidget>
#include<QMouseEvent>
#include<QFont>
#include<QPainter>
#include<QDesktopWidget>
#include<QDebug>
#include<QSettings>
#include<QColorDialog>


namespace Ui {
class lrcdesktop;
}

class lrcdesktop : public QWidget
{
    Q_OBJECT

public:
    explicit lrcdesktop(QWidget *parent = 0);
    void setlrc(QString);
    ~lrcdesktop();
public slots:
    void tolock(bool);

signals :
    void closed(bool);

private slots:
    void on_settext_clicked();

    void on_setbg_clicked();

    void on_setalpha_valueChanged(int value);

    void on_nexttext_clicked();

private:
    Ui::lrcdesktop *ui;

    void mousePressEvent(QMouseEvent * event);
    void mouseMoveEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent * event);

    void paintEvent(QPaintEvent* event);
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void closeEvent(QCloseEvent *event);
    void savecfg();
    void readcfg();
    bool entered , locked,setting;
    QString lrctext,tmptosetting;
    int nowtext;
    QPoint oldpos;
    QString nowcolor ,nextcolor ,bgcolor ;
    int alpha;

};

#endif // LRCDESKTOP_H
