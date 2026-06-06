cmake_minimum_required(VERSION 4.0)

project(WebServer VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 解决 MSVC 中文编码问题
if(WIN32)
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
endif()

# MySQL Server 安装路径
set(MYSQL_DIR "C:/Program Files/MySQL/MySQL Server 8.0")

message(STATUS "MySQL Server Dir: ${MYSQL_DIR}")

# 包含 MySQL 头文件目录
include_directories("${MYSQL_DIR}/include" "source/header")

# 创建可执行文件
add_executable(${PROJECT_NAME}
    source/body/main.cpp 
    source/body/mysql.cpp
    source/body/Tools.cpp
)

# 链接 MySQL 客户端库
target_link_libraries(${PROJECT_NAME} PRIVATE
    "${MYSQL_DIR}/lib/libmysql.lib"
)

# Windows 下链接必要的系统库
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        ws2_32
    )
endif()

# ========== 复制 MySQL DLL 到输出目录 ==========
# 需要复制的 MySQL 动态库列表（可根据实际需要增删）
set(MYSQL_DLLS
    libmysql.dll
)

# 为每个 DLL 添加自定义命令，在构建后复制到目标输出目录
foreach(DLL_FILE ${MYSQL_DLLS})
    set(SOURCE_DLL "${MYSQL_DIR}/lib/${DLL_FILE}")
    set(DEST_DLL   "$<TARGET_FILE_DIR:${PROJECT_NAME}>/${DLL_FILE}")

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${SOURCE_DLL}"
            "${DEST_DLL}"
        COMMENT "Copying ${DLL_FILE} to output directory..."
    )
endforeach()