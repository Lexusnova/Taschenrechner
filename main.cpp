#include "Taschenrechner.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Taschenrechner w;
    w.show();
    return a.exec();
}
