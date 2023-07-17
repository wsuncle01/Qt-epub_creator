#include "tree.h"
#include <QDebug>
#include <QTime>
#include <QDate>

QByteArray calcFileCRC(QString file)
{
    if(file==""){
        QByteArray zerocrc;
        zerocrc.resize(4);
        zerocrc[0]='\x00';
        zerocrc[1]='\x00';
        zerocrc[2]='\x00';
        zerocrc[3]='\x00';
        return zerocrc;
    }


    quint32 crc32 = 0xffffffffL;

    //    QByteArray bfile=file.toLocal8Bit();
    QByteArray bfile=file.toUtf8();
    //    qDebug()<<test<<test.size();
    //    QFile file(fileName);
    //    bool result = file.open(QFile::ReadOnly);
    //    if(result) {
    //        while(!file.atEnd()) {
    //            QByteArray data = file.read(1024*1024);
    //            for (auto i = 0; i < data.size(); ++i ) {
    //                crc32 = crc32Table[ ( crc32 ^ data.constData()[ i ]) & 0xff ] ^ ( crc32 >> 8 );
    //            }
    //        }
    //        crc32 = crc32 ^ 0xffffffffL;

    //        file.close();
    //    }
    for (auto i = 0; i < bfile.size(); ++i ) {
        crc32 = crc32Table[ ( crc32 ^ bfile.constData()[ i ]) & 0xff ] ^ ( crc32 >> 8 );
    }
    crc32 = crc32 ^ 0xffffffffL;
    //qDebug()<<QString::number(crc32,16);
    QByteArray crc;
    crc.resize(4);
    crc[0]=(crc32&0x000000ff);
    crc[1]=(crc32&0x0000ff00)>>8;
    crc[2]=(crc32&0x00ff0000)>>16;
    crc[3]=(crc32&0xff000000)>>24;
    return crc;
}

QByteArray fsize(QString file)
{
    unsigned int length;
    length = file.toUtf8().size();
    QByteArray len;
    len.resize(4);
    len[0]=(length&0x000000ff);
    len[1]=(length&0x0000ff00)>>8;
    len[2]=(length&0x00ff0000)>>16;
    len[3]=(length&0xff000000)>>24;
    return len;
}

QByteArray fNsize(QString fileName)
{
    unsigned int length;
    length = fileName.toUtf8().size();
    QByteArray len;
    len.resize(2);
    len[0]=(length&0x000000ff);
    len[1]=(length&0x0000ff00)>>8;
    return len;
}



//前序遍历
void Tree::preOrder()
{
    //qDebug() << "前序遍历: ";
    preOrder(root,"");
//    qDebug()<<offsetrecord<<centralDirFileNum<<centralDirFileSize;
    formEOCD(offsetrecord,centralDirFileNum,centralDirFileSize);
}

void Tree::preOrder(TreeNode* parent,QString path)
{
    if(parent != NULL){
        QString cur=path+"/"+parent->fileName;
        cur.remove(0,2);
        if(parent->isDir){
            cur+="/";
        }
        if(parent!=root){
            unsigned int bais;
            bais=formlocalFile(parent,cur);
            unsigned int Onecdsize;
            Onecdsize=formcentralDir(parent,cur,offsetrecord);
            centralDirFileSize+=Onecdsize;
            offsetrecord+=bais;
            centralDirFileNum+=1;
        }
        fileInfo.filePath.append(cur);
        fileInfo.fileData.append(parent->data);
        fileInfo.isDir.append(parent->isDir);
        preOrder(parent->firstChild,path+"/"+parent->fileName);
        preOrder(parent->nextSibling,path);
    }
}

