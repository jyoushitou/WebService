# WebServer - C++ gRPC 微服务框架

> **从零实现的 C++ HTTP 服务器 → gRPC 微服务架构演进**
>
> 用最底层的方式理解 Web 工作原理，用微服务架构承载业务扩展

![C++](https://img.shields.io/badge/C++-17-%2300599C?style=flat-square&logo=c%2B%2B)
![Rust](https://img.shields.io/badge/Rust-1.70-%23DEA584?style=flat-square&logo=rust)
![Go](https://img.shields.io/badge/Go-1.21-%2300ADD8?style=flat-square&logo=go)
![Vue](https://img.shields.io/badge/Vue-3-%234FC08D?style=flat-square&logo=vue.js)
![gRPC](https://img.shields.io/badge/gRPC-1.0-%234285F4?style=flat-square&logo=grpc)
![MySQL](https://img.shields.io/badge/MySQL-8-%234479A1?style=flat-square&logo=mysql)
![Protobuf](https://img.shields.io/badge/Protobuf-3.15-%23FF6C37?style=flat-square&logo=protocol-buffers)
![Elasticsearch](https://img.shields.io/badge/Elasticsearch-7-%23005571?style=flat-square&logo=elasticsearch)
![Docker](https://img.shields.io/badge/Docker-24-%232496ED?style=flat-square&logo=docker)
![Kubernetes](https://img.shields.io/badge/Kubernetes-1.28-%23326CE5?style=flat-square&logo=kubernetes)

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
  gRPC 微服务框架 + 独立 Proto 仓库 + 服务拆分
  └── 核心框架：C++ 实现网关、注册发现、配置中心、链路追踪、监控告警
  └── 业务服务：Rust/Go 实现用户、文章、博客、图片、视频、搜索
  └── 多语言支持：C++ / Rust / Go

阶段三：容器化部署（规划中）
  Docker + Kubernetes + CI/CD
  └── 弹性伸缩、服务治理、自动化运维
```

---

## 🏗️ 项目结构

```
WebServer/                              # 总仓库（Git 根仓库）
│
├── proto/                              # [子仓库] Proto 接口定义
│   ├── source/                         # Proto 源文件
│   │   ├── common/                     # 公共类型、错误码
│   │   ├── gateway/                    # 网关服务接口
│   │   ├── registry/                   # 服务注册发现接口
│   │   ├── config/                     # 配置中心接口
│   │   ├── tracing/                    # 链路追踪接口
│   │   ├── monitor/                    # 监控告警接口
│   │   ├── user/                       # 用户服务接口
│   │   ├── article/                    # 文章服务接口
│   │   ├── blog/                       # 博客服务接口
│   │   ├── image/                      # 图片服务接口
│   │   ├── video/                      # 视频服务接口
│   │   ├── search/                     # 搜索服务接口
│   │   └── frontend/                   # 前端服务接口
│   └── build/                          # 编译输出
│
├── GRPCGateway/                        # [子仓库] 网关 (C++) ✅ 核心
│   └── 对外统一入口，协议转换，路由分发，限流控制，鉴权验证
│
├── ServiceRegistry/                    # [子仓库] 服务注册发现 (C++) ✅ 核心
│   └── 动态路由，负载均衡，健康检查，服务上下线
│
├── ConfigCenter/                       # [子仓库] 配置中心 (C++) ⚠️ 可选
│   └── 统一配置管理，热更新，配置版本控制
│
├── TracingService/                     # [子仓库] 链路追踪 (C++) ⚠️ 可选
│   └── 请求链路追踪，性能分析，故障排查
│
├── MonitorService/                     # [子仓库] 监控告警 (C++) ⚠️ 可选
│   └── 指标采集，告警规则，可视化面板
│
├── UserService/                        # [子仓库] 用户服务 (Rust)
│   └── 用户注册/登录、Token 管理、权限控制、设备管理
│
├── ArticleService/                     # [子仓库] 文章服务 (Go)
│   └── 文章 CRUD、分类管理、标签管理、文章搜索
│
├── BlogService/                        # [子仓库] 博客服务 (Go)
│   └── 博客管理、评论系统、点赞收藏、博客推荐
│
├── ImageService/                       # [子仓库] 图片服务 (Rust)
│   └── 图片上传、缩略图生成、格式转换、CDN 分发
│
├── VideoService/                       # [子仓库] 视频服务 (Rust)
│   └── 视频上传、转码处理、切片存储、流媒体播放
│
├── SearchService/                      # [子仓库] 搜索服务 (Go)
│   └── 全文索引、搜索排序、搜索建议、索引同步
│
├── vue/                                # [子仓库] Vue 3 前端
│   └── 用户界面、gRPC-Web 通信、实时数据展示
│
├── .gitmodules                         # Git 子模块配置
└── README.md                           # 本文件
```

---

## 🧠 架构设计

### 完整架构图

```
                    ┌──────────────────────────────────────┐
                    │            Vue 前端                   │ (2核2g 服务器)
                    │         (gRPC-Web)                   │
                    └──────────────┬───────────────────────┘
                                   │ gRPC-Web (通过 Nginx 代理)
                                   ▼
┌──────────────────────────────────────────────────────────────────┐
│                     GRPCGateway (C++)                            │
│  对外统一入口 / HTTP-gRPC协议转换 / 路由分发 / 限流控制 / 鉴权验证 │
└──────┬─────────────────────────────────────────────────────┬─────┘
       │                                                     │
       │ 服务发现                                            │ 配置拉取
       ▼                                                     ▼
┌──────────────────┐                                ┌──────────────────┐
│ ServiceRegistry  │◄──────── 注册/心跳 ────────────│  ConfigCenter   │
│  (C++)           │                                │   (C++)         │
│ 服务注册发现      │                                │   配置中心       │
│ 动态路由/负载均衡  │                                │   统一配置管理    │
│ 健康检查/服务上下线 │                                │   热更新/版本控制 │
└──────────────────┘                                └──────────────────┘
       │                                                     │
       │ 路由到业务服务                                       │ 配置下发
       ▼                                                     ▼
┌──────────────────────────────────────────────────────────────────┐
│                    业务微服务层                                   │
├────────────┬───────────┬──────────┬───────────┬─────────────────┤
│ UserService│  Article  │   Blog   │   Image   │     Video       │
│  (Rust)    │  (Go)     │  (Go)    │  (Rust)   │    (Rust)       │
│  :50052    │  :50053   │  :50054  │  :50055   │    :50056       │
├────────────┴───────────┴──────────┴───────────┴─────────────────┤
│                           │                                     │
│                           ▼                                     │
│                    ┌──────────────┐                             │
│                    │SearchService │                             │
│                    │   (Go)       │                             │
│                    │   :50057     │                             │
│                    └──────┬───────┘                             │
│                           │                                     │
│                           ▼                                     │
│                    ┌──────────────┐                             │
│                    │Elasticsearch │                             │
│                    │  (搜索引擎)   │                             │
│                    └──────────────┘                             │
└──────────────────────────────────────────────────────────────────┘
       │                                                     │
       │ 链路追踪                                            │ 监控指标
       ▼                                                     ▼
┌──────────────────┐                                ┌──────────────────┐
│ TracingService   │                                │ MonitorService  │
│  (C++)           │                                │   (C++)         │
│  链路追踪         │                                │   监控告警       │
│  请求全链路跟踪    │                                │   指标采集       │
│  性能分析/故障排查  │                                │   告警规则/可视化 │
└──────────────────┘                                └──────────────────┘
```

### 核心框架组件（C++ 实现）

| 微服务 | 优先级 | 职责 | 说明 |
|--------|--------|------|------|
| **GRPCGateway** | ✅ 核心 | 对外统一入口 | 协议转换、路由分发、限流控制、鉴权验证 |
| **ServiceRegistry** | ✅ 核心 | 服务注册发现 | 动态路由、负载均衡、健康检查、服务上下线 |
| **ConfigCenter** | ✅ 可选 | 配置中心 | 统一配置管理、热更新、配置版本控制 |
| **TracingService** | ✅ 可选 | 链路追踪 | 请求全链路跟踪、性能分析、故障排查 |
| **MonitorService** | ✅ 可选 | 监控告警 | 指标采集、告警规则、可视化面板 |

### 业务微服务（多语言）

| 微服务 | 语言 | 端口 | 职责 |
|--------|------|------|------|
| UserService | Rust | 50052 | 用户注册/登录、Token 管理、权限控制 |
| ArticleService | Go | 50053 | 文章 CRUD、分类管理、标签管理 |
| BlogService | Go | 50054 | 博客管理、评论系统、点赞收藏 |
| ImageService | Rust | 50055 | 图片上传、缩略图生成、格式转换 |
| VideoService | Rust | 50056 | 视频上传、转码处理、流媒体播放 |
| SearchService | Go | 50057 | 全文索引、搜索排序、搜索建议 |

---

## 🔌 核心框架微服务接口

### GRPCGateway — 网关服务（C++，端口:50051）

对外统一入口，负责 HTTP/gRPC 协议转换、路由分发、限流控制、鉴权验证。

| RPC | 请求 | 响应 | 说明 |
|-----|------|------|------|
| `RouteRequest` | service_name, method_name, payload, metadata | status_code, data, error_message | 路由请求到后端微服务 |
| `HttpToGrpc` | method, path, headers, body, query_params | status_code, headers, body | HTTP 协议转 gRPC |
| `RateLimit` | client_ip, api_path, request_count | allowed, remaining_quota, reset_time_seconds | 限流控制 |
| `Authenticate` | token, service_name, method_name | authenticated, user_id, permissions | 鉴权验证 |

### ServiceRegistry — 服务注册发现（C++，端口:51051）

动态路由、负载均衡、健康检查、服务上下线通知。

| RPC | 请求 | 响应 | 说明 |
|-----|------|------|------|
| `Register` | service_name, instance_id, host, port, metadata | success, ttl_seconds | 服务注册 |
| `Unregister` | service_name, instance_id | success, message | 服务注销 |
| `Heartbeat` | service_name, instance_id | success, ttl_seconds | 心跳续约 |
| `Discover` | service_name | instances[] (host, port, metadata, status) | 服务发现 |
| `Watch` | service_name | stream<event_type, instance> | 服务变更监听（服务端流） |
| `HealthCheck` | service_name, instance_id | status, last_heartbeat | 健康检查 |
| `ListServices` | Empty | services[] (service_name, instance_count) | 获取所有服务列表 |

### ConfigCenter — 配置中心（C++，端口:51052）

统一配置管理、热更新、配置版本控制。

| RPC | 请求 | 响应 | 说明 |
|-----|------|------|------|
| `GetConfig` | service_name, config_key | config_value, version, updated_at | 获取配置 |
| `SetConfig` | service_name, config_key, config_value | version, success | 设置配置 |
| `DeleteConfig` | service_name, config_key | success, message | 删除配置 |
| `WatchConfig` | service_name, config_key | stream<config_key, config_value, version> | 配置变更监听（服务端流） |
| `ListConfigs` | service_name | configs[] (config_key, config_value, version) | 获取服务所有配置 |
| `GetConfigHistory` | service_name, config_key | history[] (version, config_value, updated_at, updated_by) | 配置变更历史 |

### TracingService — 链路追踪（C++，端口:51053）

请求全链路跟踪、性能分析、故障排查。

| RPC | 请求 | 响应 | 说明 |
|-----|------|------|------|
| `ReportSpan` | trace_id, span_id, parent_span_id, service_name, method_name, start_time, end_time, status, tags | success | 上报 Span |
| `QueryTrace` | trace_id | spans[] (span_id, service_name, method_name, duration, status) | 查询链路 |
| `SearchTraces` | service_name, method_name, start_time, end_time, min_duration, max_duration, status | traces[] (trace_id, root_service, duration, span_count) | 搜索链路 |
| `GetServiceMap` | Empty | services[] (service_name, dependencies[]) | 获取服务依赖拓扑图 |
| `GetSlowTraces` | service_name, min_duration, limit | traces[] (trace_id, duration, method_name) | 获取慢请求链路 |

### MonitorService — 监控告警（C++，端口:51054）

指标采集、告警规则、可视化面板。

| RPC | 请求 | 响应 | 说明 |
|-----|------|------|------|
| `ReportMetric` | service_name, metric_name, value, tags, timestamp | success | 上报指标 |
| `QueryMetric` | service_name, metric_name, start_time, end_time, aggregation | data_points[] (timestamp, value) | 查询指标 |
| `SetAlertRule` | rule_name, metric_name, condition, threshold, duration, notify_channels | rule_id, success | 设置告警规则 |
| `ListAlertRules` | service_name | rules[] (rule_id, rule_name, metric_name, condition, threshold) | 获取告警规则列表 |
| `GetAlerts` | service_name, start_time, end_time, status | alerts[] (alert_id, rule_name, metric_value, triggered_at, status) | 获取告警历史 |
| `GetServiceHealth` | service_name | status, metrics[] (metric_name, value, status) | 获取服务健康状态 |

---

## 🔌 业务微服务接口

### UserService — 用户服务（Rust，端口:50052）

| RPC | 请求 | 响应 | 说明 |
|-----|------|------|------|
| `Register` | username, password, email | user_id, token, created_at | 用户注册 |
| `Login` | username, password | token, user_info, expires_at | 用户登录 |
| `Logout` | token, device_id | success, message | 注销登录 |
| `GetUserInfo` | user_id | user_id, username, avatar, email, permission, created_at | 获取用户信息 |
| `UpdateUserInfo` | user_id, username, avatar, email | success, message | 更新用户信息 |
| `ListDevices` | user_id | devices[] | 获取登录设备列表 |
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
| `GetCategories` | Empty | categories[] | 获取分类列表 |
| `CreateCategory` | name, description | category_id, created_at | 创建分类 |
| `GetTags` | Empty | tags[] | 获取标签列表 |

### BlogService — 博客服务（Go，端口:50054）

| RPC | 请求 | 响应 | 说明 |
|-----|------|------|------|
| `CreatePost` | title, content, author_id, tags[] | post_id, created_at | 创建博客文章 |
| `GetPost` | post_id | post_id, title, content, author, tags[], created_at, updated_at, like_count, comment_count | 获取博客详情 |
| `UpdatePost` | post_id, title, content, tags[] | success, updated_at | 更新博客 |
| `DeletePost` | post_id | success, message | 删除博客 |
| `ListPosts` | page, page_size, tag_id, author_id | posts[], total | 博客列表（分页） |
| `AddComment` | post_id, author_id, content | comment_id, created_at | 添加评论 |
| `ListComments` | post_id, page, page_size | comments[], total | 评论列表 |
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
| `ProcessImage` | image_id, operations[] | image_id, url, thumbnail_url | 图片处理 |
| `GenerateThumbnail` | image_id, width, height | thumbnail_url | 生成缩略图 |

### VideoService — 视频服务（Rust，端口:50056）

| RPC | 请求 | 响应 | 说明 |
|-----|------|------|------|
| `UploadVideo` | stream<file_name, content, chunk_index, total_chunks> | video_id, status | 视频上传（客户端流） |
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
| `Search` | query, type, page, page_size, filters | results[], total, elapsed_ms | 全文搜索 |
| `IndexDocument` | document_id, type, title, content, tags[], metadata | success, indexed_at | 索引文档 |
| `RemoveIndex` | document_id, type | success, message | 移除索引 |
| `RebuildIndex` | type | job_id, status | 重建索引 |
| `GetSearchSuggestions` | prefix, type, limit | suggestions[] | 搜索建议 |
| `GetTrendingSearches` | limit | trending[] | 热门搜索 |

---

## 🚀 快速开始

### 前置依赖

| 组件 | 版本要求 | 说明 |
|------|----------|------|
| C++ 编译器 | C++17（GCC 8+ / MSVC 2019+） | 核心框架编译 |
| CMake | 3.10+ | C++ 构建系统 |
| MySQL | 8.0+ | 数据库 |
| gRPC | 1.40+ | 微服务通信框架 |
| Protobuf | 3.15+ | 序列化协议 |
| Rust | 1.70+ | UserService、ImageService、VideoService |
| Go | 1.21+ | ArticleService、BlogService、SearchService |
| Node.js | 18+ | 前端构建 |

### 1️⃣ 克隆仓库

```bash
git clone --recursive https://github.com/jyoushitou/WebServer.git
cd WebServer
git submodule update --init --recursive
```

### 2️⃣ 编译 Proto 库

```bash
cd proto/source && mkdir -p ../build && cd ../build
cmake ../source && make -j$(nproc)
```

### 3️⃣ 启动核心框架（C++）

```bash
# 启动服务注册发现
cd ServiceRegistry && mkdir build && cd build
cmake .. && cmake --build . && ./ServiceRegistry

# 启动配置中心
cd ConfigCenter && mkdir build && cd build
cmake .. && cmake --build . && ./ConfigCenter

# 启动网关
cd GRPCGateway && mkdir build && cd build
cmake .. && cmake --build . && ./GRPCGateway

# 启动链路追踪（可选）
cd TracingService && mkdir build && cd build
cmake .. && cmake --build . && ./TracingService

# 启动监控告警（可选）
cd MonitorService && mkdir build && cd build
cmake .. && cmake --build . && ./MonitorService
```

### 4️⃣ 启动业务微服务

```bash
# Rust 服务
cd UserService && cargo build --release && ./target/release/user-service
cd ImageService && cargo build --release && ./target/release/image-service
cd VideoService && cargo build --release && ./target/release/video-service

# Go 服务
cd ArticleService && go build -o article-service . && ./article-service
cd BlogService && go build -o blog-service . && ./blog-service
cd SearchService && go build -o search-service . && ./search-service
```

### 5️⃣ 启动前端

```bash
cd vue && npm install && npm run dev
```

---

## ⚙️ 配置说明

### 服务器端口

| 服务 | 语言 | 端口 | 类型 |
|------|------|------|------|
| GRPCGateway | C++ | 50051 | 核心框架 |
| ServiceRegistry | C++ | 51051 | 核心框架 |
| ConfigCenter | C++ | 51052 | 核心框架（可选） |
| TracingService | C++ | 51053 | 核心框架（可选） |
| MonitorService | C++ | 51054 | 核心框架（可选） |
| UserService | Rust | 50052 | 业务服务 |
| ArticleService | Go | 50053 | 业务服务 |
| BlogService | Go | 50054 | 业务服务 |
| ImageService | Rust | 50055 | 业务服务 |
| VideoService | Rust | 50056 | 业务服务 |
| SearchService | Go | 50057 | 业务服务 |
| Vue 前端 | JS | 60907 | 前端 |

---

## 📋 开发计划

### 阶段一：单体架构 ✅ 已归档
- [x] C++ 原生 Socket HTTP 服务器
- [x] HTTP 协议手动解析、多线程并发处理
- [x] MySQL 数据库直连、Vue 3 前端界面
- [x] 用户认证系统（Token）、异步任务处理
- [x] 归档仓库：https://github.com/jyoushitou/WebSever_cpp.git

### 阶段二：微服务架构 🔄 进行中
- [x] Proto 独立仓库搭建
- [ ] GRPCGateway 网关服务（C++）
- [ ] ServiceRegistry 服务注册发现（C++）
- [ ] ConfigCenter 配置中心（C++，可选）
- [ ] TracingService 链路追踪（C++，可选）
- [ ] MonitorService 监控告警（C++，可选）
- [ ] UserService 用户服务（Rust）
- [ ] ArticleService 文章服务（Go）
- [ ] BlogService 博客服务（Go）
- [ ] ImageService 图片服务（Rust）
- [ ] VideoService 视频服务（Rust）
- [ ] SearchService 搜索服务（Go）
- [ ] Vue 前端 gRPC-Web 接入

### 阶段三：容器化部署 📋 规划中
- [ ] Docker 容器化、Docker Compose 编排
- [ ] Kubernetes 部署、CI/CD 流水线
- [ ] 监控告警（Prometheus + Grafana）
- [ ] 链路追踪（Jaeger）

---

## 📬 联系方式

- 项目维护者：[jyoushitou]
- 邮箱：[xzt98948364@outlook.com]
- 项目地址：[https://github.com/jyoushitou/WebServer](https://github.com/jyoushitou/WebServer)
- 归档仓库：[https://github.com/jyoushitou/WebSever_cpp.git](https://github.com/jyoushitou/WebSever_cpp.git)