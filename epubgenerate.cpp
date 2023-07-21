#include "epubgenerate.h"
#include <QFile>
#include <regex>
#include <QWidget>
#include <QTextStream>
#include <QDebug>
#include <QBuffer>
#include <QByteArray>
#include <QDir>
#include <QApplication>
#include <QUuid>
#include <QDateTime>
#include "zip.h"

//#include <QLatin1Char>

EpubGenerate::EpubGenerate()
{

}

void EpubGenerate::getContext(QStringList filePath)
{
    if(filePath.empty()){
        return;
    }
    EpubfilePath=filePath;
    for(QString i:filePath){
        QFile file(i);
        if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qDebug()<<"无法打开文件:"<<i;
            return;
        }
        std::smatch results;
        std::string str=i.toStdString();
        std::string pattern = "/([^/]+).txt$";
        std::regex re(pattern);
        QString title;
        if(std::regex_search(str, results, re)){
            title=QString::fromStdString(results[1].str());//将文件名设置为chaptertitle
            chapterTitle.append(title);//将文件名存入chaptertitle表中
        }

        QString context;
        context="<?xml version=\"1.0\" encoding=\"utf-8\"?>\n<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//\
                  EN\"\n\"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">\n<html xmlns=\"http://www.w3.org/1999/xhtml\">\
                  <head><title>";
        context+=title;
        context+="<link href=\"../Styles/Style0001.css\" type=\"text/css\" rel=\"stylesheet\"/>";
        context+="</title></head><body>";
        context+="<h1 class=\"title\">"+title+"</h1>";
        while(!file.atEnd())
        {
            QTextStream in(&file);
            in.setCodec("utf-8");
            QString line = in.readLine();
            context+="<p>"+line+"</p>";
        }
        context+="</body></html>";
        contextlist.append(context);
        file.close();
    }
    //qDebug()<<contextlist;
    //qDebug()<<chapterTitle;
}

void EpubGenerate::generate(QString author,QString BookName,QStringList filePath,QString outDir)
{
//    initDir(outDir,BookName);
    getContext(filePath);
    reNameSameChapterTitle();
    setDate();
    setUuid();
    setContentopf(author,BookName);
    setTocncx(BookName);
    LoadSrc();
    Zip Z2Epub;
    Z2Epub.addfile("META-INF/container.xml",containerXML);//这行代码必须在添加时放在第一个
    Z2Epub.addfile("mimetype",mimetype);
    Z2Epub.addfile("OEBPS/Styles/Style0001.css",cssFile);
    Z2Epub.addfile("OEBPS/content.opf",Content_Opf);
    Z2Epub.addfile("OEBPS/toc.ncx",Toc_Ncx);
    for(int i=0;i<chapterTitle.size();i++){
        QString Section=QString("Section%1").arg(i,8,10,QLatin1Char('0'));
        Z2Epub.addfile("OEBPS/Text/"+Section+".xhtml",contextlist[i]);
    }
    Z2Epub.zipfile(outDir+"/"+BookName+".epub");
    //outPutFile(outDir,BookName);
    /*
    for(int i=0;i<chapterTitle.size();i++){
        QString outpath=outDir+"/"+chapterTitle[i]+".txt";
        QString outzip=outDir+"/"+chapterTitle[i]+".zip";
        //qDebug()<<outpath;
        QFile file(outpath);
        file.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream out(&file);
        out.setCodec("utf-8");
        out << contextlist[i];
        file.close();
        //int a=compress();
    }
    */
}

void EpubGenerate::mk_dir(QString dirPath)
{
    /*
    int n = _access(path.c_str(), 0);
    if(n==-1){
        _mkdir(path.c_str());
    }
    */
    //CreateDirectory(path.c_str(), NULL );
    QString path=dirPath;
    QDir dir;
    if(!dir.exists(path))
    {
        dir.mkpath(path);
    }
}

void EpubGenerate::initDir(QString outDir,QString BookName)
{
    QString path=outDir+"/"+BookName;
    //qDebug()<<path;
    mk_dir(path);
    mk_dir(path+"/OEBPS");
    mk_dir(path+"/META-INF");
    mk_dir(path+"/OEBPS/Text");
    mk_dir(path+"/OEBPS/Styles");
}

void EpubGenerate::LoadSrc()
{
    QString BasePath=QApplication::applicationDirPath()+"/source/";
    //qDebug()<<BasePath;
    QFile file(BasePath+"container.xml");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        containerXML = file.readAll();
    }
    file.close();
    file.setFileName(BasePath+"mimetype");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        mimetype = file.readAll();
    }
    file.close();
    file.setFileName(BasePath+"Style0001.css");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        cssFile = file.readAll();
    }
    file.close();
    //qDebug()<<containerXML;
    //qDebug()<<mimetype;
    //qDebug()<<cssFile;
}

