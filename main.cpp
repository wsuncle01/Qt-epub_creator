#include "epub_creator.h"

#include <QApplication>


int main(int argc, char *argv[])
{
    QLocale::setDefault(QLocale::c());
    QApplication a(argc, argv);
    epub_creator w;
    w.show();
    return a.exec();
}
