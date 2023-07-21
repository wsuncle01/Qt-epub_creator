#include "comic_creator.h"
#include "ui_comic_creator.h"
#include "epub_creator.h"
#include <QtWidgets>
#include <QKeyEvent>
#include <QMouseEvent>
#include "comic_generate.h"

Comic_Creator::Comic_Creator(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Comic_Creator)
{

    ui->setupUi(this);
    outputDir=QDir::homePath();
    displayOutDir();
    mods<<"PureTextBook"<<"Comic";
    ui->ModSelection->addItems(mods);
    ui->ModSelection->setCurrentIndex(1);
    ui->picView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    authorQString="unknown";
    CnameQString="untitled";
    ui->editAuthor->setPlaceholderText(authorQString);
    ui->editBname->setPlaceholderText(CnameQString);
    ui->editChapName->setClearButtonEnabled(true);
    ui->editAuthor->setClearButtonEnabled(true);
    ui->editBname->setClearButtonEnabled(true);
//    ui->picView->setMovement(QListView::Static);
    picListOffset=0;
    keyInfo=0;
    connect(ui->ModSelection, SIGNAL(currentIndexChanged(int)), this, SLOT(ChangeMod()));
    connect(ui->AddPic,SIGNAL(clicked(bool)),this,SLOT(addfile()));
    connect(ui->Clear,SIGNAL(clicked(bool)),this,SLOT(clear()));
    connect(ui->AddChap,SIGNAL(clicked(bool)),this,SLOT(createNewChapter()));
    connect(ui->delChap,SIGNAL(clicked(bool)),this,SLOT(deleteChapter()));
    connect(ui->picView,SIGNAL(itemSelectionChanged()),this,SLOT(modifyChCB()));
    connect(ui->add2chap,SIGNAL(clicked(bool)),this,SLOT(add2Chapter()));
    connect(ui->deletePic,SIGNAL(clicked(bool)),this,SLOT(delPic()));
    connect(ui->renameChap,SIGNAL(clicked(bool)),this,SLOT(changeChapName()));
    connect(ui->editAuthor,SIGNAL(editingFinished()),this,SLOT(getAuthor()));
    connect(ui->editBname,SIGNAL(editingFinished()),this,SLOT(getCname()));
    connect(ui->picUp,SIGNAL(clicked(bool)),this,SLOT(picUp()));
    connect(ui->picDown,SIGNAL(clicked(bool)),this,SLOT(picDown()));
    connect(ui->chapUp,SIGNAL(clicked(bool)),this,SLOT(Up()));
    connect(ui->chapDown,SIGNAL(clicked(bool)),this,SLOT(Down()));
//    connect(ui->editChapName,&QLineEdit::editingFinished,this,&Comic_Creator::changeChapName);
    connect(ui->generateComic,SIGNAL(clicked(bool)),this,SLOT(CreateComic()));
    connect(ui->setOutput,SIGNAL(clicked(bool)),this,SLOT(setOutput()));//获得输出目录
}

Comic_Creator::~Comic_Creator()
{
    delete ui;
}

void Comic_Creator::ChangeMod()
{
    int curIndex=ui->ModSelection->currentIndex();
    if(curIndex==0){
        this->close();
        epub_creator *e=new epub_creator;
        e->show();
    }
}

void Comic_Creator::addfile()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(
        this,
        "选择文件",
        "C:/",
        "图片(*.jpg;*.webp;*.png;*.jpeg)");
    if (!fileNames.isEmpty()) {
        QStringList filelist;
        for(QString i:fileNames){
            filelist.append(i);
        }
        picList.append(filelist);
        displayFile();
    }
}

void Comic_Creator::displayFile()
{
    ui->picView->clear();
    for(int i=0;i<chapterList.size();i++){
        ui->picView->addItem(chapterList[i].chapterName);
        for(int j=0;j<chapterList[i].dataList.size();j++){
            QFontMetrics fontWidth(ui->picView->font());
            QString elideNote = fontWidth.elidedText(chapterList[i].dataList[j], Qt::ElideLeft, 200);
            ui->picView->addItem(elideNote);
        }
    }
    if(!picList.empty()){
        ui->picView->addItem("No Chapter Pic");
        for(int i=0;i<picList.size();i++){
            QFontMetrics fontWidth(ui->picView->font());
            QString elideNote = fontWidth.elidedText(picList[i], Qt::ElideLeft, 200);
            ui->picView->addItem(elideNote);
        }
    }

}

