# WebServer - C++ 原生全栈 Web 服务

> **从零实现的 C++ HTTP 服务器 → gRPC 微服务架构演进**
>
> 用最底层的方式理解 Web 工作原理，用微服务架构承载业务扩展

![C++](https://img.shields.io/badge/C++-17-%2300599C?style=flat-square&logo=c%2B%2B)
![Vue](https://img.shields.io/badge/Vue-3-%234FC08D?style=flat-square&logo=vue.js)
![gRPC](https://img.shields.io/badge/gRPC-1.0-%234285F4?style=flat-square&logo=grpc)
![MySQL](https://img.shields.io/badge/MySQL-8-%234479A1?style=flat-square&logo=mysql)
![Protobuf](https://img.shields.io/badge/Protobuf-3.15-%23FF6C37?style=flat-square&logo=protocol-buffers)

---

## 📖 项目简介

本项目是一个**从零开始、不依赖任何第三方 Web 框架**的 C++ HTTP 服务器，逐步演进为完整的 gRPC 微服务架构。

### 架构演进路线

```
阶段一：单体架构（已完成）
  C++ 原生 Socket HTTP 服务器 + Vue 3 前端
  └── 手动解析 HTTP 协议、多线程处理、MySQL 直连

阶段二：微服务架构（进行中）
  gRPC 微服务 + 独立 Proto 仓库 + 服务拆分
  └── 网关统一入口、业务解耦、独立部署

阶段三：容器化部署（规划中）
  Docker + Kubernetes + CI/CD
  └── 弹性伸缩、服务治理、自动化运维
```

### 核心理念

从 TCP Socket 连接到 HTTP 协议解析，从多线程并发到 gRPC 微服务通信，完整呈现一个 Web 服务器是如何炼成的。每个组件都亲手实现，深入理解每一层的工作原理。

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
│   │   ├── service/                    # 业务服务接口
│   │   │   └── business_service.proto  # 业务处理、缓存、编排
│   │   ├── mysql/                      # 数据库服务接口
│   │   │   └── db_service.proto        # CRUD、事务、连接池
│   │   ├── cert/                       # 证书服务接口
│   │   │   └── cert_service.proto      # SSL 分发、续期、验证
│   │   ├── frontend/                   # 前端服务接口
│   │   │   └── frontend_service.proto  # 页面数据、事件推送、文件传输
│   │   ├── CMakeLists.txt              # 编译为静态库 librpc_protos.a
│   │   └── README.md                   # Proto 仓库使用说明
│   └── build/                          # 编译输出目录
│       ├── lib/                        # 编译后的库文件
│       ├── bin/                        # 可执行文件
│       └── generated/                  # 生成的 .pb.h / .pb.cc
│
├── GRPCGateway/                        # [子仓库] gRPC 网关
│   └── 职责：协议转换、路由分发、限流控制、鉴权验证
│
├── Service/                            # [子仓库] 业务服务
│   └── 职责：核心业务逻辑、缓存操作、业务编排
│
├── MySQL/                              # [子仓库] 数据库服务
│   └── 职责：CRUD 操作、事务管理、连接池管理
│
├── CertService/                        # [子仓库] 证书服务
│   └── 职责：SSL 证书分发、续期、验证、吊销、内网同步
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
| C++ 编译器 | C++17（GCC 8+ / MSVC 2019+） | 后端编译 |
| CMake | 3.10+ | 构建系统 |
| MySQL | 8.0+（含 `libmysqlclient-dev`） | 数据库 |
| gRPC | 1.40+ | 微服务通信框架 |
| Protobuf | 3.15+ | 序列化协议 |
| Node.js | 18+ | 前端构建 |
| npm | 9+ | 前端包管理 |

### 1️⃣ 克隆仓库（含子模块）

```bash
git clone --recursive https://github.com/your/WebServer.git
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

#### 编译 GRPCGateway（网关）

```bash
cd GRPCGateway
mkdir build && cd build
cmake ..
cmake --build .
./GRPCGateway
```

#### 编译 Service（业务服务）

```bash
cd Service
mkdir build && cd build
cmake ..
cmake --build .
./Service
```

#### 编译 MySQL（数据库服务）

```bash
cd MySQL
mkdir build && cd build
cmake ..
cmake --build .
./MySQL
```

#### 编译 CertService（证书服务）

```bash
cd CertService
mkdir build && cd build
cmake ..
cmake --build .
./CertService
```

### 4️⃣ 启动前端

```bash
cd vue
npm install
npm run dev
```

前端开发服务器运行在 **http://localhost:60907**。

---

## 🔌 API 接口

### 单体架构 API（当前阶段）

所有 API 通过 `/api` 前缀访问，请求/响应格式为 JSON。

| 方法 | 路径 | 鉴权 | 说明 |
|------|------|------|------|
| `GET` | `/api/hello` | ❌ | 服务问候消息 |
| `GET` | `/api/contents` | ❌ | 首页内容板块 |
| `GET` | `/api/article` | ❌ | 文章章节内容 |
| `POST` | `/api/register` | ❌ | 用户注册 |
| `POST` | `/api/login` | ❌ | 登录，返回 Token |
| `POST` | `/api/logout` | ✅ | 注销当前设备 |
| `GET` | `/api/userinfo` | ✅ | 当前用户信息 |
| `GET` | `/api/devices` | ✅ | 已登录设备列表 |
| `POST` | `/api/data` | ✅ | 提交数据处理任务 |
| `PUT` | `/api/update` | ✅ | 提交数据更新任务 |
| `DELETE` | `/api/delete` | ✅ | 提交数据删除任务 |
| `POST` | `/api/task/result` | ✅ | 查询异步任务结果 |

> **鉴权方式**：`Authorization: Bearer <token>`

### gRPC 微服务接口

#### GatewayService — 网关服务（端口:50051）

| RPC | 请求 | 响应 | 说明 |
|-----|------|------|------|
| `RouteRequest` | service_name, method_name, payload, metadata | status_code, data, error_message | 路由请求到后端微服务 |
| `HttpToGrpc` | method, path, headers, body, query_params | status_code, headers, body | HTTP 协议转 gRPC |
| `RateLimit` | client_ip, api_path, request_count | allowed, remaining_quota, reset_time_seconds | 限流控制 |
| `Authenticate` | token, service_name, method_name | authenticated, user_id, permissions | 鉴权验证 |

#### BusinessService — 业务服务（端口:50052）

| RPC | 请求 | 响应 | 说明 |
|-----|------|------|------|
| `ProcessBusiness` | business_type, action, data, context | success, result, error_message | 处理业务请求 |
| `Orchestrate` | workflow_id, steps, input_data | workflow_instance_id, status, output_data | 业务编排 |
| `CacheOperation` | operation(GET/SET/DELETE/EXISTS), key, value, ttl_seconds | found, value | 缓存操作 |
| `HealthCheck` | Empty | code, message | 健康检查 |

#### DatabaseService — 数据库服务（端口:50053）

| RPC | 请求 | 响应 | 说明 |
|-----|------|------|------|
| `Query` | sql, params, transaction_id, timeout_seconds | rows, affected_rows, elapsed_seconds | 查询操作 |
| `Execute` | sql, params, transaction_id | affected_rows, last_insert_id | 执行操作（INSERT/UPDATE/DELETE） |
| `BatchExecute` | requests[] | responses[] | 批量操作 |
| `BeginTransaction` | Empty | transaction_id, success | 开启事务 |
| `Commit` | transaction_id | Empty | 提交事务 |
| `Rollback` | transaction_id | Empty | 回滚事务 |
| `GetConnection` | Empty | connection_id, database_url | 获取连接池连接 |
| `ReleaseConnection` | connection_id | Empty | 释放连接 |
| `HealthCheck` | Empty | code, message | 健康检查 |

#### CertService — 证书服务（端口:50054）

| RPC | 请求 | 响应 | 说明 |
|-----|------|------|------|
| `DistributeCert` | domain, cert_type(RSA/ECC), validity_days, subject_info | cert_id, certificate_pem, private_key_pem, ca_certificate_pem, expires_at | 分发 SSL 证书 |
| `RenewCert` | cert_id, domain | cert_id, certificate_pem, private_key_pem, new_expires_at | 证书续期 |
| `VerifyCert` | certificate_pem, domain | valid, error_message, expires_at, issuer | 证书验证 |
| `SyncInternalCert` | target_server, cert_ids[], force_sync | success, synced_cert_ids[], failed_cert_ids[] | 内网证书同步 |
| `RevokeCert` | cert_id, reason | code, message | 证书吊销 |
| `HealthCheck` | Empty | code, message | 健康检查 |

#### FrontendService — 前端服务

| RPC | 请求 | 响应 | 说明 |
|-----|------|------|------|
| `GetPageData` | page_name, params, pagination | page_name, data, pagination | 获取页面渲染数据 |
| `UserAction` | action_type, action_data, session_id | success, result, message | 用户交互操作 |
| `SubscribeEvents` | user_id, event_types[] | stream<event_type, data, timestamp> | 实时事件推送（服务端流） |
| `UploadFile` | stream<file_name, content, chunk_index, total_chunks, file_id> | file_id, url, size_bytes | 文件上传（客户端流） |
| `DownloadFile` | file_id, file_name | stream<file_name, content, chunk_index, total_chunks, file_id> | 文件下载（服务端流） |

---

## 🧠 架构设计

### 当前架构（单体）

```
main()
  ├── Initiate_MySQL()     → 全局 MySQL 连接
  └── Initiate_Http()      → HTTP 服务器
        └── Http_Server_Routine()
              ├── 注册 API 路由
              ├── 监听端口，接受连接
              └── 每客户端新线程处理
```

### 目标架构（微服务）

```
                    ┌─────────────────┐
                    │    Vue 前端      │ (2核2g 服务器)
                    │  (gRPC-Web)     │
                    └────────┬────────┘
                             │ gRPC-Web (通过 Nginx 代理)
                             ▼
                    ┌─────────────────┐
                    │  GRPCGateway     │
                    │  (协议转换/路由)  │
                    └──┬────┬────┬────┘
                       │    │    │
              ┌────────┘    │    └────────┐
              ▼             ▼             ▼
     ┌────────────┐ ┌────────────┐ ┌────────────┐
     │  Service   │ │ CertService│ │   MySQL    │
     │ (业务逻辑)  │ │(证书分发)   │ │ (数据存储)  │
     └────────────┘ └────────────┘ └────────────┘
              │             │
              └─────────────┘
                    │
                    ▼
              ┌────────────┐
              │   MySQL    │
              │ (数据库)    │
              └────────────┘
```

### 微服务间调用关系

```
用户请求 → Vue前端 → gRPC-Web → GRPCGateway
    → 路由到 Service → Service 调用 MySQL 获取数据
    → 返回数据 → GRPCGateway → Vue前端

证书续期请求 → Vue前端 → gRPC-Web → GRPCGateway
    → 路由到 CertService → CertService 生成新证书
    → 返回证书 → GRPCGateway → Vue前端
```

### 部署架构

```
┌──────────────────────────────────────────────┐
│ 服务器 A (5核10g)                            │
│  ├── GRPCGateway     (端口:50051)            │
│  ├── Service         (端口:50052)            │
│  ├── MySQL           (端口:50053)            │
│  └── CertService     (端口:50054)            │
└──────────────────┬───────────────────────────┘
                   │ gRPC 内部通信
                   ▼
┌──────────────────────────────────────────────┐
│ 服务器 B (2核2g)                             │
│  └── Vue 前端 + Nginx (gRPC-Web 代理)        │
└──────────────────────────────────────────────┘
```

### 核心设计：每用户独立线程

```
用户 A 登录
  └── Create_User_Thread()   → 为用户 A 创建专属线程
        └── User_Worker_Routine()
              ├── 独立 MySQL 连接
              ├── 任务队列（等待处理）
              ├── 心跳检测（每30秒）
              └── 处理异步任务

用户 B 登录 → 同理创建独立线程
```

### HTTP 请求处理流程

```
socket()              → 创建 TCP Socket
bind()                → 绑定 IP:Port
listen()              → 开始监听
accept() 循环          → 接受新连接
  └── Handle_Client() → 读取原始 HTTP 请求
        └── Request_Parse() → 手动解析 HTTP 协议
              ├── 请求行（GET /api/hello HTTP/1.1）
              ├── 请求头
              └── 请求体（JSON）
        └── Default_Request_Handle() → 路由匹配 + 静态文件
        └── Response_Build() → 构建 HTTP 响应报文
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
│   ├── service/               # 业务接口
│   │   └── business_service.proto
│   ├── mysql/                 # 数据库接口
│   │   └── db_service.proto
│   ├── cert/                  # 证书接口
│   │   └── cert_service.proto
│   └── frontend/              # 前端接口
│       └── frontend_service.proto
└── build/                     # 编译输出
    ├── lib/                   # 编译后的库文件
    ├── bin/                   # 可执行文件
    └── generated/             # 生成的 .pb.h / .pb.cc

编译为静态库 librpc_protos.a
其他微服务通过 find_package 引用
```

**优势**：
- **类型安全**：所有微服务使用同一份 proto 定义，接口变更编译期即可发现
- **版本控制**：proto 版本变更可追溯，支持多版本共存
- **语言无关**：可生成 C++/Java/Python/Go 等代码，支持多语言微服务
- **编译隔离**：修改 proto 只需重新编译依赖库，无需修改业务代码
- **可移植性**：新微服务直接引用 proto 库，开箱即用

### Git Submodule 管理

```bash
# 总仓库添加子仓库
git submodule add <proto-repo-url> proto
git submodule add <gateway-repo-url> GRPCGateway
git submodule add <service-repo-url> Service
git submodule add <mysql-repo-url> MySQL
git submodule add <cert-repo-url> CertService
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

| 服务 | 端口 | 说明 |
|------|------|------|
| HTTP 服务器 | `60906` | 后端 API |
| Vue 开发服务器 | `60907` | 前端开发 |
| GRPCGateway | `50051` | gRPC 网关 |
| Service | `50052` | 业务服务 |
| MySQL | `50053` | 数据库服务 |
| CertService | `50054` | 证书服务 |

### 数据库初始化

```sql
CREATE DATABASE IF NOT EXISTS web_server;
USE web_server;

CREATE TABLE IF NOT EXISTS users (
    id INT AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(50) UNIQUE NOT NULL,
    password VARCHAR(255) NOT NULL,
    permission INT DEFAULT 1,
    avatar VARCHAR(255) DEFAULT '',
    login_count INT DEFAULT 0,
    last_login_time DATETIME,
    last_login_ip VARCHAR(45),
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP
);
```

---

## 📋 开发计划

### 阶段一：单体架构 ✅ 已完成
- [x] C++ 原生 Socket HTTP 服务器
- [x] HTTP 协议手动解析
- [x] 多线程并发处理
- [x] MySQL 数据库直连
- [x] Vue 3 前端界面
- [x] 用户认证系统（Token）
- [x] 异步任务处理

### 阶段二：微服务架构 🔄 进行中
- [x] Proto 独立仓库搭建
- [ ] GRPCGateway 网关服务实现
- [ ] Service 业务服务实现
- [ ] MySQL 数据库服务实现
- [ ] CertService 证书服务实现
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

- 项目维护者：[Your Name]
- 邮箱：[your.email@example.com]
- 项目地址：[https://github.com/your/WebServer](https://github.com/your/WebServer)