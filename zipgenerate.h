#ifndef ZIPGENERATE_H
#define ZIPGENERATE_H

#include <QObject>
#include <QWidget>
#include <string>
#include <vector>
class ZipGenerate
{
private:    QStringList *fileName;
    QStringList *fileData;
    QString localFile;
    QString centralDirFile;
    QString EOCD;
public:
    ZipGenerate();
    void addfile(QString Data,QString fName);
};

#endif // ZIPGENERATE_H
