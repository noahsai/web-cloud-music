#include "webmusic.h"
#include <QApplication>
#include<lrcdesktop.h>
#include<cache.h>
#include<QMap>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    webmusic w;
 //   lrcdesktop w;
//    QMap<QString , QString> m;
//    m["Breath and Life\tAudio Machine"] = "http://m10.music.126.net/20171219143734/fc0c1d42a18bcf17e8cd4ce06eb813a4/ymusic/18ba/7e9f/69fd/75f095ea5e4031ec40a8f7b16e39ba81.mp3";
//   cache w;
//   w.setlist( m);
    w.show();

    return a.exec();
}