void Comic_Creator::clear()
{
    picList.clear();
    int len=chapterList.size();
    for(int i=0;i<len;i++){
        chapterList.removeAt(0);
    }
    ui->ChapNameCB->clear();
//    ui->toChapCB->clear();
    displayFile();
}

void Comic_Creator::createNewChapter()
{
    QString cName="Ch"+QString::number(chapterList.size()+1,10);
    chapterNode newchapter;
    newchapter.chapterName=cName;
    unsigned int cur=0;
    for(int i=0;i<chapterList.size();i++){
        cur=chapterList[i].end;
    }
    newchapter.offset=cur;
    newchapter.end=cur+1;
    picListOffset=cur+1;
    chapterList.append(newchapter);
    chapterNameList.append(cName);
    displayChapter();
    displayFile();
    /*testOffset()*/;
}

void Comic_Creator::deleteChapter()
{
    if(chapterList.empty()){
        return;
    }
    int cur=ui->ChapNameCB->currentIndex();
    picList.append(chapterList[cur].dataList);
//    qDebug()<<1;
    int curoffset=chapterList[cur].offset;
//    qDebug()<<1;
    for(int i=cur+1;i<chapterList.size();i++){
        chapterList[i].offset=curoffset;
        chapterList[i].end=curoffset+chapterList[i].dataList.size()+1;
        curoffset=chapterList[i].end;
    }
    picListOffset=curoffset;
    chapterNameList.removeAt(cur);
    chapterList.removeAt(cur);
    displayChapter();
    displayFile();
//    testOffset();
}

void Comic_Creator::displayChapter()
{
    ui->ChapNameCB->clear();
    ui->ChapNameCB->addItems(chapterNameList);
    ui->ChapNameCB->setCurrentIndex(chapterNameList.size()-1);
}

void Comic_Creator::testOffset()
{
    QStringList testoffset;
    QStringList testend;
    for(int i=0;i<chapterList.size();i++){
        testend<< chapterNameList[i] <<QString::number(chapterList[i].end,10);
        testoffset<< chapterNameList[i]<<QString::number(chapterList[i].offset,10);
    }
    testoffset<<"PicListOffSet"<<QString::number(picListOffset,10);
    qDebug()<<testoffset;
    qDebug()<<testend;
}

void Comic_Creator::modifyChCB()
{

    //ui->picView->selectedItems();
//    QList<QListWidgetItem *> test = ui->picView->selectedItems();
//    for(int i=0;i<test.size();i++){
//        qDebug()<<test[i]->textAlignment();
//    }
//    qDebug()<<keyInfo;
    this->grabKeyboard();
    unsigned int currow=ui->picView->currentRow();
    if(keyInfo==0){
        selectedIndex.clear();
        selectedIndex.append(currow);
        //qDebug()<<currow;
        for(int i=0;i<chapterList.size();i++){
            if(currow>=chapterList[i].offset&&currow<chapterList[i].end){
                ui->ChapNameCB->setCurrentIndex(i);
                break;
            }
        }
    }
    if(keyInfo==1){
        if(selectedIndex.contains(currow)){
            selectedIndex.removeAll(currow);
        }
        else{
            selectedIndex.append(currow);
        }
    }
    if(keyInfo==2){
        if(selectedIndex.empty()){
            selectedIndex.append(currow);
        }
        else{
            unsigned int start=selectedIndex[0];
            selectedIndex.clear();
            selectedIndex.append(start);
            if(currow<start){
                for(unsigned int i=currow;i<start;i++){
                    selectedIndex.append(i);
                }
            }
            else{
                for(unsigned int i=start+1;i<=currow;i++){
                    selectedIndex.append(i);
                }
            }
        }
    }
    this->releaseKeyboard();
//    qDebug()<<selectedIndex;
    //    qDebug()<<currow;
}

