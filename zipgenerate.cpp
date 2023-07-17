#include "zipgenerate.h"
#include <sstream>

ZipGenerate::ZipGenerate()
{
    fileName=new QStringList;
    fileData=new QStringList;
    localFile="";
    centralDirFile="";
    EOCD="";
}

