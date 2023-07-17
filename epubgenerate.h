#ifndef EPUBGENERATE_H
#define EPUBGENERATE_H

#include <QObject>
#include <QWidget>
#include <regex>

class EpubGenerate
{
private:
    QStringList contextlist;
    QStringList EpubfilePath;
    QStringList chapterTitle;
    QString cssFile;
    QString containerXML;
    QString mimetype;
    QString Buuid;
    QString date;
    QString Content_Opf;
    QString Toc_Ncx;
    QStringList strListFile;
public:
    EpubGenerate();
    void getContext(QStringList filePath);
    void generate(QString author,QString BookName,QStringList filePath,QString outDir);
    void mk_dir(QString dirPath);
    void initDir(QString outDir,QString BookName);
    void LoadSrc();
    void setContentopf(QString author,QString BookName);
    void setDate();
    void setUuid();
    void setTocncx(QString BookName);
    void reNameSameChapterTitle();
//    void outPutFile(QString outDir,QString BookName);
//    void Zip2Epub(QString outDir,QString BookName);
};

#endif // EPUBGENERATE_H
