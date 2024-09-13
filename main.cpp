#include "voplatform.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    VOPlatForm w;
    w.show();
    return a.exec();
}
