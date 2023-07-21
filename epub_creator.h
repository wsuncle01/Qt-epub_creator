#ifndef EPUB_CREATOR_H
#define EPUB_CREATOR_H

#include <QMainWindow>
#include <QFrame>
#include <QComboBox>

QT_BEGIN_NAMESPACE
namespace Ui { class epub_creator; }
QT_END_NAMESPACE

class epub_creator : public QMainWindow
{
    Q_OBJECT

    //data
private:
    QStringList  filelist;
    QString authorQString;
    QString BnameQString;
    QString outputDir;
    QStringList mods;
    //text
public:
    epub_creator(QWidget *parent = nullptr);
    ~epub_creator();
public:
    void addfile();//添加文件到filelist中并显示到fileview中
    void deletefile();//删除fileview中选中的行
    void up();//使得fileview中光标选中的行向上移动
    void down();//使得fileview中光标选中的行向下移动
    void clear();//清除所有fileview里面的内容,并清空filelist
    void getAuthor();//当作者栏输入完成的时候，将修改内容填入authorQString
    void getBname();//当书名栏输入完成的时候，将修改内容填入BnameQString
    void createAsOne();//将filelist中的文件以Chapter的形式生成一本书
    void setOutput();//设置输出目录
    void displayOutDir();//显示输出目录
    void createSep();
public slots:
    void ChangeMod();
    //ui
private:
    Ui::epub_creator *ui;
};
#endif // EPUB_CREATOR_H