void EpubGenerate::setContentopf(QString author, QString BookName)
{
    QString contentopf="";
    contentopf+="<?xml version=\"1.0\" encoding=\"utf-8\"?>\
<package version=\"2.0\" unique-identifier=\"BookId\" xmlns=\"http://www.idpf.org/2007/opf\">\
<metadata xmlns:dc=\"http://purl.org/dc/elements/1.1/\" xmlns:opf=\"http://www.idpf.org/2007/opf\">";
    contentopf+=QString("<dc:creator opf:role=\"aut\">%1</dc:creator>").arg(author);
    contentopf+=QString("<dc:language>en</dc:language>");
    contentopf+=QString("<dc:title>%1</dc:title>").arg(BookName);
    contentopf+=QString("<meta content=\"1.9.20\" name=\"Sigil version\"/>\
<dc:date opf:event=\"modification\" xmlns:opf=\"http://www.idpf.org/2007/opf\">%1</dc:date>\
<dc:identifier id=\"BookId\" opf:scheme=\"UUID\">urn:uuid:%2</dc:identifier>\
</metadata><manifest><item id=\"ncx\" href=\"toc.ncx\" media-type=\"application/x-dtbncx+xml\"/>").arg(date,Buuid);
    for(int i=0;i<chapterTitle.size();i++){
        QString Section=QString("Section%1").arg(i,8,10,QLatin1Char('0'));
        contentopf+=QString("<item id=\"x%1.xhtml\" href=\"Text/%1.xhtml\" media-type=\"application/xhtml+xml\"/>").arg(Section);
    }
    contentopf+=QString("<item id=\"Style0001.css\" href=\"Styles/Style0001.css\" media-type=\"text/css\"/></manifest><spine toc=\"ncx\">");
    for(int i=0;i<chapterTitle.size();i++){
        QString Section=QString("Section%1").arg(i,8,10,QLatin1Char('0'));
        contentopf+=QString("<itemref idref=\"x%1.xhtml\"/>").arg(Section);
    }
    contentopf+=QString("</spine></package>");
    Content_Opf=contentopf;
    //qDebug()<<Content_Opf;
}

void EpubGenerate::setDate()
{
    QDateTime dateTime= QDateTime::currentDateTime();
    date = dateTime.toString("yyyy-MM-dd");
}

void EpubGenerate::setUuid()
{
    QUuid uuid=QUuid::createUuid();
    Buuid = uuid.toString().remove("{").remove("}");
}

void EpubGenerate::setTocncx(QString BookName)
{
    QString tocncx="";
    tocncx+=QString("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
<!DOCTYPE ncx PUBLIC \"-//NISO//DTD ncx 2005-1//EN\"\
\"http://www.daisy.org/z3986/2005/ncx-2005-1.dtd\">\
<ncx xmlns=\"http://www.daisy.org/z3986/2005/ncx/\" version=\"2005-1\"><head>\
<meta name=\"dtb:uid\" content=\"urn:uuid:%1\"/>\
<meta name=\"dtb:depth\" content=\"1\"/>\
<meta name=\"dtb:totalPageCount\" content=\"0\"/>\
<meta name=\"dtb:maxPageNumber\" content=\"0\"/></head><docTitle>\
<text>%2</text></docTitle><navMap>").arg(Buuid,BookName);
    for(int i=0;i<chapterTitle.size();i++){
        QString Section=QString("Section%1").arg(i,8,10,QLatin1Char('0'));
        tocncx+=QString("<navPoint id=\"navPoint-%1\" playOrder=\"%1\"><navLabel><text>%2</text>\
</navLabel><content src=\"Text/%3.xhtml\"/></navPoint>").arg(QString::number(i+1),chapterTitle[i],Section);
    }
    tocncx+=QString("</navMap></ncx>");
    Toc_Ncx=tocncx;
    //qDebug()<<Toc_Ncx;
}

void EpubGenerate::reNameSameChapterTitle()
{
    QStringList distin;
    for(int i = 0; i < chapterTitle.length(); i++){
        if(!distin.contains(chapterTitle[i])){
            distin.append(chapterTitle[i]);
        }
        else{
            chapterTitle[i]+="1";
            while(distin.contains(chapterTitle[i])){
                chapterTitle[i]+="1";
            }
            distin.append(chapterTitle[i]);
        }
    }
}

//void EpubGenerate::outPutFile(QString outDir,QString BookName)
//{
//    QString BaseOutath=outDir+"/"+BookName;
//    QFile file;
//    for(int i=0;i<chapterTitle.size();i++){
//        //qDebug()<<outpath;
//        QString Section=QString("Section%1").arg(i,8,10,QLatin1Char('0'));
//        file.setFileName(BaseOutath+"/OEBPS/Text/"+Section+".xhtml");
//        if(file.open(QIODevice::WriteOnly | QIODevice::Text)){
//            QTextStream out1(&file);
//            out1.setCodec("utf-8");
//            out1 << contextlist[i];
//        }
//        file.close();
//    }

//    file.setFileName(BaseOutath+"/OEBPS/Styles/Style0001.css");
//    if(file.open(QIODevice::WriteOnly | QIODevice::Text)){
//        QTextStream out2(&file);
//        out2.setCodec("utf-8");
//        out2 << cssFile;
//    }
//    file.close();

//    file.setFileName(BaseOutath+"/META-INF/container.xml");
//    if(file.open(QIODevice::WriteOnly | QIODevice::Text)){
//        QTextStream out3(&file);
//        out3.setCodec("utf-8");
//        out3 << containerXML;
//    }
//    file.close();

//    file.setFileName(BaseOutath+"/mimetype");
//    if(file.open(QIODevice::WriteOnly | QIODevice::Text)){
//        QTextStream out4(&file);
//        out4.setCodec("utf-8");
//        out4 << mimetype;
//    }
//    file.close();

//    file.setFileName(BaseOutath+"/OEBPS/content.opf");
//    if(file.open(QIODevice::WriteOnly | QIODevice::Text)){
//        QTextStream out5(&file);
//        out5.setCodec("utf-8");
//        out5 << Content_Opf;
//    }
//    file.close();

//    file.setFileName(BaseOutath+"/OEBPS/toc.ncx");
//    if(file.open(QIODevice::WriteOnly | QIODevice::Text)){
//        QTextStream out6(&file);
//        out6.setCodec("utf-8");
//        out6 << Toc_Ncx;
//    }
//    file.close();
//}

//void EpubGenerate::Zip2Epub(QString outDir, QString BookName)
//{

//}

