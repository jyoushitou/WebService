#include <iostream>
#include <string>
#include <atomic>
#include <thread>
#include <chrono>

#ifdef _WIN32
// winsock2.h 必须在 windows.h 之前包含
#include <winsock2.h>
#include <windows.h>
#endif

#include "Tools.h"
#include "MyMySQL.h"
#include "https_api.h"

// 声明全局变量（extern，不定义）
extern Http::http_server* g_server;   // 全局 HTTP 服务器指针，在 FrameWork.cpp 中定义
extern std::atomic<bool> g_running;  // 全局原子性运行状态标志，在 FrameWork.cpp 中定义

// 声明外部函数
extern void Initiave_Http(int Port);                                          // 初始化并启动 HTTP 服务器（定义在 FrameWork.cpp）
extern MySQL::mysql* Initiave_MySQL();                                        // 初始化 MySQL 数据库连接（定义在 FrameWork.cpp）


int main(){
    // 设置控制台编码为 UTF-8，解决中文乱码问题
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);            // Windows 下将控制台输出代码页设为 UTF-8，确保中文正常显示
#endif
    Tools::Out_System("Web_Server启动");    // 输出系统启动日志信息
    
        // 初始化 MySQL
    MySQL::mysql* web_db = Initiave_MySQL();    // 创建 MySQL 数据库连接对象，若失败返回 nullptr
    
    // 启动 HTTP 服务器（子进程/子线程中运行，不阻塞主进程）
    Initiave_Http(60906);
    
    // 主进程保持存活，等待程序结束
    Tools::Out_System("主进程正在运行，HTTP 服务器在子进程中运行...");
    
    // 循环保持主进程存活，直到收到退出信号
    while (g_running) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    // 下面的代码在服务器停止后才会执行
    if (web_db) {                           // 检查数据库连接是否成功建立
        std::string name, password;         // 用于接收 User() 方法返回的用户名和密码
        if(web_db->User(name, password)){   // 调用数据库用户验证（具体逻辑取决于 User() 的实现）
            return 0;                       // 验证成功，程序正常退出
        }
        else{
            delete web_db;                  // 释放数据库连接对象
            return 1;                       // 验证失败，程序以非零值退出
        }
    }
    
    delete web_db;  // 释放数据库连接（delete nullptr 是安全的，不会报错）
    return 0;       // 程序正常结束
}