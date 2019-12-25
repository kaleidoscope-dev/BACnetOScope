#include "BACnetOScope.h"
#include <QtWidgets/QApplication>



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    BACnetOScope w;
    w.show();
    return a.exec();
}
