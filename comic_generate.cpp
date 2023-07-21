#include "comic_generate.h"
#include "comic_creator.h"
#include <QImage>
#include <QDate>
#include <QUuid>
#include <regex>
#include <QDebug>
#include <QApplication>
#include "zip.h"
#include <QBuffer>

Comic_generate::Comic_generate()
{

}

void Comic_generate::setDate()
{
    QDateTime dateTime= QDateTime::currentDateTime();
    date = dateTime.toString("yyyy-MM-dd");
}

void Comic_generate::setUuid()
{
    QUuid uuid=QUuid::createUuid();
    Buuid = uuid.toString().remove("{").remove("}");
}

void Comic_generate::LoadSrc()
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
}

void Comic_generate::setTocncx(QString ComicName)
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
<text>%2</text></docTitle><navMap>").arg(Buuid,ComicName);
    for(int i=0;i<ChapterNameList.size();i++){
       QString Section=QString("Section%1").arg(i,8,10,QLatin1Char('0'));
       tocncx+=QString("<navPoint id=\"navPoint-%1\" playOrder=\"%1\"><navLabel><text>%2</text>\
                             </navLabel><content src=\"Text/%3.xhtml\"/></navPoint>").arg(QString::number(i+1),ChapterNameList[i],Section);
    }
    tocncx+=QString("</navMap></ncx>");
    Toc_Ncx=tocncx;

}



void Comic_generate::getContext(QList<chapterNode> filePath, QStringList picList)
{
    for(int i=0;i<filePath.size();i++){
        ChapterNameList.append(filePath[i].chapterName);
        QString context="";
        context+="<?xml version=\"1.0\" encoding=\"utf-8\"?>\n<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//\
EN\"\n\"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">\n<html xmlns=\"http://www.w3.org/1999/xhtml\">\
<head><title>";
        context+=filePath[i].chapterName;
        context+="<link href=\"../Styles/Style0001.css\" type=\"text/css\" rel=\"stylesheet\"/>";
        context+="</title></head><body><div class=\"image\">";
        for(int j=0;j<filePath[i].dataList.size();j++){
            std::smatch results;
            std::string str=filePath[i].dataList[j].toStdString();
            std::string pattern = "/([^/]+)$";
            std::regex re(pattern);
            QString title;
            if(std::regex_search(str, results, re)){
                QString name;
                name=QString::fromStdString(results[1].str());
                fileNameList.append(name);
                filePathList.append(filePath[i].dataList[j]);
                context+="<img alt=\""+name.split(".")[0]+"\" src=\"../Images/"+name+"\"/>";
            }
        }
        context+="</div></body></html>";
        contextList.append(context);
    }
    ChapterNameList.append("Unnamed Chapter");
    QString context="";
    context+="<?xml version=\"1.0\" encoding=\"utf-8\"?>\n<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//\
               EN\"\n\"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">\n<html xmlns=\"http://www.w3.org/1999/xhtml\">\
               <head><title>";
    context+="Unnamed Chapter";
    context+="<link href=\"../Styles/Style0001.css\" type=\"text/css\" rel=\"stylesheet\"/>";
    context+="</title></head><body><div class=\"image\">";
    for(int i=0;i<picList.size();i++){
        std::smatch results;
        std::string str=picList[i].toStdString();
        std::string pattern = "/([^/]+)$";
        std::regex re(pattern);
        QString title;
        if(std::regex_search(str, results, re)){
            QString name;
            name=QString::fromStdString(results[1].str());
            fileNameList.append(name);
            filePathList.append(picList[i]);
            context+="<img alt=\""+name.split(".")[0]+"\" src=\"../Images/"+name+"\"/>";
        }
    }
    context+="</div></body></html>";
    contextList.append(context);
//    QFile file("C:/Users/黄辙/Desktop/1111/eeee.txt");
//    if (!file.open(QIODevice::WriteOnly|QIODevice::Append)) {
//        qDebug() << "Cannot open file for writing: "
//                 << qPrintable(file.errorString());
//        return;
//    }
//    QByteArray outfileData;
//    QDataStream out(&file);
//    for(int i=0;i<contextList.size();i++){
//        QByteArray outfileData=QByteArray::fromRawData(contextList[i].toUtf8(),contextList[i].toUtf8().size());
//        out.writeRawData(outfileData,outfileData.end()-outfileData.begin());
//    }
    //    file.close();
}

