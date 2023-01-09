#include "deviceServer.h"
#include <QtWidgets/QApplication>
#include "LogSrc/customMessageHandle.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    deviceServer w;
    w.show();
    return a.exec();
}
