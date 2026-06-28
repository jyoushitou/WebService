# WebServer - C++ gRPC 微服务框架

> **从零实现的 C++ HTTP 服务器 → gRPC 微服务框架**
>
> 用最底层的方式理解 Web 工作原理，用微服务架构承载业务扩展

![C++](https://img.shields.io/badge/C++-17-%2300599C?style=flat-square&logo=c%2B%2B)
![Vue](https://img.shields.io/badge/Vue-3-%234FC08D?style=flat-square&logo=vue.js)
![gRPC](https://img.shields.io/badge/gRPC-1.0-%234285F4?style=flat-square&logo=grpc)
![MySQL](https://img.shields.io/badge/MySQL-8-%234479A1?style=flat-square&logo=mysql)
![Protobuf](https://img.shields.io/badge/Protobuf-3.15-%23FF6C37?style=flat-square&logo=protocol-buffers)

---

## 📖 项目简介

本项目是一个**从零开始、不依赖任何第三方 Web 框架**的 C++ HTTP 服务器，逐步演进为完整的 **C++ gRPC 微服务框架**。

> **原 C++ 单体后端已归档**：[WebSever_cpp](https://github.com/jyoushitou/WebSever_cpp.git)
>
> 归档版本为纯 C++ 实现的单体 HTTP 服务器，当前仓库为 gRPC 微服务框架版本。

### 架构演进路线

```
阶段一：单体架构（已归档）
  C++ 原生 Socket HTTP 服务器 + Vue 3 前端
  └── 手动解析 HTTP 协议、多线程处理、MySQL 直连
  └── 归档仓库：https://github.com/jyoushitou/WebSever_cpp.git

阶段二：gRPC 微服务框架（进行中）
  C++ gRPC 框架核心 + 独立 Proto 仓库 + 框架组件
  └── 网关、注册中心、配置中心、链路追踪、监控告警
  └── 全部使用 C++ 实现

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
│   │   ├── registry/                   # 注册中心接口
│   │   ├── config/                     # 配置中心接口
│   │   ├── tracing/                    # 链路追踪接口
│   │   ├── monitor/                    # 监控告警接口
│   │   ├── frontend/                   # 前端服务接口
│   │   ├── CMakeLists.txt              # 编译为静态库
│   │   └── README.md
│   └── build/
│
├── GRPCGateway/                        # [子仓库] ✅ 网关 - 对外统一入口
│   └── 职责：协议转换(HTTP→gRPC)、路由分发、限流控制、鉴权验证
│
├── RegistryCenter/                     # [子仓库] ✅ 注册中心 - 服务注册发现
│   └── 职责：服务注册、健康检查、动态路由、负载均衡
│
├── ConfigCenter/                       # [子仓库] ⚠️ 配置中心 - 统一配置管理
│   └── 职责：配置存储、动态下发、版本管理、配置热更新
│
├── TracingService/                     # [子仓库] ⚠️ 链路追踪 - 请求全链路追踪
│   └── 职责：Span 收集、调用链分析、性能瓶颈定位
│
├── MonitorService/                     # [子仓库] ⚠️ 监控告警 - 系统监控
│   └── 职责：指标采集、告警规则、可视化面板、通知推送
│
├── BusinessService/                    # [子仓库] ✅ 业务微服务模板
│   └── 职责：实际业务逻辑承载（用户/文章/博客/图片/视频/搜索）
│
├── vue/                                # [子仓库] Vue 3 前端
│   └── 职责：用户界面、gRPC-Web 通信
│
├── .gitmodules
└── README.md
```

---

## 🚀 快速开始

### 前置依赖

| 组件 | 版本要求 | 说明 |
|------|----------|------|
| C++ 编译器 | C++17（GCC 8+ / MSVC 2019+） | 全部 C++ 实现 |
| CMake | 3.16+ | 构建系统 |
| gRPC | 1.40+ | 微服务通信框架 |
| Protobuf | 3.15+ | 序列化协议 |
| MySQL | 8.0+ | 数据库 |
| Redis | 6.0+ | 缓存/配置存储 |
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
cmake ../source
make -j$(nproc)
```

### 3️⃣ 编译框架组件

```bash
# 编译网关
cd GRPCGateway && mkdir build && cd build
cmake .. && cmake --build . && ./GRPCGateway

# 编译注册中心
cd RegistryCenter && mkdir build && cd build
cmake .. && cmake --build . && ./RegistryCenter

# 编译配置中心
cd ConfigCenter && mkdir build && cd build
cmake .. && cmake --build . && ./ConfigCenter

# 编译链路追踪
cd TracingService && mkdir build && cd build
cmake .. && cmake --build . && ./TracingService

# 编译监控告警
cd MonitorService && mkdir build && cd build
cmake .. && cmake --build . && ./MonitorService
```

### 4️⃣ 启动前端

```bash
cd vue && npm install && npm run dev
```

---

## 🔌 gRPC 框架核心组件

### 1. GRPCGateway — 网关（✅ 核心，端口:50051）

对外统一入口，所有客户端请求首先到达网关。

| RPC | 说明 |
|-----|------|
| `RouteRequest` | 根据 service_name 路由到后端微服务 |
| `HttpToGrpc` | HTTP/1.1 协议转换为 gRPC 调用 |
| `WebSocketToGrpc` | WebSocket 长连接转 gRPC 双向流 |
| `RateLimit` | 令牌桶限流，支持接口级别控制 |
| `Authenticate` | JWT Token 鉴权，RBAC 权限验证 |
| `CircuitBreaker` | 熔断保护，防止雪崩效应 |

**核心能力**：
- 协议转换：HTTP/WebSocket → gRPC
- 动态路由：从注册中心获取服务地址
- 限流熔断：保护后端服务
- 请求转发：负载均衡策略

### 2. RegistryCenter — 注册中心（✅ 核心，端口:50052）

服务注册与发现，动态路由和负载均衡的基础。

| RPC | 说明 |
|-----|------|
| `Register` | 服务实例注册（名称、地址、元数据） |
| `Deregister` | 服务实例注销 |
| `Heartbeat` | 心跳保活，超时自动摘除 |
| `Discover` | 服务发现，获取可用实例列表 |
| `Watch` | 服务变更监听（服务端流） |
| `GetServiceList` | 获取所有已注册服务 |

**核心能力**：
- 服务注册：启动时自动注册
- 健康检查：心跳超时自动摘除
- 动态路由：实时获取可用实例
- 负载均衡：支持轮询/随机/一致性哈希

### 3. ConfigCenter — 配置中心（⚠️ 可选，端口:50053）

统一配置管理，支持配置热更新。

| RPC | 说明 |
|-----|------|
| `GetConfig` | 获取配置项 |
| `SetConfig` | 设置配置项 |
| `DeleteConfig` | 删除配置项 |
| `WatchConfig` | 配置变更监听（服务端流） |
| `GetConfigVersion` | 获取配置版本历史 |
| `RollbackConfig` | 配置回滚到指定版本 |

**核心能力**：
- 配置存储：Key-Value 结构
- 动态下发：配置变更实时推送
- 版本管理：支持回滚
- 环境隔离：开发/测试/生产环境

### 4. TracingService — 链路追踪（⚠️ 可选，端口:50054）

请求全链路追踪，帮助排查性能瓶颈。

| RPC | 说明 |
|-----|------|
| `ReportSpan` | 上报 Span 数据 |
| `QueryTrace` | 查询调用链 |
| `GetServiceMap` | 获取服务依赖拓扑图 |
| `GetSlowTraces` | 获取慢调用链 |
| `GetErrorTraces` | 获取错误调用链 |

**核心能力**：
- Span 收集：跨服务调用链追踪
- 调用链查询：按 TraceID 查询
- 服务拓扑：自动生成依赖图
- 性能分析：慢调用/错误调用定位

### 5. MonitorService — 监控告警（⚠️ 可选，端口:50055）

系统监控和告警通知。

| RPC | 说明 |
|-----|------|
| `ReportMetric` | 上报指标数据 |
| `QueryMetric` | 查询指标数据 |
| `SetAlertRule` | 设置告警规则 |
| `GetAlerts` | 获取告警列表 |
| `AcknowledgeAlert` | 确认告警 |

**核心能力**：
- 指标采集：CPU/内存/QPS/延迟
- 告警规则：阈值/趋势/同比
- 通知推送：邮件/短信/Webhook
- 可视化：Grafana 面板集成

### 6. BusinessService — 业务微服务模板（✅ 核心，端口:50056+）

实际业务逻辑的承载者，每个业务领域独立部署。

| 业务领域 | 建议语言 | 端口 | 说明 |
|----------|----------|------|------|
| UserService | C++/Rust | 50056 | 用户注册登录、权限管理 |
| ArticleService | C++/Go | 50057 | 文章 CRUD、分类标签 |
| BlogService | C++/Go | 50058 | 博客管理、评论点赞 |
| ImageService | C++/Rust | 50059 | 图片上传处理、缩略图 |
| VideoService | C++/Rust | 50060 | 视频上传转码、流媒体 |
| SearchService | C++/Go | 50061 | 全文搜索、索引管理 |

---

## 🧠 架构设计

### 完整架构图

```
                    ┌──────────────────────────────────────┐
                    │            Vue 前端                   │
                    │         (gRPC-Web)                   │
                    └──────────────┬───────────────────────┘
                                   │ gRPC-Web
                                   ▼
                    ┌──────────────────────────────────────┐
                    │         GRPCGateway (C++)             │
                    │     协议转换 / 路由 / 限流 / 鉴权      │
                    └──┬────┬────┬────┬────┬────┬────┬────┘
                       │    │    │    │    │    │    │
              ┌────────┘    │    │    │    │    │    └────────┐
              ▼             ▼    ▼    ▼    ▼    ▼             ▼
     ┌────────────┐  ┌────────┐ ┌──────┐ ┌──────┐ ┌──────────┐
     │  Registry  │  │ Config │ │Tracing│ │Monitor│ │Business  │
     │  Center    │  │ Center │ │Service│ │Service│ │ Service  │
     │  :50052    │  │:50053  │ │:50054│ │:50055│ │ :50056+  │
     └────────────┘  └────────┘ └──────┘ └──────┘ └──────────┘
           │              │         │        │          │
           └──────────────┴─────────┴────────┴──────────┘
                                │
                                ▼
                    ┌──────────────────────┐
                    │   MySQL + Redis       │
                    │   (数据存储/缓存)      │
                    └──────────────────────┘
```

### 请求完整流程

```
用户请求 → Vue前端 → gRPC-Web → GRPCGateway
  → Gateway 调用 RegistryCenter 发现服务地址
  → Gateway 调用 ConfigCenter 获取路由规则
  → Gateway 鉴权验证 → 转发到 BusinessService
  → BusinessService 处理业务 → 返回结果
  → TracingService 记录调用链
  → MonitorService 采集指标
  → 返回响应 → Vue前端
```

### 服务注册发现流程

```
BusinessService 启动
  → 向 RegistryCenter 注册 (服务名:地址:端口)
  → 发送心跳保活 (每5秒)
  → RegistryCenter 更新服务列表

GRPCGateway 收到请求
  → 向 RegistryCenter 查询服务地址
  → RegistryCenter 返回可用实例列表
  → Gateway 负载均衡选择实例
  → 转发请求
```

### 配置中心工作流程

```
ConfigCenter 启动
  → 从 MySQL/Redis 加载配置
  → 提供 GetConfig/WatchConfig 接口

BusinessService 启动
  → 调用 GetConfig 获取初始配置
  → 调用 WatchConfig 监听配置变更
  → 配置变更时实时更新

GRPCGateway 路由
  → 从 ConfigCenter 获取路由规则
  → 动态调整路由策略
```

### 链路追踪流程

```
请求到达 GRPCGateway
  → 生成 TraceID + SpanID
  → 将 TraceContext 传递给下游

BusinessService 处理
  → 接收 TraceContext
  → 创建子 Span
  → 处理完成后上报 Span

TracingService 收集
  → 接收所有 Span 数据
  → 按 TraceID 聚合调用链
  → 提供查询接口
```

### 监控告警流程

```
MonitorService 启动
  → 定义告警规则 (QPS>1000/延迟>500ms)
  → 启动指标采集

各微服务定时上报
  → ReportMetric (CPU/内存/QPS/延迟)
  → MonitorService 存储指标

触发告警
  → MonitorService 检测到异常
  → 匹配告警规则
  → 发送通知 (邮件/短信/Webhook)
```

---

## ⚙️ 配置说明

### 服务器端口

| 服务 | 端口 | 说明 |
|------|------|------|
| Vue 前端 | 60907 | 前端开发服务器 |
| GRPCGateway | 50051 | 网关（对外统一入口） |
| RegistryCenter | 50052 | 注册中心 |
| ConfigCenter | 50053 | 配置中心 |
| TracingService | 50054 | 链路追踪 |
| MonitorService | 50055 | 监控告警 |
| BusinessService | 50056+ | 业务微服务 |

### 数据库配置

| 参数 | 默认值 | 说明 |
|------|--------|------|
| MySQL 主机 | localhost | 数据库地址 |
| MySQL 端口 | 3306 | 数据库端口 |
| MySQL 用户 | web_server | 数据库用户 |
| MySQL 密码 | 123456 | 数据库密码 |
| MySQL 数据库 | web_server | 数据库名称 |
| Redis 地址 | localhost:6379 | 缓存地址 |

---

## 📋 开发计划

### 阶段一：单体架构 ✅ 已归档
- [x] C++ 原生 Socket HTTP 服务器
- [x] HTTP 协议手动解析、多线程并发
- [x] MySQL 数据库直连、Vue 3 前端
- [x] 归档仓库：https://github.com/jyoushitou/WebSever_cpp.git

### 阶段二：gRPC 框架 🔄 进行中
- [x] Proto 独立仓库搭建
- [ ] GRPCGateway 网关（C++）
- [ ] RegistryCenter 注册中心（C++）
- [ ] ConfigCenter 配置中心（C++）
- [ ] TracingService 链路追踪（C++）
- [ ] MonitorService 监控告警（C++）
- [ ] BusinessService 业务微服务模板（C++）
- [ ] Vue 前端 gRPC-Web 接入

### 阶段三：容器化 📋 规划中
- [ ] Docker 容器化、Docker Compose 编排
- [ ] Kubernetes 部署、CI/CD 流水线
- [ ] Prometheus + Grafana 监控

---

## 📬 联系方式

- 项目地址：[https://github.com/jyoushitou/WebServer](https://github.com/jyoushitou/WebServer)
- 归档仓库：[https://github.com/jyoushitou/WebSever_cpp.git](https://github.com/jyoushitou/WebSever_cpp.git)