void Comic_generate::setContentopf(QString author, QString ComicName)
{
    QString contentopf="";
    contentopf+="<?xml version=\"1.0\" encoding=\"utf-8\"?>\
<package version=\"2.0\" unique-identifier=\"BookId\" xmlns=\"http://www.idpf.org/2007/opf\">\
<metadata xmlns:dc=\"http://purl.org/dc/elements/1.1/\" xmlns:opf=\"http://www.idpf.org/2007/opf\">";
    contentopf+=QString("<dc:creator opf:role=\"aut\">%1</dc:creator>").arg(author);
    contentopf+=QString("<dc:language>en</dc:language>");
    contentopf+=QString("<dc:title>%1</dc:title>").arg(ComicName);
    contentopf+=QString("<meta content=\"1.9.20\" name=\"Sigil version\"/>\
<dc:date opf:event=\"modification\" xmlns:opf=\"http://www.idpf.org/2007/opf\">%1</dc:date>\
<dc:identifier id=\"BookId\" opf:scheme=\"UUID\">urn:uuid:%2</dc:identifier>\
</metadata><manifest><item id=\"ncx\" href=\"toc.ncx\" media-type=\"application/x-dtbncx+xml\"/>").arg(date,Buuid);
    for(int i=0;i<ChapterNameList.size();i++){
        QString Section=QString("Section%1").arg(i,8,10,QLatin1Char('0'));
        contentopf+=QString("<item id=\"x%1.xhtml\" href=\"Text/%1.xhtml\" media-type=\"application/xhtml+xml\"/>").arg(Section);
    }
    for(int i=0;i<fileNameList.size();i++){
        contentopf+=QString("<item id=\"x%1\" href=\"Images/%1\" media-type=\"image/png\"/>").arg(fileNameList[i]);
    }
    contentopf+=QString("<item id=\"Style0001.css\" href=\"Styles/Style0001.css\" media-type=\"text/css\"/></manifest><spine toc=\"ncx\">");
    for(int i=0;i<ChapterNameList.size();i++){
        QString Section=QString("Section%1").arg(i,8,10,QLatin1Char('0'));
        contentopf+=QString("<itemref idref=\"x%1.xhtml\"/>").arg(Section);
    }
//    for(int i=0;i<fileNameList.size();i++){
//        contentopf+=QString("<itemref idref=\"x%1\"/>").arg(fileNameList[i]);
//    }
    contentopf+=QString("</spine></package>");
    Content_Opf=contentopf;
}

void Comic_generate::reNameSameChapterTitle()
{
    QStringList distin;
    for(int i = 0; i < ChapterNameList.size(); i++){
        if(!distin.contains(ChapterNameList[i])){
            distin.append(ChapterNameList[i]);
        }
        else{
            ChapterNameList[i]+="1";
            while(distin.contains(ChapterNameList[i])){
                ChapterNameList[i]+="1";
            }
            distin.append(ChapterNameList[i]);
        }
    }
}

void Comic_generate::generate(QList<chapterNode> filePath, QStringList picList, QString author, QString ComicName, QString outDir)
{
    getContext(filePath,picList);
    reNameSameChapterTitle();
    setDate();
    setUuid();
    setContentopf(author,ComicName);
    setTocncx(ComicName);
    LoadSrc();
    Zip Z2Epub;
    Z2Epub.addfile("META-INF/container.xml",containerXML);//这行代码必须在添加时放在第一个
    Z2Epub.addfile("mimetype",mimetype);
    Z2Epub.addfile("OEBPS/Styles/Style0001.css",cssFile);
    Z2Epub.addfile("OEBPS/content.opf",Content_Opf);
    Z2Epub.addfile("OEBPS/toc.ncx",Toc_Ncx);
    for(int i=0;i<ChapterNameList.size();i++){
        QString Section=QString("Section%1").arg(i,8,10,QLatin1Char('0'));
        Z2Epub.addfile("OEBPS/Text/"+Section+".xhtml",contextList[i]);
    }
    for(int i=0;i<filePathList.size();i++){
        QImage image(filePathList[i]);
        QByteArray arr;
        QBuffer buffer(&arr);
        buffer.open(QIODevice::WriteOnly);
        QByteArray format=fileNameList[i].split(".")[fileNameList[i].split(".").size()-1].toUtf8();
        image.save(&buffer,format.data());
        Z2Epub.addfile("OEBPS/Images/"+fileNameList[i],buffer.data());
    }
    Z2Epub.zipfile(outDir+"/"+ComicName+".epub");
}


