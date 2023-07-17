#ifndef ZIP_H
#define ZIP_H
#include "tree.h"
#include <QString>
#include <sstream>

class Zip
{
private:
    Tree ziptree;
public:
    Zip();
    void zipfile(QString OutPutPath);
    QByteArray getcrc32(QString data);
    QByteArray getlength(QString data);
    void addfile(QString path,QString data);
};

#endif // ZIP_H