void Tree::addFile(QStringList filePath, QString data)
{
    QByteArray ZeroByteZero;
    ZeroByteZero.resize(0);
    QByteArray TwoByteZero;
    TwoByteZero.resize(2);
    TwoByteZero[0]='\x00';
    TwoByteZero[1]='\x00';
    QByteArray FourByteZero;
    FourByteZero.resize(4);
    FourByteZero[0]='\x00';
    FourByteZero[1]='\x00';
    FourByteZero[2]='\x00';
    FourByteZero[3]='\x00';
    TreeNode *Parent=root;
    TreeNode *cur;
    int len=filePath.size()-1;
    //qDebug()<<len;
    for(int i=0;i<len;i++){
        cur=Parent->firstChild;
        bool found=false;
        while(cur!=nullptr){
            if(cur->fileName==filePath[i]){
                found=true;
                break;
            }
            cur=cur->nextSibling;
        }
        if(found){
            if(cur->isDir){
                Parent=cur;
            }
            else{
                qDebug()<<"[Tree]Error:Path error";
                return;
            }
        }
        else{
            TreeNode *newDir=new TreeNode;
            newDir->fileName=filePath[i].toUtf8();
            newDir->data=ZeroByteZero;
            newDir->crc=FourByteZero;
            newDir->datasize=FourByteZero;
            newDir->isDir=true;
            newDir->firstChild=nullptr;
            newDir->nextSibling=nullptr;
            addNode(Parent,newDir);
            Parent=newDir;
        }
    }
    TreeNode *newFile=new TreeNode;
    newFile->fileName=filePath[len].toUtf8();
    newFile->crc=calcFileCRC(data);
    newFile->crc=calcFileCRC(data);
    newFile->datasize=fsize(data);
    newFile->data=data.toUtf8();
    newFile->isDir=false;
    newFile->firstChild=nullptr;
    newFile->nextSibling=nullptr;
    addNode(Parent,newFile);

}

void Tree::addDir(QStringList filePath)
{
    QByteArray ZeroByteZero;
    ZeroByteZero.resize(0);
    QByteArray TwoByteZero;
    TwoByteZero.resize(2);
    TwoByteZero[0]='\x00';
    TwoByteZero[1]='\x00';
    QByteArray FourByteZero;
    FourByteZero.resize(4);
    FourByteZero[0]='\x00';
    FourByteZero[1]='\x00';
    FourByteZero[2]='\x00';
    FourByteZero[3]='\x00';
    TreeNode *Parent=root;
    TreeNode *cur;
    int len=filePath.size();
    for(int i=0;i<len;i++){
        cur=Parent->firstChild;
        bool found=false;
        while(cur!=nullptr){
            if(cur->fileName==filePath[i]){
                found=true;
                break;
            }
            cur=cur->nextSibling;
        }
        if(found){
            if(cur->isDir){
                Parent=cur;
            }
            else{
                qDebug()<<"[Tree]Error:Path error";
                return;
            }
        }
        else{
            TreeNode *newDir=new TreeNode;
            newDir->fileName=filePath[i].toUtf8();
            newDir->data=ZeroByteZero;
            newDir->crc=FourByteZero;
            newDir->datasize=FourByteZero;
            newDir->isDir=true;
            newDir->firstChild=nullptr;
            newDir->nextSibling=nullptr;
            addNode(Parent,newDir);
            Parent=newDir;
        }
    }
}


//分配并初始化所有的树结点

//释放所有节点的内存空间
Tree::Tree()
{
    root=new TreeNode;
    root->data=QString::fromStdString("").toUtf8();
    root->fileName=QString::fromStdString("").toUtf8();
    root->crc=QString::fromStdString("").toUtf8();
    root->datasize=QString::fromStdString("").toUtf8();
    root->isDir=true;
    root->firstChild=nullptr;
    root->nextSibling=nullptr;
    localFileStrat=QByteArray::fromRawData("\x50\x4b\x03\x04",4);
    centralDirFileStart=QByteArray::fromRawData("\x50\x4b\x01\x02",4);
    EOCDStrat=QByteArray::fromRawData("\x50\x4b\x05\x06",4);
    localFile=QString::fromStdString("").toUtf8();
    centralDirFile=QString::fromStdString("").toUtf8();
    EOCD=QString::fromStdString("").toUtf8();
    getDate();
    offsetrecord=0;
    centralDirFileSize=0;
    centralDirFileNum=0;
}

Tree::~Tree()
{
    if(root != NULL)
        delete root;
}

