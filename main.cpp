#include "turbocapture.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TurBoCapture w;
    w.show();

    return a.exec();
}
