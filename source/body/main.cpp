#include <iostream>
#include <windows.h>
#include <MyMySQL.h>
#include <Tools.h>

int main(){
    // 设置控制台编码为 UTF-8，解决中文乱码问题
    SetConsoleOutputCP(CP_UTF8);
    //SetConsoleInputCP(CP_UTF8);
    Tools::Out_System("Web_Server启动");
    std::string name , password ;
    MySQL::mysql* web_db;
    try{
        web_db = new MySQL::mysql();
        
    }
    catch(const std::exception& e){
        Tools::Out_System_Error(e.what());
    }
    catch(...){
        Tools::Out_System_Error("出错！");
    }
    if(web_db->User(name,password)){
        return 0;
    }
    else{
        return 1;
    }
    return 0;
}