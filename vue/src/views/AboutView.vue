<script setup>
</script>

<template>
  <div class="about">
    <h2>关于本项目</h2>

    <div class="about-content">
      <section class="about-section">
        <h3>📖 项目简介</h3>
        <p>
          本项目是一个基于 C++ 原生 Socket 实现的高性能 HTTP 服务器，
          搭配 Vue 3 现代前端框架构建的全栈 Web 应用示例。
          展示了如何使用底层网络编程构建 Web 服务，同时保持前端开发的现代化体验。
        </p>
      </section>

      <section class="about-section">
        <h3>🎯 技术特点</h3>
        <div class="feature-grid">
          <div class="feature-card">
            <h4>🔧 后端技术</h4>
            <ul>
              <li>C++ 原生 Socket 编程</li>
              <li>多线程并发处理</li>
              <li>HTTP 协议解析</li>
              <li>静态文件服务</li>
              <li>跨平台兼容 (Windows/Linux)</li>
            </ul>
          </div>

          <div class="feature-card">
            <h4>🎨 前端技术</h4>
            <ul>
              <li>Vue 3 Composition API</li>
              <li>Vue Router 路由管理</li>
              <li>Pinia 状态管理</li>
              <li>Vite 构建工具</li>
              <li>Axios HTTP 客户端</li>
            </ul>
          </div>
        </div>
      </section>

      <section class="about-section">
        <h3>🚀 快速开始</h3>
        <div class="code-block">
          <h4>启动后端服务器：</h4>
          <pre><code># 编译并运行 C++ 服务器
g++ -std=c++17 main.cpp -o server -lpthread
./server</code></pre>
        </div>
        <div class="code-block">
          <h4>启动前端开发服务器：</h4>
          <pre><code># 安装依赖并启动
cd frontend
npm install
npm run dev</code></pre>
        </div>
      </section>

      <section class="about-section">
        <h3>📡 API 接口</h3>
        <div class="api-table">
          <table>
            <thead>
              <tr>
                <th>方法</th>
                <th>路径</th>
                <th>描述</th>
              </tr>
            </thead>
            <tbody>
              <tr>
                <td><span class="method get">GET</span></td>
                <td><code>/api/hello</code></td>
                <td>获取问候消息和工作进程 PID</td>
              </tr>
              <tr>
                <td><span class="method post">POST</span></td>
                <td><code>/api/login</code></td>
                <td>登录鉴权（提交 name 和 password）</td>
              </tr>
              <tr>
                <td><span class="method post">POST</span></td>
                <td><code>/api/data</code></td>
                <td>提交 name、age、email 数据</td>
              </tr>
              <tr>
                <td><span class="method put">PUT</span></td>
                <td><code>/api/update</code></td>
                <td>更新指定 ID 的记录</td>
              </tr>
              <tr>
                <td><span class="method delete">DELETE</span></td>
                <td><code>/api/delete/:id</code></td>
                <td>删除指定 ID 的记录</td>
              </tr>
            </tbody>
          </table>
        </div>
      </section>
    </div>
  </div>
</template>

<style scoped>
.about {
  animation: fadeIn 0.5s ease;
}

@keyframes fadeIn {
  from { opacity: 0; transform: translateY(20px); }
  to { opacity: 1; transform: translateY(0); }
}

h2 {
  color: #1a1a2e;
  font-size: 2rem;
  margin-bottom: 32px;
}

.about-content {
  display: flex;
  flex-direction: column;
  gap: 32px;
}

.about-section {
  background: white;
  border-radius: 12px;
  padding: 24px;
  box-shadow: 0 4px 6px rgba(0, 0, 0, 0.07);
}

.about-section h3 {
  color: #1a1a2e;
  font-size: 1.3rem;
  margin-bottom: 16px;
}

.about-section p {
  color: #555;
  line-height: 1.8;
  font-size: 1rem;
}

.feature-grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(280px, 1fr));
  gap: 20px;
}

.feature-card {
  background: #f8f9fa;
  border-radius: 8px;
  padding: 20px;
}

.feature-card h4 {
  color: #667eea;
  margin-bottom: 12px;
  font-size: 1.1rem;
}

.feature-card ul {
  list-style: none;
  padding: 0;
}

.feature-card li {
  padding: 6px 0;
  color: #555;
  position: relative;
  padding-left: 20px;
}

.feature-card li::before {
  content: '→';
  position: absolute;
  left: 0;
  color: #667eea;
}

.code-block {
  background: #1a1a2e;
  border-radius: 8px;
  padding: 16px;
  margin-bottom: 16px;
}

.code-block h4 {
  color: #94a3b8;
  margin-bottom: 12px;
  font-size: 0.9rem;
}

.code-block pre {
  margin: 0;
}

.code-block code {
  color: #4ade80;
  font-family: 'Fira Code', monospace;
  font-size: 0.9rem;
  line-height: 1.6;
}

.api-table {
  overflow-x: auto;
}

table {
  width: 100%;
  border-collapse: collapse;
}

th, td {
  padding: 12px 16px;
  text-align: left;
  border-bottom: 1px solid #e0e0e0;
}

th {
  background: #f8f9fa;
  color: #1a1a2e;
  font-weight: 600;
}

td {
  color: #555;
}

.method {
  padding: 4px 8px;
  border-radius: 4px;
  font-size: 0.85rem;
  font-weight: 600;
}

.method.get {
  background: #dbeafe;
  color: #2563eb;
}

.method.post {
  background: #fef3c7;
  color: #d97706;
}

.method.put {
  background: #dbeafe;
  color: #2563eb;
}

.method.delete {
  background: #fce4ec;
  color: #dc2626;
}

td code {
  background: #f1f5f9;
  padding: 2px 6px;
  border-radius: 4px;
  font-size: 0.9rem;
  color: #667eea;
}
</style>