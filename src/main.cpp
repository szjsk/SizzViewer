#include "SzViewer.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SzViewer w;
    w.show();
    return a.exec();
}
