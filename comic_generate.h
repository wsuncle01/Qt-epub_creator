#ifndef COMIC_GENERATE_H
#define COMIC_GENERATE_H
#include <QString>
#include "comic_creator.h"
#include <QList>

class Comic_generate
{
private:
    QString cssFile;
    QString containerXML;
    QString mimetype;
    QString Buuid;
    QString date;
    QString Content_Opf;
    QString Toc_Ncx;
    QStringList ChapterNameList;
    QStringList contextList;
    QStringList fileNameList;
    QStringList filePathList;
    QList<QStringList> file;

public:
    Comic_generate();
    void setDate();
    void setUuid();
    void LoadSrc();
    void setTocncx(QString ComicName);
    void getContext(QList<chapterNode> filePath,QStringList picList);
    void setContentopf(QString author,QString ComicName);
    void reNameSameChapterTitle();
    void generate(QList<chapterNode> filePath,QStringList picList,QString author,QString ComicName,QString outDir);

};

#endif // COMIC_GENERATE_H
