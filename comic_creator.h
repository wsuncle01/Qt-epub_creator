#ifndef COMIC_CREATOR_H
#define COMIC_CREATOR_H

#include <QMainWindow>
#include <QStringList>
#include <QKeyEvent>
#include <QMouseEvent>

struct chapterNode
{
    QStringList dataList;
    QString chapterName;
    unsigned int offset;
    unsigned int end;
    QString authorQString;
    QString CNameQString;
    void operator=(chapterNode otherChapterNode){
        dataList=otherChapterNode.dataList;
        chapterName=otherChapterNode.chapterName;
        offset=otherChapterNode.offset;
        end=otherChapterNode.end;
        authorQString=otherChapterNode.authorQString;
        CNameQString=otherChapterNode.CNameQString;
    }
};
namespace Ui {
class Comic_Creator;
}

class Comic_Creator : public QMainWindow
{
    Q_OBJECT
private:
    QStringList mods;
    unsigned int picListOffset;
    QStringList picList;
    QList<chapterNode> chapterList;
    QStringList chapterNameList;
    unsigned int keyInfo;
    QList<unsigned int> selectedIndex;
    QString authorQString;
    QString CnameQString;
    QString outputDir;
public:
    explicit Comic_Creator(QWidget *parent = nullptr);
    ~Comic_Creator();

public slots:
    void ChangeMod();
    void addfile();
    void displayFile();
    void clear();
    void createNewChapter();
    void deleteChapter();
    void displayChapter();
    void testOffset();
    void modifyChCB();
    void add2Chapter();
    void delPic();
    void changeChapName();
    void getAuthor();
    void getCname();
    void picUp();
    void picDown();
    void Up();
    void Down();
    void CreateComic();
    void setOutput();//设置输出目录
    void displayOutDir();//显示输出目录


protected:
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void keyReleaseEvent(QKeyEvent *event);
private:
    Ui::Comic_Creator *ui;
};

#endif // COMIC_CREATOR_H