//addNode将父子结点组对
//如果父节点的firstChild==NULL, 则firstChild = node;
//如果父节点的firstChild != NULL, 则
void Tree::addNode(TreeNode *parent,TreeNode* node)
{
    if(parent->firstChild == NULL)
        parent->firstChild = node;
    else
        addBrotherNode(parent->firstChild, node);
}

//将节点插入到兄弟节点
void Tree::addBrotherNode(TreeNode* bro, TreeNode* node)
{
    if(bro->fileName==node->fileName){
        qDebug()<<"[Tree]Error:same file name";
        return;
    }
    if(bro->nextSibling == NULL)
        bro->nextSibling = node;
    else
        addBrotherNode(bro->nextSibling, node);
}

void Tree::print()
{
    printFile(root, 0);
    printData(root, 0);
    printIsDir(root, 0);
}

void Tree::printData(TreeNode *node, int num)
{
    if(node != NULL){
        QString space=printSpace(num);
        qDebug() << space + node->data;
        printData(node->firstChild, num+4);
        printData(node->nextSibling, num);
    }
}

void Tree::printIsDir(TreeNode *node, int num)
{
    if(node != NULL){
        QString space=printSpace(num);
        qDebug() << space + QString::number(node->isDir);
        printIsDir(node->firstChild, num+4);
        printIsDir(node->nextSibling, num);
    }
}

QString Tree::printSpace(int num)
{
    QString space="";
    int i = 0;
    for(i = 0; i < num-3; i++)
        space+= " ";

    for(; i < num-2; ++i)
        space+= "|";
    for(; i < num; ++i)
        space+= "_";
    return space;
}

unsigned int Tree::formlocalFile(TreeNode *cur, QString fileName)
{
    QByteArray TwoByteZero;
    TwoByteZero.resize(2);
    TwoByteZero[0]='\x00';
    TwoByteZero[1]='\x00';
    QByteArray lf;
    lf=QString::fromStdString("").toUtf8();
//    Local file header signature;4
    lf.append(localFileStrat,4);
//    Version needed to extract (minimum);2
    lf.append(QString::fromStdString("\x14\x00").toUtf8(),2);
//    General purpose bit flag;2
    lf.append(TwoByteZero,2);
//    Compression method;2
    lf.append(TwoByteZero,2);
//    File last modification time and File last modification date;4
    lf.append(zDate,4);
//    CRC-32 of uncompressed data;4
    lf.append(cur->crc,4);
//    Compressed size;4
    lf.append(cur->datasize,4);
//    Uncompressed size;4
    lf.append(cur->datasize,4);
//    File name length;2
    lf.append(fNsize(fileName),2);
//    Extra field length;2
    lf.append(TwoByteZero,2);
//    File name;n
    lf.append(fileName.toUtf8());
    lf.append(cur->data);
    localFile.append(lf);

    unsigned int uint_offset=lf.size();
    return uint_offset;
}

unsigned int Tree::formcentralDir(TreeNode *cur, QString fileName,unsigned int uint_offset)
{
    QByteArray TwoByteZero;
    TwoByteZero.resize(2);
    TwoByteZero[0]='\x00';
    TwoByteZero[1]='\x00';
    QByteArray FourByteZero;
    FourByteZero.resize(4);
    FourByteZero[0]='\x00';
    FourByteZero[1]='\x00';
    FourByteZero[2]='\x00';
    FourByteZero[3]='\x00';
    QByteArray cd;
    cd=QString::fromStdString("").toUtf8();
//    Central directory file header signature;4
    cd.append(centralDirFileStart,4);
//    Version made by;2
    cd.append(QString::fromStdString("\x14\x00").toUtf8(),2);
//    Version needed to extract (minimum);2
    cd.append(QString::fromStdString("\x14\x00").toUtf8(),2);
//    General purpose bit flag;2
    cd.append(TwoByteZero,2);
//    Compression method;2
    cd.append(TwoByteZero,2);
//    File last modification time and File last modification date;4
    cd.append(zDate,4);
//    	CRC-32 of uncompressed data;4
    cd.append(cur->crc,4);
//    Compressed size;4
    cd.append(cur->datasize,4);
//    Uncompressed size;4
    cd.append(cur->datasize,4);
//    	File name length;2
    cd.append(fNsize(fileName),2);
//    Extra field length;2
    cd.append(TwoByteZero,2);
//    File comment length;2
    cd.append(TwoByteZero,2);
//    File comment length;2
    cd.append(TwoByteZero,2);
//    Internal file attributes;2
    cd.append(TwoByteZero,2);
//    External file attributes;4
    cd.append(FourByteZero,4);
//    Relative offset of local file header;4

    QByteArray offset;
    offset.resize(4);
    offset[0]=(uint_offset&0x000000ff);
    offset[1]=(uint_offset&0x0000ff00)>>8;
    offset[2]=(uint_offset&0x00ff0000)>>16;
    offset[3]=(uint_offset&0xff000000)>>24;
    cd.append(offset,4);
//    File name;n
    cd.append(fileName.toUtf8());

    unsigned int length;
    length=cd.size();
    centralDirFile.append(cd);
    return length;
}

