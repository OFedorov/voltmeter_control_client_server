#include "controlboard.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ControlBoard w;
    w.show();
    return a.exec();
}
