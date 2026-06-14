#include "view/openwidget.h"
#include <QApplication>
#include "view/settingwidget.h"
#include "view/loginwidget.h"
#include <QTime>
#include "view/mainwidget.h"
#include "view/videoplayerwidget.h"


int main(int argc, char *argv[])
{
    CTool::init_crc_table();
    qsrand(QTime::currentTime().msec());

    QApplication a(argc, argv);
    openWidget w;
    w.show();

    return a.exec();
}