void Comic_Creator::add2Chapter()
{
    if(selectedIndex.empty()&&picList.empty()){
        return;
    }
    QStringList newdata;
    for(unsigned int i=0;i<selectedIndex.size();i++){
        if(selectedIndex[i]>picListOffset){
            unsigned int index=selectedIndex[i]-picListOffset;
            newdata.append(picList[index-1]);
        }
        else{
            for(unsigned int j=0;j<chapterList.size();j++){
                if(selectedIndex[i]>chapterList[j].offset&&selectedIndex[i]<chapterList[j].end){
                    unsigned int index=selectedIndex[i]-chapterList[j].offset;
                    newdata.append(chapterList[j].dataList[index-1]);
                }
            }
        }
    }
//    unsigned int count=0;
//    for(unsigned int i=0;i<selectedIndex.size();i++){
//        if(selectedIndex[i]>picListOffset){
//            unsigned int index=selectedIndex[i]-picListOffset;
//            picList.removeAt(index-1-count);
//            count++;
//        }
//        else{
//            for(unsigned int j=0;j<chapterList.size();j++){
//                if(selectedIndex[i]>chapterList[j].offset&&selectedIndex[i]<chapterList[j].end){
//                    unsigned int index=selectedIndex[i]-picListOffset;
//                    chapterList[j].dataList.removeAt(index-1-count);
//                    count++;
//                }
//            }
//        }
//    }
    for(unsigned int i=0;i<selectedIndex.size();i++){
        if(selectedIndex[i]>picListOffset){
            unsigned int index=selectedIndex[i]-picListOffset;
            picList[index-1]="";
        }
        else{
            for(unsigned int j=0;j<chapterList.size();j++){
                if(selectedIndex[i]>chapterList[j].offset&&selectedIndex[i]<chapterList[j].end){
                    unsigned int index=selectedIndex[i]-chapterList[j].offset;
                    chapterList[j].dataList[index-1]="";
                }
            }
        }
    }
    for(unsigned int i=0;i<chapterList.size();i++){
        chapterList[i].dataList.removeAll("");
    }
    picList.removeAll("");
    unsigned int cur;
    cur=ui->ChapNameCB->currentIndex();
    chapterList[cur].dataList.append(newdata);
    int curoffset=0;
    for(int i=0;i<chapterList.size();i++){
        chapterList[i].offset=curoffset;
        chapterList[i].end=curoffset+chapterList[i].dataList.size()+1;
        curoffset=chapterList[i].end;
    }
    picListOffset=curoffset;
    displayChapter();
    displayFile();
//    testOffset();
}

void Comic_Creator::delPic()
{
    for(unsigned int i=0;i<selectedIndex.size();i++){
        if(selectedIndex[i]>picListOffset){
            unsigned int index=selectedIndex[i]-picListOffset;
            picList[index-1]="";
        }
        else{
            for(unsigned int j=0;j<chapterList.size();j++){
                if(selectedIndex[i]>chapterList[j].offset&&selectedIndex[i]<chapterList[j].end){
                    unsigned int index=selectedIndex[i]-chapterList[j].offset;
                    chapterList[j].dataList[index-1]="";
                }
            }
        }
    }
    qDebug()<<1;
    for(unsigned int i=0;i<chapterList.size();i++){
        chapterList[i].dataList.removeAll("");
    }
    picList.removeAll("");
    if(!chapterList.empty()){
        int curoffset=0;
        for(int i=0;i<chapterList.size();i++){
            chapterList[i].offset=curoffset;
            chapterList[i].end=curoffset+chapterList[i].dataList.size()+1;
            curoffset=chapterList[i].end;
        }
        picListOffset=curoffset;
    }
    else{
        picListOffset=0;
    }
    displayChapter();
    displayFile();
//    testOffset();

}

void Comic_Creator::changeChapName()
{
    unsigned int row;
    row=ui->ChapNameCB->currentIndex();
    if(row>chapterList.size()||row<0){
        return;
    }
    chapterNameList[row]=ui->editChapName->text();
    chapterList[row].chapterName=ui->editChapName->text();
    //ui->editChapName->clear();
//    ui->editChapName->setEnabled(true);

    ui->editChapName->setReadOnly(false);
    displayChapter();
    displayFile();
//    testOffset();
}

void Comic_Creator::getAuthor()
{
    authorQString=ui->editAuthor->text();
    qDebug()<<authorQString;
}

void Comic_Creator::getCname()
{
    CnameQString=ui->editBname->text();
    qDebug()<<CnameQString;
}

void Comic_Creator::picUp()
{
    for(int i=0;i<selectedIndex.size();i++){
        if(selectedIndex[i]>picListOffset){
            unsigned int index=selectedIndex[i]-picListOffset-1;
            if(index>0){
                QString selectline=picList[index];
                picList[index]=picList[index-1];
                picList[index-1]=selectline;
                index--;
            }
            displayChapter();
            displayFile();
            ui->picView->setCurrentRow(picListOffset+index+1);
        }
        else{
            for(unsigned int j=0;j<chapterList.size();j++){
                if(selectedIndex[i]>chapterList[j].offset&&selectedIndex[i]<chapterList[j].end){
                    unsigned int index=selectedIndex[i]-chapterList[j].offset-1;
                    if(index>0){
                        QString selectline=chapterList[j].dataList[index];
                        chapterList[j].dataList[index]=chapterList[j].dataList[index-1];
                        chapterList[j].dataList[index-1]=selectline;
                        index--;
                    }
                    displayChapter();
                    displayFile();
                    ui->picView->setCurrentRow(chapterList[j].offset+index+1);
                    break;
                }
            }
        }
    }
}

