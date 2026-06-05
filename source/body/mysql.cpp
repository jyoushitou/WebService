#include "../header/mysql.h"

namespace MySQL{
    bool MySQL_Create(){
        if (conn == nullptr) {
            std::cerr << "mysql_init() 失败" << std::endl;
            return 0;
        }
        return 1;
    }

    bool MySQL_Coonect(){
        if (mysql_real_connect(conn, host.c_str(), 
        user.c_str(), password.c_str(), 
        nullptr, port, nullptr, 0) == nullptr) {
            std::cerr << "mysql_real_connect() 失败: " << mysql_error(conn) << std::endl;
            mysql_close(conn);
            return 1;
        }
    std::cout << "成功连接到 MySQL 服务器！" << std::endl;

    }
}