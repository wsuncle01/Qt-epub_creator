#ifndef ZIP_HELPER_H
#define ZIP_HELPER_H

#include <map>
#include <string>

//1.暂时不支持子目录
//注意: 因为使用了zlib库,使用时加上预编译宏 ZLIB_WINAPI
class ZipHelper
{
public:
    ZipHelper(){}
    ~ZipHelper(){}

    //path: utf8 path
    ZipHelper& AddFile(const char* input_path,const char* inzip_path = "");
    ZipHelper& AddDir(const char* input_dir,const char* temp_dir = NULL);

    //output_path :utf8 path
    bool ToZip(const char* output_path);

private:
    std::map<std::string,std::string> files_;
};
————————————————
    版权声明：本文为CSDN博主「infoworld」的原创文章，遵循CC 4.0 BY-SA版权协议，转载请附上原文出处链接及本声明。
    原文链接：https://blog.csdn.net/infoworld/article/details/60480313
#endif // ZIP_HELPER_H
