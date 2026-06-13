cmake_minimum_required(VERSION 3.10)

project(WebServer VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 启用线程支持
find_package(Threads REQUIRED)

# ==================== MySQL 配置 ====================
# Windows 下使用 MySQL Connector/C（libmysql）
# Linux 下使用系统的 libmysqlclient
if(WIN32)
    set(MYSQL_DIR "C:/Program Files/MySQL/MySQL Server 8.0" CACHE PATH "MySQL install dir")
    include_directories("${MYSQL_DIR}/include")
    set(MYSQL_LIB "${MYSQL_DIR}/lib/libmysql.lib")
    set(MYSQL_DLL "${MYSQL_DIR}/lib/libmysql.dll")
else()
    # Linux：使用 pkg-config 查找 MySQL
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_check_modules(MYSQL QUIET mysqlclient)
    endif()
    
    if(NOT MYSQL_FOUND)
        # 手动查找常见路径
        find_path(MYSQL_INCLUDE_DIR
            NAMES mysql.h
            PATHS /usr/include/mysql /usr/local/include/mysql
        )
        find_library(MYSQL_LIBRARY
            NAMES mysqlclient libmysqlclient
            PATHS /usr/lib /usr/lib/x86_64-linux-gnu /usr/local/lib
        )
        if(MYSQL_INCLUDE_DIR AND MYSQL_LIBRARY)
            set(MYSQL_INCLUDE_DIRS "${MYSQL_INCLUDE_DIR}")
            set(MYSQL_LIBRARIES "${MYSQL_LIBRARY}")
            set(MYSQL_FOUND TRUE)
        endif()
    else()
        set(MYSQL_INCLUDE_DIRS "${MYSQL_INCLUDE_DIRS}")
        set(MYSQL_LIBRARIES "${MYSQL_LIBRARIES}")
    endif()
    
    if(NOT MYSQL_FOUND)
        message(WARNING "MySQL client library not found! Build will likely fail.")
        message(WARNING "Install with: sudo apt install libmysqlclient-dev")
    endif()
    
    include_directories(${MYSQL_INCLUDE_DIRS})
endif()

# 头文件目录
include_directories("source/header")

# 创建可执行文件
add_executable(${PROJECT_NAME}
    source/body/main.cpp 
    source/body/FrameWork.cpp
    source/body/mysql.cpp
    source/body/Tools.cpp
    source/body/https_api.cpp
)

# 链接线程库
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)

# ==================== 平台相关链接 ====================
if(WIN32)
    # Windows：链接 MySQL lib + winsock
    target_link_libraries(${PROJECT_NAME} PRIVATE "${MYSQL_LIB}" ws2_32)
    
    # 复制 MySQL DLL 到输出目录
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${MYSQL_DLL}"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/"
        COMMENT "Copying libmysql.dll to output directory..."
    )
else()
    # Linux：链接 MySQL 客户端库
    if(MYSQL_FOUND)
        target_link_libraries(${PROJECT_NAME} PRIVATE ${MYSQL_LIBRARIES})
    endif()
    
    # Linux 下链接 pthread（有时需要显式链接）
    target_link_libraries(${PROJECT_NAME} PRIVATE pthread)
endif()
