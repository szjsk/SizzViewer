#include "SizzViewer.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SizzViewer w;
    w.show();
    return a.exec();
}
