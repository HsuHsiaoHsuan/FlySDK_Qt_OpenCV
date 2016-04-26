#include "widget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.show();

    FlyCapture2::Error error;
    FlyCapture2::BusManager busMgr;

    return a.exec();
}
