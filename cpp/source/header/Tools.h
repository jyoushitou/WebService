#include <iostream>
#include <fstream>                                  // 文件输入输出流
#include <sstream>                                  // 字符串流
#include <string>                                   // 字符串类

#define MIME_HTML "text/html"              // HTML 的 MIME 类型
#define MIME_JS  "application/javascript"  // JavaScript 的 MIME 类型
#define MIME_CSS "text/css"                // CSS 的 MIME 类型
#define MIME_JSON "application/json"       // JSON 的 MIME 类型


namespace Tools{
    void Out_System(std::string);

    void Out_System_Mysql(std::string);                 //mysql输出

    void Out_System_Http(std::string);                  //http输出

    void Out_System_Error(std::string);                 //输出错误

    int Input_System_int();                             //输入int

    std::string Input_System_string();                  //输入string
    
    std::string Read_File(const std::string& path);     //读取文件

    std::string Get_MimeType(const std::string& ext);   //获取文件格式
}