cmake_minimum_required(VERSION 3.16)
project(rpc_protos VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找Protobuf
find_package(Protobuf REQUIRED)
if(Protobuf_FOUND)
    message(STATUS "Protobuf version: ${Protobuf_VERSION}")
    message(STATUS "Protobuf include: ${Protobuf_INCLUDE_DIRS}")
    message(STATUS "Protobuf libraries: ${Protobuf_LIBRARIES}")
endif()

# 查找gRPC
find_package(gRPC REQUIRED)
if(gRPC_FOUND)
    message(STATUS "gRPC version: ${gRPC_VERSION}")
    message(STATUS "gRPC include: ${gRPC_INCLUDE_DIRS}")
    message(STATUS "gRPC libraries: ${gRPC_LIBRARIES}")
endif()

# 设置输出目录到build文件夹
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../build/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../build/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../build/bin)

# 设置生成文件输出目录
set(PROTO_GEN_DIR ${CMAKE_SOURCE_DIR}/../build/generated)

# 递归查找所有proto文件
file(GLOB_RECURSE PROTO_FILES
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    "*.proto"
)

message(STATUS "Found proto files: ${PROTO_FILES}")

# 为每个proto文件生成C++和gRPC代码
foreach(PROTO_FILE ${PROTO_FILES})
    get_filename_component(PROTO_DIR ${PROTO_FILE} DIRECTORY)
    get_filename_component(PROTO_NAME ${PROTO_FILE} NAME_WE)
    
    # 生成C++代码
    protobuf_generate(
        TARGET rpc_protos
        LANGUAGE cpp
        PROTOS ${PROTO_FILE}
        PROTOC_OUT_DIR ${PROTO_GEN_DIR}
        DEPENDENCIES ${PROTO_FILE}
    )
    
    # 生成gRPC代码
    protobuf_generate(
        TARGET rpc_protos
        LANGUAGE grpc
        PROTOS ${PROTO_FILE}
        PROTOC_OUT_DIR ${PROTO_GEN_DIR}
        DEPENDENCIES ${PROTO_FILE}
    )
endforeach()

# 收集所有生成的源文件
file(GLOB_RECURSE GEN_SOURCES
    ${PROTO_GEN_DIR}/*.pb.cc
    ${PROTO_GEN_DIR}/*.pb.h
)

# 创建静态库
add_library(rpc_protos STATIC
    ${GEN_SOURCES}
)

# 包含路径
target_include_directories(rpc_protos
    PUBLIC
        ${PROTO_GEN_DIR}
        ${Protobuf_INCLUDE_DIRS}
        ${gRPC_INCLUDE_DIRS}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# 链接库
target_link_libraries(rpc_protos
    PUBLIC
        protobuf::libprotobuf
        gRPC::grpc++
        gRPC::grpc
    PRIVATE
        pthread
)

# 安装规则
install(TARGETS rpc_protos
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(DIRECTORY ${PROTO_GEN_DIR}/
    DESTINATION include
    FILES_MATCHING
    PATTERN "*.h"
)

# 导出头文件路径供其他项目使用
target_include_directories(rpc_protos
    INTERFACE
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${PROTO_GEN_DIR}>
)