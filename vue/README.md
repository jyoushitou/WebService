# Native Socket Server - Vue 3 Frontend

基于 C++ 原生 Socket 的高性能 HTTP 服务器前端界面。

## 技术栈

- **Vue 3** - 渐进式 JavaScript 框架
- **Vue Router** - 官方路由管理
- **Pinia** - 状态管理库
- **Vite** - 构建工具
- **Axios** - HTTP 客户端

## 快速开始

### 1. 启动后端服务器

```bash
# 在 backend 目录下编译并运行
cd backend
g++ -std=c++17 main.cpp -o server -lpthread
./server
```

### 2. 启动前端开发服务器

```bash
# 在 frontend 目录下
cd frontend
npm install
npm run dev
```

### 3. 构建生产版本

```bash
npm run build
```

构建后的文件将输出到 `dist` 目录，可以直接由后端服务器提供静态文件服务。

## 项目结构

```
frontend/
├── public/
│   └── vite.svg          # 网站图标
├── src/
│   ├── assets/
│   │   └── main.css      # 全局样式
│   ├── router/
│   │   └── index.js      # 路由配置
│   ├── stores/
│   │   └── api.js        # API 状态管理
│   ├── views/
│   │   ├── HomeView.vue  # 首页
│   │   ├── ApiDemoView.vue # API 演示页
│   │   └── AboutView.vue # 关于页面
│   ├── App.vue           # 根组件
│   └── main.js           # 入口文件
├── index.html            # HTML 模板
├── vite.config.js        # Vite 配置
└── package.json          # 依赖配置
```

## API 接口

| 方法 | 路径 | 描述 |
|------|------|------|
| GET | /api/hello | 获取问候消息和工作进程 PID |
| POST | /api/data | 提交 name、age、email 数据（由子进程处理） |
| PUT | /api/update | 更新指定 ID 的记录 |
| DELETE | /api/delete/:id | 删除指定 ID 的记录 |

## 开发说明

- 前端开发服务器运行在 `http://localhost:3000`
- API 请求通过 Vite 代理转发到后端 `http://localhost:8080`
- 生产构建后，后端服务器可直接提供静态文件服务