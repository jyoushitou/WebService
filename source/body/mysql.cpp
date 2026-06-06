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
        Tools::Out_System("数据库 " + database + " 已就绪");
        if(database!=str){
            database=str;
            Tools::Out_System("数据库"+database+"已经设置为默认数据库");
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
        Tools::Out_System("成功连接到 MySQL 服务器！");
        return 0;
    }

    bool mysql::Set_databases(std::string str){
        if (mysql_select_db(conn, str.c_str())) {
            Tools::Out_System_Error("选择数据库失败: " + std::string(mysql_error(conn)));
            return 1;
        }
        database=str;
        return 0;
    }

    bool mysql::Set_UTF8(){
        if (mysql_set_character_set(conn, "utf8mb4") != 0) {
            Tools::Out_System_Error("设置字符集失败: " + std::string(mysql_error(conn)));
            return 1;
        }
        Tools::Out_System("字符集已设置为 utf8mb4");
        return 0;
    }

    bool mysql::User(const std::string name , const std::string user_password){
        std::string sql="select name,password from users where name = '" + name + "';";
        if (mysql_query(conn, sql.c_str())) {
            Tools::Out_System_Error("查询失败: " +std::string(mysql_error(conn)));
            return 0;
        }
        MYSQL_RES* result = mysql_store_result(conn);

        if(!result){
            Tools::Out_System_Error("获取结果失败: " + std::string(mysql_error(conn)));
            return 0;
        }

        MYSQL_ROW row = mysql_fetch_row(result);
        if(!row){
            Tools::Out_System("未找到用户");
            return 0;
        }
        std::string db_name = row[0]?row[0]:"";
        std::string db_password = row[1]?row[1]:"";

        if(db_password == user_password){
            Tools::Out_System("欢迎用户" + name);
            mysql_free_result(result);
            return 1;
        }
        else{
            Tools::Out_System("密码错误");
            mysql_free_result(result);
            return 0;
        }
    }

    bool mysql::Close(){
        mysql_close(conn);
        Tools::Out_System("连接已关闭");
        return 0;
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