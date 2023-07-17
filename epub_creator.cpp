#include "epub_creator.h"
#include "./ui_epub_creator.h"
#include <QtWidgets>
#include <QFile>
#include "epubgenerate.h"
#include <regex>

epub_creator::epub_creator(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::epub_creator)
{
    //std::locale::global(std::locale(""));
    ui->setupUi(this);

    //default outputfile
    outputDir=QDir::homePath();
    authorQString="unknown";
    BnameQString="untitled";
    ui->editAuthor->setPlaceholderText(authorQString);
    ui->editBname->setPlaceholderText(BnameQString);
    displayOutDir();
    //qDebug()<<outputDir;
    connect(ui->Addfile,&QPushButton::clicked,this,&epub_creator::addfile);//添加文件
    connect(ui->deleteFile,&QPushButton::clicked,this,&epub_creator::deletefile);//删除光标选中的文件
    connect(ui->Up,&QPushButton::clicked,this,&epub_creator::up);//选中的文件向上
    connect(ui->Down,&QPushButton::clicked,this,&epub_creator::down);//选中的文件向下
    connect(ui->Clear,&QPushButton::clicked,this,&epub_creator::clear);//清空
    connect(ui->editAuthor,&QLineEdit::editingFinished,this,&epub_creator::getAuthor);//获得作者名
    connect(ui->editBname,&QLineEdit::editingFinished,this,&epub_creator::getBname);//获得书名
    connect(ui->CreateAsOne,&QPushButton::clicked,this,&epub_creator::createAsOne);//创建一本书
    connect(ui->setOutput,&QPushButton::clicked,this,&epub_creator::setOutput);//获得输出目录
    connect(ui->CreateSep,&QPushButton::clicked,this,&epub_creator::createSep);
}

epub_creator::~epub_creator()
{
    delete ui;
}

//添加文件到filelist中并显示到fileview中
void epub_creator::addfile()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(
        this,
        "选择文件",
        "D:/",
        "文档(*.txt)");
    if (!fileNames.isEmpty()) {
        for(QString i:fileNames){
            filelist.append(i);
        }
        ui->fileView->clear();
        ui->fileView->addItems(filelist);
    }
    if(!filelist.empty()){
        ui->fileView->setCurrentRow(0);
    }
    /*
    for(QString i:filelist){
        qDebug()<<i;
    }
    */
}

//删除fileview中选中的行
void epub_creator::deletefile()
{
    if(ui->fileView->currentItem()!=nullptr){
        int currRow = ui->fileView->currentRow();
        filelist.removeAt(currRow);
        ui->fileView->clear();
        ui->fileView->addItems(filelist);
        int len=filelist.length();
        if(currRow<len){
            ui->fileView->setCurrentRow(currRow);
        }
        else{
            ui->fileView->setCurrentRow(len-1);
        }
    }

}

//使得fileview中光标选中的行向上移动
void epub_creator::up()
{
    if(ui->fileView->currentItem()!=nullptr){
        int currRow = ui->fileView->currentRow();
        if(currRow>0){
            QString selectline=filelist[currRow];
            filelist[currRow]=filelist[currRow-1];
            filelist[currRow-1]=selectline;
            ui->fileView->clear();
            ui->fileView->addItems(filelist);
            currRow--;
        }
        ui->fileView->setCurrentRow(currRow);
    }
}

//使得fileview中光标选中的行向下移动
void epub_creator::down()
{
    if(ui->fileView->currentItem()!=nullptr){
        int currRow = ui->fileView->currentRow();
        if(currRow<filelist.length()-1){
            QString selectline=filelist[currRow];
            filelist[currRow]=filelist[currRow+1];
            filelist[currRow+1]=selectline;
            ui->fileView->clear();
            ui->fileView->addItems(filelist);
            currRow++;
        }
        ui->fileView->setCurrentRow(currRow);
    }
}


//清除所有fileview里面的内容,并清空filelist
void epub_creator::clear()
{
    filelist.clear();
    ui->fileView->clear();
    ui->fileView->addItems(filelist);
}

//当作者栏输入完成的时候，将修改内容填入authorQString
void epub_creator::getAuthor()
{
    authorQString=ui->editAuthor->text();
    //qDebug()<<authorQString;
}

//当书名栏输入完成的时候，将修改内容填入BnameQString
void epub_creator::getBname()
{
    BnameQString=ui->editBname->text();
}

//将filelist中的文件以Chapter的形式生成一本书
void epub_creator::createAsOne()
{
    /*
    if(filelist.size()==0){
        return;
    }
    */
    EpubGenerate *Book=new EpubGenerate;
    Book->generate(authorQString,BnameQString,filelist,outputDir);
    delete Book;
}

//设置输出目录
void epub_creator::setOutput()
{
    outputDir = QFileDialog::getExistingDirectory(0, ("选择输出目录"), QDir::currentPath());
    displayOutDir();
    //ui->output->setText(outputDir);
}

//显示输出目录
void epub_creator::displayOutDir()
{
    QFontMetrics fontWidth(ui->output->font());
    QString elideNote = fontWidth.elidedText(outputDir, Qt::ElideMiddle, 200);
    ui->output->setText(elideNote);
    ui->output->setToolTip(outputDir);
}

void epub_creator::createSep()
{
    for(int i=0;i<filelist.size();i++){
        QStringList f;
        f.append(filelist[i]);
        std::smatch results;
        std::string str=filelist[i].toStdString();
        std::string pattern = "/([^/]+).txt$";
        std::regex re(pattern);
        QString title;
        if(std::regex_search(str, results, re)){
            title=QString::fromStdString(results[1].str());
        }
        EpubGenerate *Book=new EpubGenerate;
        Book->generate("",title,f,outputDir);
        delete Book;
    }
}

