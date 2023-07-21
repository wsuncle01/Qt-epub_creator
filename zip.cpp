#include "zip.h"
#include <fstream>
#include <QFile>
#include <QDebug>
#include <QDataStream>
#include "tree.h"


Zip::Zip()
{
//
}

void Zip::zipfile(QString OutPutPath)
{
    ziptree.preOrder();
    QFile file(OutPutPath);
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "Cannot open file for writing: "
                 << qPrintable(file.errorString());
        return;
    }
    QByteArray outfileData;
    outfileData=QString::fromStdString("").toUtf8();
    outfileData.append(ziptree.localFile);
    outfileData.append(ziptree.centralDirFile);
    outfileData.append(ziptree.EOCD);
    QDataStream out(&file);
    out.writeRawData(outfileData,outfileData.end()-outfileData.begin());
    file.close();
}


QByteArray Zip::getcrc32(QString data)
{
    QByteArray crcval;
    crcval=calcFileCRC(data);
    return crcval;
}

QByteArray Zip::getlength(QString data)
{
    QByteArray length;
    length=fsize(data);
    return length;
}

void Zip::addfile(QString path, QString data)
{
    QStringList filepath;
    filepath=path.split('/');
    ziptree.addFile(filepath,data);
}

void Zip::addfile(QString path, QByteArray data)
{
    QStringList filepath;
    filepath=path.split('/');
    ziptree.addFile(filepath,data);
}



