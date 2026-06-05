#include <iostream>
#include <string>        // C++ 字符串类

#include <mysql.h>       // MySQL C API 头文件


namespace MySQL{
    //MySQL的参数
    const std::string host = "localhost";           // 数据库服务器地址
    const int port = 3306;                          // MySQL 默认端口
    const std::string user = "root";                // 数据库用户名
    const std::string password = "123456";          // 数据库密码
    const std::string database = "jxau_shuju2503";  // 要使用的数据库名

    MYSQL* conn = mysql_init(nullptr);  //  MySQL 连接句柄

    bool MySQL_Create();//验证句柄是否创建成功

    bool MySQL_Coonect();
    
int mysql_select(){
    // ========== 2. 连接到 MySQL 服务器 ==========
    // mysql_real_connect() 尝试与运行在主机上的 MySQL 数据库引擎建立连接
    // 参数：连接句柄、主机名、用户名、密码、数据库名(先传nullptr)、端口、Unix socket(传nullptr)、客户端标志(传0)
    
    // 设置连接字符集为 UTF-8（解决中文乱码）
    // utf8mb4 是真正的 UTF-8 编码，支持 4 字节的 Unicode 字符（如 emoji）
    if (mysql_set_character_set(conn, "utf8mb4") != 0) {
        std::cerr << "设置字符集失败: " << mysql_error(conn) << std::endl;
        mysql_close(conn);
        return 1;
    }
    std::cout << "字符集已设置为 utf8mb4" << std::endl;

    // ========== 3. 创建数据库（如果不存在） ==========
    // 使用 CREATE DATABASE IF NOT EXISTS 语句，避免重复创建报错
    std::string sql = "CREATE DATABASE IF NOT EXISTS " + database;
    if (mysql_query(conn, sql.c_str()) != 0) {
        std::cerr << "创建数据库失败: " << mysql_error(conn) << std::endl;
        mysql_close(conn);
        return 1;
    }
    std::cout << "数据库 " << database << " 已就绪" << std::endl;

    // ========== 4. 选择数据库 ==========
    // mysql_select_db() 使指定的数据库成为当前数据库
    if (mysql_select_db(conn, database.c_str()) != 0) {
        std::cerr << "选择数据库失败: " << mysql_error(conn) << std::endl;
        mysql_close(conn);
        return 1;
    }

    // ========== 8. 条件查询 ==========
    // 执行 SELECT 查询，获取 students 表中的所有记录
    sql = "SELECT * from students";
    if (mysql_query(conn, sql.c_str()) != 0) {
        std::cerr << "查询失败: " << mysql_error(conn) << std::endl;
    } else {
        // mysql_store_result() 读取整个查询结果并存储在客户端
        MYSQL_RES* result = mysql_store_result(conn);
        if (result != nullptr) {
            std::cout << "\n学生列表：" << std::endl;
            
            // mysql_fetch_row() 从结果集中获取下一行
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(result)) != nullptr) {
                // row[0] 是 ID 字段，row[1] 是姓名字段，row[2] 是年龄字段
                // 使用三元运算符处理 NULL 值，避免访问空指针
                std::cout << "  ID: " << (row[0] ? row[0] : "NULL")
                          << ", 姓名: " << (row[1] ? (row[1]) : "NULL")
                          << ", 年龄: " << (row[2] ? row[2] : "NULL")
                          << std::endl;
            }
            
            // 释放结果集内存
            mysql_free_result(result);
        }
    }

    // ========== 调试代码：打印姓名字段的原始字节 ==========
    // 这段代码用于检查从数据库读取的中文字符的原始编码字节
    // 注意：这里又调用了一次 mysql_store_result()，但之前的查询结果已经被释放了
    // 所以这段代码实际上不会输出任何内容，因为 result 会是 nullptr
    MYSQL_RES* result = mysql_store_result(conn);
    if (result != nullptr) {
        MYSQL_ROW row = mysql_fetch_row(result);
        if (row != nullptr && row[1] != nullptr) {
            std::cout << "原始字节: ";
            // 以十六进制格式打印姓名字段的每个字节
            for (size_t i = 0; i < strlen(row[1]); i++) {
                printf("%02X ", (unsigned char)row[1][i]);
            }
            std::cout << std::endl;
        }
        mysql_free_result(result);
    }

    // ========== 10. 关闭连接 ==========
    // mysql_close() 关闭与 MySQL 服务器的连接并释放连接句柄
    mysql_close(conn);
    std::cout << "\n连接已关闭" << std::endl;
}
}