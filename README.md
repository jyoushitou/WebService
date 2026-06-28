# Native Socket Server

> **基于 C++ 原生 Socket 的高性能 HTTP 全栈 Web 服务**

后端使用 C++ 底层 Socket API 从头实现 HTTP 服务器，搭配 Vue 3 现代前端框架，展示原生网络编程与全栈开发的完整实践。

![C++](https://img.shields.io/badge/C++-17-%2300599C?style=flat-square&logo=c%2B%2B)
![Vue](https://img.shields.io/badge/Vue-3-%234FC08D?style=flat-square&logo=vue.js)
![Vite](https://img.shields.io/badge/Vite-5-%23646CFF?style=flat-square&logo=vite)
![MySQL](https://img.shields.io/badge/MySQL-8-%234479A1?style=flat-square&logo=mysql)

---

## 📖 项目简介

本项目是一个从零开始、**不依赖任何第三方 Web 框架**的 C++ HTTP 服务器，搭配 Vue 3 前端构成完整的全栈应用。

**核心理念**：用最底层的方式理解 Web 工作原理——从 TCP Socket 连接到 HTTP 协议解析，从多线程并发到任务队列调度，完整呈现一个 Web 服务器是如何炼成的。

### ✨ 技术亮点

| 层面 | 亮点 |
|------|------|
| **后端** | C++ 原生 Socket、HTTP 协议手动解析、多线程并发、任务队列、Token 鉴权、跨平台 |
| **前端** | Vue 3 Composition API、Pinia 状态管理、Vue Router、Axios 封装、响应式动画 |
| **数据库** | MySQL C API 封装、用户鉴权、多设备会话管理 |
| **架构** | 每用户独立线程、异步任务队列、多设备登录管理、前后端分离 |

---

## 🏗️ 项目结构

```
WebServer/
├── cpp/                               # C++ 后端
│   ├── CMakeLists.txt                 # CMake 构建配置（C++17）
│   └── source/
│       ├── header/                    # 头文件
│       │   ├── FarmeWork.h            # 核心框架：全局变量、结构体、函数声明
│       │   ├── https_api.h            # HTTP 服务器：请求/响应、路由、Socket
│       │   ├── MyMySQL.h              # MySQL 数据库操作封装
│       │   ├── Task.h                 # 任务系统：任务类型/状态/线程管理
│       │   └── Tools.h                # 通用工具：日志、MIME、文件操作 + Tools::Json + Tools::Auth
│       └── body/                      # 源文件
│           ├── main.cpp               # 程序入口
│           ├── FrameWork.cpp          # 核心业务（路由、任系统、用户线程）
│           ├── https_api.cpp          # HTTP 服务器实现
│           ├── mysql.cpp              # MySQL 连接与操作
│           ├── Task.cpp               # 任务系统实现
│           └── Tools.cpp              # 工具函数 + Tools::Json + Tools::Auth 实现
│
└── vue/                               # Vue 3 前端
    ├── vite.config.js                 # Vite 配置
    ├── package.json                   # 依赖管理
    ├── index.html                     # HTML 模板
    └── src/
        ├── main.js                    # 应用入口
        ├── App.vue                    # 根组件
        ├── router/index.js            # 路由配置
        ├── stores/api.js              # Pinia 状态管理 & Axios 封装
        ├── components/
        │   └── UserNav.vue            # 用户导航组件（登录/注册弹窗）
        └── views/
            ├── HomeView.vue           # 首页（开屏动画、内容展示）
            ├── ArticleView.vue        # 文章阅读页
            ├── ApiDemoView.vue        # API 接口演示页
            └── AboutView.vue          # 关于页面
```

---

## 🚀 快速开始

### 前置依赖

**后端**
- C++ 编译器（支持 C++17：GCC 8+ / MSVC 2019+）
- CMake 3.10+
- MySQL 8.0+（以及 `libmysqlclient-dev`）
- pthread（Linux 平台）

**前端**
- Node.js 18+
- npm 9+

### 1️⃣ 编译并启动后端

```bash
# 使用 CMake 构建（推荐）
cd cpp
mkdir build && cd build
cmake ..
cmake --build .

# 运行服务器
./WebServer
```

或者 Linux 上快速编译测试：

```bash
cd cpp/source
g++ -std=c++17 body/*.cpp -I header -lpthread -lmysqlclient -o server
./server
```

服务器默认监听端口 **60906**，绑定所有网络接口（`0.0.0.0`），允许任意 IP 通过该端口访问 API。

> **跨平台说明**：Windows 上数据库连接本地的 `localhost`，Linux 上自动连接远程 `192.168.0.52` 的 MySQL 服务器。

### 2️⃣ 启动前端开发服务器

```bash
cd vue
npm install
npm run dev
```

前端开发服务器运行在 **http://localhost:60907**，Vite 自动将 `/api/*` 请求代理到后端。

### 3️⃣ 构建前端生产版本

```bash
cd vue
npm run build
```

构建产物在 `vue/dist/`，后端 C++ 服务器可自动查找并服务静态文件。

---

## 🔌 API 接口

所有 API 通过 `/api` 前缀访问，请求/响应格式为 JSON。

### 公共接口（无需登录）

| 方法 | 路径 | 说明 |
|------|------|------|
| `GET` | `/api/hello` | 获取服务问候消息和工作进程 PID |
| `GET` | `/api/contents` | 获取首页内容板块数据 |
| `GET` | `/api/article` | 获取文章章节内容 |
| `POST` | `/api/register` | 用户注册（提交 `name`, `password`） |

### 鉴权接口（需登录）

| 方法 | 路径 | 说明 |
|------|------|------|
| `POST` | `/api/login` | 登录（提交 `name`, `password`），返回 Token |
| `POST` | `/api/logout` | 注销当前设备登录 |
| `GET` | `/api/userinfo` | 获取当前用户信息 |
| `GET` | `/api/devices` | 获取该用户所有已登录设备列表 |

### 业务接口（需 Token）

| 方法 | 路径 | 说明 |
|------|------|------|
| `POST` | `/api/data` | 提交数据处理任务（异步，由用户专属线程处理） |
| `PUT` | `/api/update` | 提交数据更新任务 |
| `DELETE` | `/api/delete` | 提交数据删除任务 |
| `POST` | `/api/task/result` | 查询异步任务执行结果（传 `task_id`） |

> **鉴权方式**：请求头中添加 `Authorization: Bearer <token>`，前端 Axios 拦截器自动注入。

---

## 🧠 架构设计

### 后端架构

```
main()
  ├── Initiate_MySQL()     → 创建全局 MySQL 连接
  └── Initiate_Http()      → 启动 HTTP 服务器
        └── fork() / thread()
              └── Http_Server_Routine()
                    ├── 注册 API 路由
                    ├── 监听端口，接受连接
                    └── 每个客户端新线程处理
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

### 各模块说明

| 模块 | 文件 | 说明 |
|------|------|------|
| **HTTP 服务器** | `https_api.cpp` | 完整 HTTP/1.1 实现：请求解析、响应构建、静态文件服务、CORS |
| **RPC 服务器** | `RPCServer.cpp` | 基于 Boost.Beast WebSocket 的 JSON-RPC 服务端 |
| **任务框架** | `UserThread.cpp` + `Task.cpp` | 用户线程管理、任务投递/查询、异步处理、心跳 |
| **数据库** | `mysql.cpp` | MySQL C API 封装：连接管理、用户鉴权 |
| **通用工具** | `Utils.cpp` | 日志输出、文件读取、MIME 类型、JSON 解析（Utils::Json）、Token 认证（Utils::Auth） |

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

---

## ⚙️ 配置说明

### 后端数据库（`cpp/source/header/MyMySQL.h`）

数据库配置通过条件编译自动适配平台：

| 平台 | MySQL 头文件路径 | 数据库服务器地址 |
|------|-----------------|-----------------|
| **Windows**（本地开发） | `#include <mysql.h>` | `host = "localhost"` |
| **Linux**（部署） | `#include <mysql/mysql.h>` | `host = "192.168.0.52"` |

| 参数 | 默认值 | 说明 |
|------|--------|------|
| `port` | `3306` | MySQL 端口 |
| `user` | `web_server` | 数据库用户名 |
| `password` | `123456` | 数据库密码 |
| `database` | `web_server` | 数据库名称 |

### HTTP 服务器端口

| 位置 | 代码 | 说明 |
|------|------|------|
| `main.cpp:Initiate_Http(60906)` | 后端监听端口 | 服务器绑定所有网络接口（`INADDR_ANY`），允许各 IP 访问 |

### 前端代理（`vue/vite.config.js`）

| 参数 | 默认值 | 说明 |
|------|--------|------|
| `server.port` | `60907` | 前端开发服务器端口 |
| 代理目标 | `http://localhost:60906` | API 后端地址 |

### 数据库初始化

需要 MySQL 数据库中有 `users` 表：

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

## 🎨 前端特性

- **开屏动画**：首页加载时展示品牌标题动画，内容级联弹入
- **滚动动画**：基于 `IntersectionObserver` 的入场/退场效果
- **登录/注册弹窗**：Teleport 到 body，避免父容器影响定位
- **多设备管理**：同一账户可多设备登录，独立退出
- **服务器状态检测**：导航栏实时显示后端连通状态
- **响应式设计**：适配桌面与移动端

---

## 🧪 开发说明

- 后端日志分级输出：`[输出]` `[MySQL]` `[HTTP]` `[错误]`
- 前端 Axios 请求拦截器自动注入 Token
- 生产构建后，后端服务器自动查找并托管静态文件
- 支持 Windows（线程）和 Linux（fork）两种后端启动方式

---
## 📜 许可证

[MIT License](LICENSE)

---

> **用原生代码理解 Web 的本质，用现代框架提升开发的体验。**

- 后端日志分级输出：`[输出]` `[MySQL]` `[HTTP]` `[错误]`
- 前端 Axios 请求拦截器自动注入 Token
- 生产构建后，后端服务器自动查找并托管静态文件
- 支持 Windows（线程）和 Linux（fork）两种后端启动方式

---
## 📜 许可证

[MIT License](LICENSE)

---

> **用原生代码理解 Web 的本质，用现代框架提升开发的体验。**