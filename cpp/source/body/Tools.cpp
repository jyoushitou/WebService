#include "../header/Tools.h"

namespace Tools{
    void Out_System(std::string str){
        std::cout<<"[输出]"<<str<<std::endl;
    }
    
    void Out_System_Mysql(std::string str){
        std::cout<<"[MySQL]"<<str<<std::endl;
    }

    void Out_System_Http(std::string str){
        std::cout<<"[HTTP]"<<str<<std::endl;
    }

    void Out_System_Error(std::string str){
        std::cerr<<"[错误]"<<str<<std::endl;
    }

    int Input_System_int(){
        int a;
        std::cin>>a;
        return a;
    }

    std::string Input_System_string(){
        std::string a;
        std::cin>>a;
        return a;
    }

    std::string Read_File(const std::string& path) {
        std::ifstream file(path, std::ios::in | std::ios::binary);  // 以二进制模式打开文件
        if (!file) return "";                                        // 打开失败返回空字符串
        std::ostringstream ss;                                       // 字符串输出流
        ss << file.rdbuf();                                          // 将文件内容读入流
        return ss.str();                                             // 返回字符串
    }

    std::string Get_MimeType(const std::string& ext) {
        if (ext == ".html" || ext == ".htm") return MIME_HTML;  // HTML 文件
        if (ext == ".js")   return MIME_JS;                     // JavaScript 文件
        if (ext == ".css")  return MIME_CSS;                    // CSS 文件
        if (ext == ".json") return MIME_JSON;                   // JSON 文件
        return "application/octet-stream";                      // 其他类型（二进制流）
    }
}