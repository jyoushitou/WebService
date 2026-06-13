#ifndef MYMYSQL_H
#define MYMYSQL_H
#include <iostream>
#include <string>        // C++ 字符串类

#include "Tools.h"       //导入工具库

#include <mysql.h>       // MySQL C API 头文件


namespace MySQL{
    bool Create_DataBases(MYSQL* ,std::string);                 //验证传入的是否有数据库没有的就创建                                  

    class mysql{
        public:
        mysql();                                                //构建默认函数(web_socket)
        mysql(std::string str);                                 //指定数据库

        bool Create_DataBases();                                //验证默认操作的数据库是否有数据库没有的就创建
        bool Create_DataBases(std::string);                     //验证传入的是否有数据库没有的就创建并修改为默认数据库
        
        int User(std::string,std::string);                     //鉴权，返回权限级别
        int Get_UserId(std::string);                            //根据用户名获取用户id

        ~mysql();
        private:
        //MySQL的参数
        const std::string host = "localhost";                   // 数据库服务器地址
        const int port = 3306;                                  // MySQL 默认端口
        const std::string user = "web_server";                        // 数据库用户名
        const std::string password = "123456";                  // 数据库密码
        std::string database;                                   // 数据库名称
        
        MYSQL* conn ;                                           // 创建句柄
        
        bool Create_Socket();                                   // 验证句柄是否创建成功
        bool Connect();                                         // 验证是否连接成功
        bool Close();                                           //关闭连接

        bool Set_UTF8();                                        //检验字符串格式为utf8
        bool Set_databases(std::string);                        //设置默认的数据库
    
    };              
}

#endif //!MYMYSQL_H