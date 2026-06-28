# WebServer - C++ 原生全栈 Web 服务 → gRPC 微服务架构

> **从零实现的 C++ HTTP 服务器 → gRPC 微服务架构演进**
>
> 用最底层的方式理解 Web 工作原理，用微服务架构承载业务扩展

![C++](https://img.shields.io/badge/C++-17-%2300599C?style=flat-square&logo=c%2B%2B)
![Vue](https://img.shields.io/badge/Vue-3-%234FC08D?style=flat-square&logo=vue.js)
![gRPC](https://img.shields.io/badge/gRPC-1.0-%234285F4?style=flat-square&logo=grpc)
![MySQL](https://img.shields.io/badge/MySQL-8-%234479A1?style=flat-square&logo=mysql)
![Protobuf](https://img.shields.io/badge/Protobuf-3.15-%23FF6C37?style=flat-square&logo=protocol-buffers)
![Rust](https://img.shields.io/badge/Rust-1.70-%23DEA584?style=flat-square&logo=rust)
![Go](https://img.shields.io/badge/Go-1.21-%2300ADD8?style=flat-square&logo=go)

---

## 📖 项目简介

本项目是一个**从零开始、不依赖任何第三方 Web 框架**的 C++ HTTP 服务器，逐步演进为完整的 gRPC 微服务架构。

> **原 C++ 单体后端已归档**：[WebSever_cpp](https://github.com/jyoushitou/WebSever_cpp.git)
>
> 归档版本为纯 C++ 实现的单体 HTTP 服务器，当前仓库为微服务架构演进版本。

### 架构演进路线

```
阶段一：单体架构（已归档）
  C++ 原生 Socket HTTP 服务器 + Vue 3 前端
  └── 手动解析 HTTP 协议、多线程处理、MySQL 直连
  └── 归档仓库：https://github.com/jyoushitou/WebSever_cpp.git

阶段二：微服务架构（进行中）
  gRPC 微服务 + 独立 Proto 仓库 + 服务拆分
  └── 网关统一入口、业务解耦、独立部署
  └── 多语言支持：C++ / Rust / Go

阶段三：容器化部署（规划中）
  Docker + Kubernetes + CI/CD
  └── 弹性伸缩、服务治理、自动化运维
```

### 核心理念

从 TCP Socket 连接到 HTTP 协议解析，从多线程并发到 gRPC 微服务通信，从单一语言到多语言异构架构，完整呈现一个 Web 服务器是如何炼成的。每个组件都亲手实现，深入理解每一层的工作原理。

---

## 🏗️ 项目结构

```
WebServer/                              # 总仓库（Git 根仓库）
│
├── proto/                              # [子仓库] Proto 接口定义
│   ├── source/                         # Proto 源文件
│   │   ├── common/                     # 公共类型、错误码
│   │   │   ├── common.proto            # Empty、PageRequest 等通用消息
│   │   │   └── error_code.proto        # 全局错误码枚举
│   │   ├── gateway/                    # 网关服务接口
│   │   │   └── gateway_service.proto   # 路由、限流、鉴权
│   │   ├── user/                       # 用户服务接口
│   │   │   └── user_service.proto      # 注册、登录、用户信息
│   │   ├── article/                    # 文章服务接口
│   │   │   └── article_service.proto   # 文章 CRUD、分类、标签
│   │   ├── blog/                       # 博客服务接口
│   │   │   └── blog_service.proto      # 博客管理、评论、点赞
│   │   ├── image/                      # 图片服务接口
│   │   │   └── image_service.proto     # 图片上传、处理、存储
│   │   ├── video/                      # 视频服务接口
│   │   │   └── video_service.proto     # 视频上传、转码、流媒体
│   │   ├── search/                     # 搜索服务接口
│   │   │   └── search_service.proto    # 全文搜索、索引管理
│   │   ├── frontend/                   # 前端服务接口
│   │   │   └── frontend_service.proto  # 页面数据、事件推送、文件传输
│   │   ├── CMakeLists.txt              # 编译为静态库 librpc_protos.a
│   │   └── README.md                   # Proto 仓库使用说明
│   └── build/                          # 编译输出目录
│       ├── lib/                        # 编译后的库文件
│       ├── bin/                        # 可执行文件
│       └── generated/                  # 生成的 .pb.h / .pb.cc
│
├── GRPCGateway/                        # [子仓库] gRPC 网关 (C++)
│   └── 职责：协议转换、路由分发、限流控制、鉴权验证
│
├── UserService/                        # [子仓库] 用户服务 (Rust)
│   └── 职责：用户注册/登录、Token 管理、权限控制、设备管理
│
├── ArticleService/                     # [子仓库] 文章服务 (Go)
│   └── 职责：文章 CRUD、分类管理、标签管理、文章搜索
│
├── BlogService/                        # [子仓库] 博客服务 (Go)
│   └── 职责：博客管理、评论系统、点赞收藏、博客推荐
│
├── ImageService/                       # [子仓库] 图片服务 (Rust)
│   └── 职责：图片上传、缩略图生成、格式转换、CDN 分发
│
├── VideoService/                       # [子仓库] 视频服务 (Rust)
│   └── 职责：视频上传、转码处理、切片存储、流媒体播放
│
├── SearchService/                      # [子仓库] 搜索服务 (Go)
│   └── 职责：全文索引、搜索排序、搜索建议、索引同步
│
├── vue/                                # [子仓库] Vue 3 前端
│   └── 职责：用户界面、gRPC-Web 通信、实时数据展示
│
├── .gitmodules                         # Git 子模块配置
└── README.md                           # 本文件
```

---

## 🚀 快速开始

### 前置依赖

| 组件 | 版本要求 | 说明 |
|------|----------|------|
| C++ 编译器 | C++17（GCC 8+ / MSVC 2019+） | 网关编译 |
| CMake | 3.10+ | C++ 构建系统 |
| MySQL | 8.0+（含 `libmysqlclient-dev`） | 数据库 |
| gRPC | 1.40+ | 微服务通信框架 |
| Protobuf | 3.15+ | 序列化协议 |
| Rust | 1.70+ | UserService、ImageService、VideoService |
| Go | 1.21+ | ArticleService、BlogService、SearchService |
| Node.js | 18+ | 前端构建 |
| npm | 9+ | 前端包管理 |

### 1️⃣ 克隆仓库（含子模块）

```bash
git clone --recursive https://github.com/jyoushitou/WebServer.git
cd WebServer

# 如果已克隆但未拉取子模块
git submodule update --init --recursive
```

### 2️⃣ 编译 Proto 库

Proto 仓库独立编译为静态库，供所有微服务引用：

```bash
cd proto/source
mkdir -p ../build && cd ../build
cmake ../source
make -j$(nproc)
```

编译完成后：
- `build/lib/librpc_protos.a` — 静态库
- `build/generated/` — 生成的 `.pb.h` 和 `.pb.cc` 文件

### 3️⃣ 编译并启动各微服务

#### 编译 GRPCGateway（网关 - C++）

```bash
cd GRPCGateway
mkdir build && cd build
cmake ..
cmake --build .
./GRPCGateway
```

#### 编译 UserService（用户服务 - Rust）

```bash
cd UserService
cargo build --release
./target/release/user-service
```

#### 编译 ArticleService（文章服务 - Go）

```bash
cd ArticleService
go build -o article-service .
./article-service
```

#### 编译 BlogService（博客服务 - Go）

```bash
cd BlogService
go build -o blog-service .
./blog-service
```

#### 编译 ImageService（图片服务 - Rust）

```bash
cd ImageService
cargo build --release
./target/release/image-service
```

#### 编译 VideoService（视频服务 - Rust）

```bash
cd VideoService
cargo build --release
./target/release/video-service
```

#### 编译 SearchService（搜索服务 - Go）

```bash
cd SearchService
go build -o search-service .
./search-service
```

### 4️⃣ 启动前端

```bash
cd vue
npm install
npm run dev
```

前端开发服务器运行在 **http://localhost:60907**。

---

## 🔌 gRPC 微服务接口

### GatewayService — 网关服务（C++，端口:50051）

| RPC | 请求 | 响应 | 说明 |
|-----|------|------|------|
| `RouteRequest` | service_name, method_name, payload, metadata | status_code, data, error_message | 路由请求到后端微服务 |
| `HttpToGrpc` | method, path, headers, body, query_params | status_code, headers, body | HTTP 协议转 gRPC |
| `RateLimit` | client_ip, api_path, request_count | allowed, remaining_quota, reset_time_seconds | 限流控制 |
| `Authenticate` | token, service_name, method_name | authenticated, user_id, permissions | 鉴权验证 |

### UserService — 用户服务（Rust，端口:50052）

| RPC | 请求 | 响应 | 说明 |
|-----|------|------|------|
| `Register` | username, password, email | user_id, token, created_at | 用户注册 |
| `Login` | username, password | token, user_info, expires_at | 用户登录 |
| `Logout` | token, device_id | success, message | 注销登录 |
| `GetUserInfo` | user_id | user_id, username, avatar, email, permission, created_at | 获取用户信息 |
| `UpdateUserInfo` | user_id, username, avatar, email | success, message | 更新用户信息 |
| `ListDevices` | user_id | devices[] (device_id, device_name, last_login_ip, last_login_time) | 获取登录设备列表 |
| `RemoveDevice` | user_id, device_id | success, message | 移除登录设备 |
| `VerifyToken` | token | valid, user_id, permissions | Token 验证 |

### ArticleService — 文章服务（Go，端口:50053）

| RPC | 请求 | 响应 | 说明 |
|-----|------|------|------|
| `CreateArticle` | title, content, category_id, tags[], author_id | article_id, created_at | 创建文章 |
| `GetArticle` | article_id | article_id, title, content, category, tags[], author, created_at, updated_at, view_count | 获取文章详情 |
| `UpdateArticle` | article_id, title, content, category_id, tags[] | success, updated_at | 更新文章 |
| `DeleteArticle` | article_id | success, message | 删除文章 |
| `ListArticles` | page, page_size, category_id, tag_id | articles[], total, page, page_size | 文章列表（分页） |
| `GetCategories` | Empty | categories[] (category_id, name, article_count) | 获取分类列表 |
| `CreateCategory` | name, description | category_id, created_at | 创建分类 |
| `GetTags` | Empty | tags[] (tag_id, name, article_count) | 获取标签列表 |

### BlogService — 博客服务（Go，端口:50054）

| RPC | 请求 | 响应 | 说明 |
|-----|------|------|------|
| `CreatePost` | title, content, author_id, tags[] | post_id, created_at | 创建博客文章 |
| `GetPost` | post_id | post_id, title, content, author, tags[], created_at, updated_at, like_count, comment_count | 获取博客详情 |
| `UpdatePost` | post_id, title, content, tags[] | success, updated_at | 更新博客 |
| `DeletePost` | post_id | success, message | 删除博客 |
| `ListPosts` | page, page_size, tag_id, author_id | posts[], total, page, page_size | 博客列表（分页） |
| `AddComment` | post_id, author_id, content | comment_id, created_at | 添加评论 |
| `ListComments` | post_id, page, page_size | comments[] (comment_id, author, content, created_at), total | 评论列表 |
| `DeleteComment` | comment_id, author_id | success, message | 删除评论 |
| `LikePost` | post_id, user_id | success, like_count | 点赞博客 |
| `UnlikePost` | post_id, user_id | success, like_count | 取消点赞 |
| `GetRecommendations` | user_id, page, page_size | posts[], total | 博客推荐 |

### ImageService — 图片服务（Rust，端口:50055）

| RPC | 请求 | 响应 | 说明 |
|-----|------|------|------|
| `UploadImage` | stream<file_name, content, chunk_index, total_chunks> | image_id, url, thumbnail_url, size_bytes, format | 图片上传（客户端流） |
| `GetImage` | image_id | image_id, url, thumbnail_url, format, width, height, size_bytes, created_at | 获取图片信息 |
| `DeleteImage` | image_id | success, message | 删除图片 |
| `ListImages` | user_id, page, page_size | images[], total | 图片列表 |
| `ProcessImage` | image_id, operations[] (resize/crop/rotate/format_convert) | image_id, url, thumbnail_url | 图片处理 |
| `GenerateThumbnail` | image_id, width, height | thumbnail_url | 生成缩略图 |

### VideoService — 视频服务（Rust，端口:50056）

| RPC | 请求 | 响应 | 说明 |
|-----|------|------|------|
| `UploadVideo` | stream<file_name, content, chunk_index, total_chunks> | video_id, status (uploading/processing/ready) | 视频上传（客户端流） |
| `GetVideoInfo` | video_id | video_id, title, url, duration, format, resolution, size_bytes, status, created_at | 获取视频信息 |
| `DeleteVideo` | video_id | success, message | 删除视频 |
| `ListVideos` | user_id, page, page_size | videos[], total | 视频列表 |
| `StartTranscoding` | video_id, target_formats[], resolutions[] | job_id, status | 启动转码任务 |
| `GetTranscodingStatus` | job_id | job_id, status, progress, output_formats[] | 查询转码进度 |
| `GetStreamUrl` | video_id, format, resolution | stream_url, expires_at | 获取流媒体播放地址 |
| `GetVideoChunk` | video_id, chunk_index, format, resolution | stream<content, chunk_index, total_chunks> | 获取视频切片（服务端流） |

### SearchService — 搜索服务（Go，端口:50057）

| RPC | 请求 | 响应 | 说明 |
|-----|------|------|------|
| `Search` | query, type (article/blog/image/video/all), page, page_size, filters | results[], total, page, page_size, elapsed_ms | 全文搜索 |
| `IndexDocument` | document_id, type, title, content, tags[], metadata | success, indexed_at | 索引文档 |
| `RemoveIndex` | document_id, type | success, message | 移除索引 |
| `RebuildIndex` | type (article/blog/image/video/all) | job_id, status | 重建索引 |
| `GetSearchSuggestions` | prefix, type, limit | suggestions[] (text, count) | 搜索建议 |
| `GetTrendingSearches` | limit | trending[] (query, count) | 热门搜索 |

### FrontendService — 前端服务

| RPC | 请求 | 响应 | 说明 |
|-----|------|------|------|
| `GetPageData` | page_name, params, pagination | page_name, data, pagination | 获取页面渲染数据 |
| `UserAction` | action_type, action_data, session_id | success, result, message | 用户交互操作 |
| `SubscribeEvents` | user_id, event_types[] | stream<event_type, data, timestamp> | 实时事件推送（服务端流） |
| `UploadFile` | stream<file_name, content, chunk_index, total_chunks, file_id> | file_id, url, size_bytes | 文件上传（客户端流） |
| `DownloadFile` | file_id, file_name | stream<file_name, content, chunk_index, total_chunks, file_id> | 文件下载（服务端流） |

---

## 🧠 架构设计

### 目标架构（微服务）

```
                    ┌──────────────────────────────────────┐
                    │            Vue 前端                   │ (2核2g 服务器)
                    │         (gRPC-Web)                   │
                    └──────────────┬───────────────────────┘
                                   │ gRPC-Web (通过 Nginx 代理)
                                   ▼
                    ┌──────────────────────────────────────┐
                    │         GRPCGateway (C++)             │
                    │     协议转换 / 路由 / 限流 / 鉴权      │
                    └──┬────┬────┬────┬────┬────┬────┬────┘
                       │    │    │    │    │    │    │
              ┌────────┘    │    │    │    │    │    └────────┐
              ▼             ▼    ▼    ▼    ▼    ▼             ▼
     ┌────────────┐  ┌────────┐ ┌──────┐ ┌──────┐ ┌──────────┐
     │ UserService│  │Article │ │Blog  │ │Image │ │Video     │
     │  (Rust)    │  │ (Go)   │ │(Go)  │ │(Rust)│ │(Rust)    │
     │  :50052    │  │:50053  │ │:50054│ │:50055│ │:50056    │
     └────────────┘  └────────┘ └──────┘ └──────┘ └──────────┘
              │         │         │        │        │
              └─────────┴─────────┴────────┴────────┴──────────┐
                                                               │
                                                               ▼
                                                    ┌──────────────────┐
                                                    │ SearchService   │
                                                    │    (Go)         │
                                                    │    :50057       │
                                                    └──────────────────┘
                                                               │
                                                               ▼
                                                    ┌──────────────────┐
                                                    │  Elasticsearch   │
                                                    │   (搜索引擎)      │
                                                    └──────────────────┘
```

### 微服务间调用关系

```
用户注册流程：
  Vue前端 → gRPC-Web → GRPCGateway → UserService(Rust) → MySQL
  → 返回 token → GRPCGateway → Vue前端

发布文章流程：
  Vue前端 → gRPC-Web → GRPCGateway → ArticleService(Go) → MySQL
  → ArticleService 调用 SearchService 更新索引
  → 返回文章ID → GRPCGateway → Vue前端

上传图片流程：
  Vue前端 → gRPC-Web → GRPCGateway → ImageService(Rust)
  → 生成缩略图 → 存储到对象存储
  → 返回图片URL → GRPCGateway → Vue前端

搜索流程：
  Vue前端 → gRPC-Web → GRPCGateway → SearchService(Go)
  → 查询 Elasticsearch → 聚合结果
  → 返回搜索结果 → GRPCGateway → Vue前端
```

### 多语言技术选型

| 微服务 | 语言 | 选型理由 |
|--------|------|----------|
| GRPCGateway | C++ | 高性能网关，协议转换核心 |
| UserService | Rust | 内存安全，用户敏感数据处理 |
| ArticleService | Go | 快速开发，CRUD 密集型 |
| BlogService | Go | 与文章服务类似，统一技术栈 |
| ImageService | Rust | 图片处理性能要求高，内存安全 |
| VideoService | Rust | 视频转码 CPU 密集型，性能优先 |
| SearchService | Go | 搜索聚合逻辑，并发处理强 |

### 部署架构

```
┌──────────────────────────────────────────────────────────────┐
│ 服务器 A (5核10g)                                            │
│  ├── GRPCGateway     (C++,   端口:50051)                     │
│  ├── UserService     (Rust,  端口:50052)                     │
│  ├── ArticleService  (Go,    端口:50053)                     │
│  ├── BlogService     (Go,    端口:50054)                     │
│  ├── ImageService    (Rust,  端口:50055)                     │
│  ├── VideoService    (Rust,  端口:50056)                     │
│  ├── SearchService   (Go,    端口:50057)                     │
│  └── MySQL + Elasticsearch                                   │
└──────────────────┬───────────────────────────────────────────┘
                   │ gRPC 内部通信
                   ▼
┌──────────────────────────────────────────────────────────────┐
│ 服务器 B (2核2g)                                             │
│  └── Vue 前端 + Nginx (gRPC-Web 代理)                        │
└──────────────────────────────────────────────────────────────┘
```

### Proto 独立仓库设计

```
proto-repo/                    # 独立 Git 仓库
├── source/                    # Proto 源文件
│   ├── common/                # 公共类型（被所有服务引用）
│   │   ├── common.proto       # Empty、PageRequest 等通用消息
│   │   └── error_code.proto   # 全局错误码枚举
│   ├── gateway/               # 网关接口
│   │   └── gateway_service.proto
│   ├── user/                  # 用户服务接口
│   │   └── user_service.proto
│   ├── article/               # 文章服务接口
│   │   └── article_service.proto
│   ├── blog/                  # 博客服务接口
│   │   └── blog_service.proto
│   ├── image/                 # 图片服务接口
│   │   └── image_service.proto
│   ├── video/                 # 视频服务接口
│   │   └── video_service.proto
│   ├── search/                # 搜索服务接口
│   │   └── search_service.proto
│   └── frontend/              # 前端接口
│       └── frontend_service.proto
└── build/                     # 编译输出
    ├── lib/                   # 编译后的库文件
    ├── bin/                   # 可执行文件
    └── generated/             # 生成的 .pb.h / .pb.cc

编译为静态库 librpc_protos.a
C++ 微服务通过 find_package 引用
Rust 微服务通过 prost + tonic 直接编译 proto
Go 微服务通过 protoc-gen-go 生成代码
```

**优势**：
- **类型安全**：所有微服务使用同一份 proto 定义，接口变更编译期即可发现
- **版本控制**：proto 版本变更可追溯，支持多版本共存
- **语言无关**：可生成 C++/Rust/Go/Java/Python 等代码，支持多语言微服务
- **编译隔离**：修改 proto 只需重新编译依赖库，无需修改业务代码
- **可移植性**：新微服务直接引用 proto 库，开箱即用

### Git Submodule 管理

```bash
# 总仓库添加子仓库
git submodule add <proto-repo-url> proto
git submodule add <gateway-repo-url> GRPCGateway
git submodule add <user-repo-url> UserService
git submodule add <article-repo-url> ArticleService
git submodule add <blog-repo-url> BlogService
git submodule add <image-repo-url> ImageService
git submodule add <video-repo-url> VideoService
git submodule add <search-repo-url> SearchService
git submodule add <vue-repo-url> vue

# 克隆总仓库时拉取所有子仓库
git clone --recursive <webserver-repo-url>

# 更新所有子仓库到最新
git submodule update --remote --recursive
```

---

## ⚙️ 配置说明

### 数据库配置

| 参数 | 默认值 | 说明 |
|------|--------|------|
| 主机 (Windows) | `localhost` | 本地开发 |
| 主机 (Linux) | `192.168.0.52` | 远程部署 |
| 端口 | `3306` | MySQL 端口 |
| 用户 | `web_server` | 数据库用户名 |
| 密码 | `123456` | 数据库密码 |
| 数据库 | `web_server` | 数据库名称 |

### 服务器端口

| 服务 | 语言 | 端口 | 说明 |
|------|------|------|------|
| HTTP 服务器（归档） | C++ | `60906` | 原单体后端 API |
| Vue 开发服务器 | JavaScript | `60907` | 前端开发 |
| GRPCGateway | C++ | `50051` | gRPC 网关 |
| UserService | Rust | `50052` | 用户服务 |
| ArticleService | Go | `50053` | 文章服务 |
| BlogService | Go | `50054` | 博客服务 |
| ImageService | Rust | `50055` | 图片服务 |
| VideoService | Rust | `50056` | 视频服务 |
| SearchService | Go | `50057` | 搜索服务 |

### 数据库初始化

```sql
CREATE DATABASE IF NOT EXISTS web_server;
USE web_server;

-- 用户表
CREATE TABLE IF NOT EXISTS users (
    id INT AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(50) UNIQUE NOT NULL,
    password VARCHAR(255) NOT NULL,
    email VARCHAR(100),
    permission INT DEFAULT 1,
    avatar VARCHAR(255) DEFAULT '',
    login_count INT DEFAULT 0,
    last_login_time DATETIME,
    last_login_ip VARCHAR(45),
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP
);

-- 文章表
CREATE TABLE IF NOT EXISTS articles (
    id INT AUTO_INCREMENT PRIMARY KEY,
    title VARCHAR(200) NOT NULL,
    content TEXT,
    category_id INT,
    author_id INT NOT NULL,
    view_count INT DEFAULT 0,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    updated_at DATETIME DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    FOREIGN KEY (author_id) REFERENCES users(id)
);

-- 分类表
CREATE TABLE IF NOT EXISTS categories (
    id INT AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(50) UNIQUE NOT NULL,
    description VARCHAR(200),
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP
);

-- 标签表
CREATE TABLE IF NOT EXISTS tags (
    id INT AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(50) UNIQUE NOT NULL,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP
);

-- 文章-标签关联表
CREATE TABLE IF NOT EXISTS article_tags (
    article_id INT NOT NULL,
    tag_id INT NOT NULL,
    PRIMARY KEY (article_id, tag_id),
    FOREIGN KEY (article_id) REFERENCES articles(id) ON DELETE CASCADE,
    FOREIGN KEY (tag_id) REFERENCES tags(id) ON DELETE CASCADE
);

-- 博客表
CREATE TABLE IF NOT EXISTS blog_posts (
    id INT AUTO_INCREMENT PRIMARY KEY,
    title VARCHAR(200) NOT NULL,
    content TEXT,
    author_id INT NOT NULL,
    like_count INT DEFAULT 0,
    comment_count INT DEFAULT 0,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    updated_at DATETIME DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    FOREIGN KEY (author_id) REFERENCES users(id)
);

-- 评论表
CREATE TABLE IF NOT EXISTS comments (
    id INT AUTO_INCREMENT PRIMARY KEY,
    post_id INT NOT NULL,
    author_id INT NOT NULL,
    content TEXT NOT NULL,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (post_id) REFERENCES blog_posts(id) ON DELETE CASCADE,
    FOREIGN KEY (author_id) REFERENCES users(id)
);

-- 点赞表
CREATE TABLE IF NOT EXISTS likes (
    post_id INT NOT NULL,
    user_id INT NOT NULL,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (post_id, user_id),
    FOREIGN KEY (post_id) REFERENCES blog_posts(id) ON DELETE CASCADE,
    FOREIGN KEY (user_id) REFERENCES users(id)
);

-- 图片表
CREATE TABLE IF NOT EXISTS images (
    id INT AUTO_INCREMENT PRIMARY KEY,
    user_id INT NOT NULL,
    filename VARCHAR(255) NOT NULL,
    url VARCHAR(500) NOT NULL,
    thumbnail_url VARCHAR(500),
    format VARCHAR(10),
    width INT,
    height INT,
    size_bytes BIGINT,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (user_id) REFERENCES users(id)
);

-- 视频表
CREATE TABLE IF NOT EXISTS videos (
    id INT AUTO_INCREMENT PRIMARY KEY,
    user_id INT NOT NULL,
    title VARCHAR(200),
    filename VARCHAR(255) NOT NULL,
    url VARCHAR(500),
    duration INT,
    format VARCHAR(10),
    resolution VARCHAR(20),
    size_bytes BIGINT,
    status ENUM('uploading', 'processing', 'ready', 'failed') DEFAULT 'uploading',
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (user_id) REFERENCES users(id)
);
```

---

## 📋 开发计划

### 阶段一：单体架构 ✅ 已归档
- [x] C++ 原生 Socket HTTP 服务器
- [x] HTTP 协议手动解析
- [x] 多线程并发处理
- [x] MySQL 数据库直连
- [x] Vue 3 前端界面
- [x] 用户认证系统（Token）
- [x] 异步任务处理
- [x] 归档仓库：https://github.com/jyoushitou/WebSever_cpp.git

### 阶段二：微服务架构 🔄 进行中
- [x] Proto 独立仓库搭建
- [ ] GRPCGateway 网关服务实现（C++）
- [ ] UserService 用户服务实现（Rust）
- [ ] ArticleService 文章服务实现（Go）
- [ ] BlogService 博客服务实现（Go）
- [ ] ImageService 图片服务实现（Rust）
- [ ] VideoService 视频服务实现（Rust）
- [ ] SearchService 搜索服务实现（Go）
- [ ] Vue 前端 gRPC-Web 接入
- [ ] 服务注册与发现（Consul/etcd）
- [ ] 配置中心

### 阶段三：容器化部署 📋 规划中
- [ ] Docker 容器化
- [ ] Docker Compose 编排
- [ ] Kubernetes 部署
- [ ] CI/CD 流水线
- [ ] 监控告警（Prometheus + Grafana）
- [ ] 链路追踪（Jaeger）

---

## 🤝 贡献指南

1. Fork 本仓库
2. 创建特性分支 (`git checkout -b feature/amazing-feature`)
3. 提交更改 (`git commit -m 'Add amazing feature'`)
4. 推送到分支 (`git push origin feature/amazing-feature`)
5. 创建 Pull Request

---

## 📄 许可证

本项目采用 MIT 许可证 - 详见 [LICENSE](LICENSE) 文件

---

## 📬 联系方式

- 项目维护者：[jyoushitou]
- 邮箱：[xzt98948364@outlook.com]
- 项目地址：[https://github.com/jyoushitou/WebServer](https://github.com/jyoushitou/WebServer)
- 归档仓库：[https://github.com/jyoushitou/WebSever_cpp.git](https://github.com/jyoushitou/WebSever_cpp.git)