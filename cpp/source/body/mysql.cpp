#include "../header/MyMySQL.h"

namespace MySQL{
    mysql::mysql(){
        conn = mysql_init(nullptr);
        database="web_server";
        if(Connect()||Create_Socket()){
            throw std::invalid_argument("key error");
        }
        Set_databases(database); 
    }

    mysql::mysql(std::string str):conn (mysql_init(nullptr)){
        conn = mysql_init(nullptr);
        Set_databases(str);
        if(Create_Socket()||Connect()){
            throw std::invalid_argument("key error");
        }
    }

    bool mysql::Create_Socket(){
        if (conn == nullptr) {
            Tools::Out_System_Error("mysql_init() 失败");
            return 1;
        }
        return 0;
    }

    bool mysql::Create_DataBases(){
       return Create_DataBases(database);
    }

    bool mysql::Create_DataBases(std::string str){
        std::string sql = "CREATE DATABASE IF NOT EXISTS " + str;
        if (mysql_query(conn, sql.c_str())) {
            Tools::Out_System_Error("创建数据库失败: " + std::string(mysql_error(conn)));
            return 1;
        }
        Tools::Out_System_Mysql("数据库 " + database + " 已就绪");
        if(database!=str){
            database=str;
            Tools::Out_System_Mysql("数据库"+database+"已经设置为默认数据库");
        }
        return 0;
    }

    bool mysql::Connect(){
        if (mysql_real_connect(conn, host.c_str(), 
        user.c_str(), password.c_str(), 
        nullptr, port, nullptr, 0) == nullptr) {
            Tools::Out_System_Error("mysql_real_connect() 失败: "+std::string(mysql_error(conn)));
            mysql_close(conn);
            return 1;
        }
        Tools::Out_System_Mysql("成功连接到 MySQL 服务器！");
        return 0;
    }

        bool mysql::Set_databases(std::string str){
        if (mysql_select_db(conn, str.c_str())) {
            Tools::Out_System_Error("选择数据库失败: " + std::string(mysql_error(conn)));
            return 1;
        }
        database=str;
        Tools::Out_System_Mysql("已设置数据库: " + database);
        return 0;
    }

    bool mysql::Set_UTF8(){
        if (mysql_set_character_set(conn, "utf8mb4") != 0) {
            Tools::Out_System_Error("设置字符集失败: " + std::string(mysql_error(conn)));
            return 1;
        }
        Tools::Out_System_Mysql("字符集已设置为 utf8mb4");
        return 0;
    }

    int mysql::User(const std::string name , const std::string user_password){
        // 查当前数据库
        mysql_query(conn, "SELECT DATABASE();");
        MYSQL_RES* dbRes = mysql_store_result(conn);
        if (dbRes) {
            MYSQL_ROW dbRow = mysql_fetch_row(dbRes);
            std::cout << "[MySQL Debug] Current database: " << (dbRow ? dbRow[0] : "NULL") << std::endl;
            mysql_free_result(dbRes);
        }

        // 查询用户信息，获取密码和权限
        std::string sql="select name, password, permission from users where name = '" + name + "';";
        std::cout << "[MySQL Debug] SQL: " << sql << std::endl;
        if (mysql_query(conn, sql.c_str())) {
            Tools::Out_System_Error("查询失败: " +std::string(mysql_error(conn)));
            return -1;    // 数据库错误
        }
        MYSQL_RES* result = mysql_store_result(conn);

        if(!result){
            Tools::Out_System_Error("获取结果失败: " + std::string(mysql_error(conn)));
            return -1;    // 数据库错误
        }

        MYSQL_ROW row = mysql_fetch_row(result);
        if(!row){
            Tools::Out_System_Mysql("未找到用户");
            std::cout << "[MySQL Debug] mysql_fetch_row returned NULL, mysql_error: " 
                      << (mysql_error(conn) ? mysql_error(conn) : "none")
                      << ", mysql_errno: " << mysql_errno(conn) << std::endl;
            mysql_free_result(result);
            return 0;     // 用户不存在
        }

        std::string db_password = row[1] ? row[1] : "";
        int db_permission = 0;
        if (row[2]) {
            db_permission = std::stoi(row[2]);
        }

        if (db_password == user_password) {
            Tools::Out_System_Mysql("欢迎用户 " + name + "，权限: " + std::to_string(db_permission));
            mysql_free_result(result);
            return db_permission;   // 返回权限值
        } else {
            Tools::Out_System_Mysql("密码错误");
            mysql_free_result(result);
            return 0;     // 密码错误
        }
    }

    bool mysql::Close(){
        mysql_close(conn);
        Tools::Out_System_Mysql("连接已关闭");
        return 0;
    }

    int mysql::Get_UserId(std::string name){
        std::string sql="select id from users where name = '" + name + "';";
        if (mysql_query(conn, sql.c_str())) {
            Tools::Out_System_Error("查询用户ID失败: " +std::string(mysql_error(conn)));
            return -1;
        }
        MYSQL_RES* result = mysql_store_result(conn);
        if(!result){
            Tools::Out_System_Error("获取结果失败: " + std::string(mysql_error(conn)));
            return -1;
        }
        MYSQL_ROW row = mysql_fetch_row(result);
        if(!row){
            Tools::Out_System_Mysql("未找到用户");
            mysql_free_result(result);
            return 0;
        }
        int user_id = row[0] ? std::stoi(row[0]) : 0;
        mysql_free_result(result);
        return user_id;
    }

    mysql::~mysql(){
        Close();
    }

    bool Create_DataBases(MYSQL* conn,std::string str){
        std::string sql = "CREATE DATABASE IF NOT EXISTS " + str;
        if (mysql_query(conn, sql.c_str())) {
            Tools::Out_System_Error("创建数据库失败: " + std::string(mysql_error(conn)));
        return 1;
        }
        return 0;
    }  
}