void Comic_Creator::picDown()
{
    for(int i=0;i<selectedIndex.size();i++){
        if(selectedIndex[i]>picListOffset){
            unsigned int index=selectedIndex[i]-picListOffset-1;
            if(index<picList.size()-1){
                QString selectline=picList[index];
                picList[index]=picList[index+1];
                picList[index+1]=selectline;
                index++;
            }
            displayChapter();
            displayFile();
            ui->picView->setCurrentRow(picListOffset+index+1);
        }
        else{
            for(unsigned int j=0;j<chapterList.size();j++){
                if(selectedIndex[i]>chapterList[j].offset&&selectedIndex[i]<chapterList[j].end){
                    unsigned int index=selectedIndex[i]-chapterList[j].offset-1;
                    if(index<chapterList[j].dataList.size()-1){
                        QString selectline=chapterList[j].dataList[index];
                        chapterList[j].dataList[index]=chapterList[j].dataList[index+1];
                        chapterList[j].dataList[index+1]=selectline;
                        index++;
                    }
                    displayChapter();
                    displayFile();
                    ui->picView->setCurrentRow(chapterList[j].offset+index+1);
                    break;
                }
            }
        }
    }
}

void Comic_Creator::Up()
{
    if(ui->ChapNameCB->currentIndex()<chapterList.size()){
        int index = ui->ChapNameCB->currentIndex();
        if(index>0){
            chapterNode selectChap=chapterList[index];
            chapterList[index]=chapterList[index-1];
            chapterList[index-1]=selectChap;
            QString seletCh=chapterNameList[index];
            chapterNameList[index]=chapterNameList[index-1];
            chapterNameList[index-1]=seletCh;
            index--;
        }
        int curoffset=0;
        for(int i=0;i<chapterList.size();i++){
            chapterList[i].offset=curoffset;
            chapterList[i].end=curoffset+chapterList[i].dataList.size()+1;
            curoffset=chapterList[i].end;
        }
        displayChapter();
        displayFile();
        ui->picView->setCurrentRow(chapterList[index].offset);
        ui->ChapNameCB->setCurrentIndex(index);
//        testOffset();
    }
}

void Comic_Creator::Down()
{
    if(ui->ChapNameCB->currentIndex()<chapterList.size()){
        int index = ui->ChapNameCB->currentIndex();
        if(index<chapterList.size()-1){
            chapterNode selectChap=chapterList[index];
            chapterList[index]=chapterList[index+1];
            chapterList[index+1]=selectChap;
            QString seletCh=chapterNameList[index];
            chapterNameList[index]=chapterNameList[index+1];
            chapterNameList[index+1]=seletCh;
            index++;
        }
        int curoffset=0;
        for(int i=0;i<chapterList.size();i++){
            chapterList[i].offset=curoffset;
            chapterList[i].end=curoffset+chapterList[i].dataList.size()+1;
            curoffset=chapterList[i].end;
        }
        displayChapter();
        displayFile();
        ui->picView->setCurrentRow(chapterList[index].offset);
        ui->ChapNameCB->setCurrentIndex(index);
        /*testOffset()*/;
    }
}

void Comic_Creator::CreateComic()
{
    Comic_generate Cgenerate;
    Cgenerate.generate(chapterList,picList,authorQString,CnameQString,outputDir);
}

void Comic_Creator::setOutput()
{
    outputDir = QFileDialog::getExistingDirectory(0, ("选择输出目录"), QDir::currentPath());
    displayOutDir();
}

void Comic_Creator::displayOutDir()
{
    QFontMetrics fontWidth(ui->outputL->font());
    QString elideNote = fontWidth.elidedText(outputDir, Qt::ElideMiddle, 200);
    ui->outputL->setText(elideNote);
    ui->outputL->setToolTip(outputDir);
}


void Comic_Creator::keyPressEvent(QKeyEvent *event)
{
    if(event->key()==Qt::Key_Control)
    {
        keyInfo=1;
//        qDebug()<<keyInfo;
    }
    if(event->key()==Qt::Key_Shift){
        keyInfo=2;
    }
}

void Comic_Creator::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key()==Qt::Key_Control)
    {
        keyInfo=0;
//        qDebug()<<keyInfo;
    }
    if(event->key()==Qt::Key_Shift){
        keyInfo=0;
    }
}