void Tree::formEOCD(unsigned int uint_offset,unsigned int cdNumber,unsigned int size)
{
    QByteArray TwoByteZero;
    TwoByteZero.resize(2);
    TwoByteZero[0]='\x00';
    TwoByteZero[1]='\x00';
    QByteArray eo;
    eo=QString::fromStdString("").toUtf8();
//    End of central directory signature;4
    eo.append(EOCDStrat,4);
//    Number of this disk;2
    eo.append(TwoByteZero,2);
//    Disk where central directory starts;2
    eo.append(TwoByteZero,2);
//    Number of central directory records on this disk;2
    QByteArray cdrecordNum;
    cdrecordNum.resize(2);
    cdrecordNum[0]=(cdNumber&0x000000ff);
    cdrecordNum[1]=(cdNumber&0x0000ff00)>>8;
    eo.append(cdrecordNum,2);
//    Total number of central directory records;2
    eo.append(cdrecordNum,2);
//    Size of central directory;4
    QByteArray esize;
    esize.resize(4);
    esize[0]=(size&0x000000ff);
    esize[1]=(size&0x0000ff00)>>8;
    esize[2]=(size&0x00ff0000)>>16;
    esize[3]=(size&0xff000000)>>24;
    eo.append(esize,4);
//    Offset of start of central directory, relative to start of archive
    QByteArray offset;
    offset.resize(4);
    offset[0]=(uint_offset&0x000000ff);
    offset[1]=(uint_offset&0x0000ff00)>>8;
    offset[2]=(uint_offset&0x00ff0000)>>16;
    offset[3]=(uint_offset&0xff000000)>>24;
    eo.append(offset,4);
//    Comment length;2
    eo.append(TwoByteZero,2);
    EOCD.append(eo);
}



void Tree::getDate()
{
    QTime current_time =QTime::currentTime();
    unsigned int hour=current_time.hour();
    unsigned int mins=current_time.minute();
    unsigned int sec=current_time.second()/2;
    hour=(hour&0x0000001f)<<11;
    mins=(mins&0x0000003f)<<5;
    sec=sec&0x0000001f;
    unsigned int time=hour+mins+sec;
    QByteArray timebyteArray;
    timebyteArray.resize(2);
    timebyteArray[0]=time&0x000000ff;
    timebyteArray[1]=(time&0x0000ff00)>>8;
    QDate current_Date=QDate::currentDate();
    unsigned int year=current_Date.year()-1980;
    unsigned int month=current_Date.month();
    unsigned int day=current_Date.day();
    year=(year&0x0000007f)<<9;
    month=(month&0x0000000f)<<5;
    day=(day&0x0000001f);
    unsigned int date=year+month+day;
    QByteArray datebyteArray;
    datebyteArray.resize(2);
    datebyteArray[0]=date&0x000000ff;
    datebyteArray[1]=(date&0x0000ff00)>>8;
    timebyteArray.append(datebyteArray);
    zDate=timebyteArray;
}


void Tree::printFile(TreeNode* node, int num)
{
    if(node != NULL){
        QString space=printSpace(num);
        qDebug() << space + node->fileName;
        printFile(node->firstChild, num+4);
        printFile(node->nextSibling, num);
    }
}


