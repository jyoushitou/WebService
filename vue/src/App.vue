<script setup>
import { RouterLink, RouterView, useRoute } from 'vue-router'
import { useApiStore } from '@/stores/api'
import { ref, onMounted, computed } from 'vue'

const apiStore = useApiStore()
const serverStatus = ref('checking...')
const route = useRoute()

const isHome = computed(() => route.path === '/')
const isArticle = computed(() => route.path === '/article')
const isFullPage = computed(() => isHome.value || isArticle.value)

onMounted(async () => {
  apiStore.initAuth()
  try {
    const response = await apiStore.hello()
    serverStatus.value = '🟢 在线'
  } catch {
    serverStatus.value = '🔴 离线'
  }
})
</script>

<template>
  <template v-if="isFullPage">
    <RouterView />
  </template>
  <template v-else>
    <div class="app-container">
      <header class="app-header">
        <div class="header-content">
          <div class="logo">
            <h1>Native Socket Server</h1>
            <span class="subtitle">C++ 后端 · Vue 3 前端</span>
          </div>
          <div class="server-status">
            <span class="status-label">服务器状态：</span>
            <span :class="['status-indicator', serverStatus.includes('在线') ? 'online' : 'offline']">
              {{ serverStatus }}
            </span>
          </div>
        </div>
        <nav class="nav-bar">
          <RouterLink to="/" class="nav-link" active-class="active">首页</RouterLink>
          <RouterLink to="/api-demo" class="nav-link" active-class="active">API 演示</RouterLink>
          <RouterLink to="/about" class="nav-link" active-class="active">关于</RouterLink>
        </nav>
      </header>
      <main class="main-content">
        <RouterView />
      </main>
      <footer class="app-footer">
        <p>Native Socket Server &copy; 2024 | 基于 C++ 原生 Socket 和 Vue 3</p>
      </footer>
    </div>
  </template>
</template>

<style>
.app-container {
  min-height: 100vh;
  display: flex;
  flex-direction: column;
}
.app-header {
  background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
  color: white;
  padding: 0;
  box-shadow: 0 2px 10px rgba(0, 0, 0, 0.1);
}
.header-content {
  max-width: 1200px;
  margin: 0 auto;
  padding: 20px;
  display: flex;
  justify-content: space-between;
  align-items: center;
}
.logo h1 { margin: 0; font-size: 1.8rem; font-weight: 700; }
.subtitle { font-size: 0.9rem; opacity: 0.8; }
.server-status {
  background: rgba(255,255,255,0.15);
  padding: 8px 16px;
  border-radius: 20px;
  font-size: 0.9rem;
}
.status-indicator { font-weight: 600; }
.status-indicator.online { color: #4ade80; }
.status-indicator.offline { color: #f87171; }
.nav-bar {
  max-width: 1200px;
  margin: 0 auto;
  padding: 0 20px;
  display: flex;
  gap: 4px;
}
.nav-link {
  color: rgba(255,255,255,0.8);
  text-decoration: none;
  padding: 12px 20px;
  border-radius: 8px 8px 0 0;
  transition: all 0.3s ease;
  font-weight: 500;
}
.nav-link:hover { background: rgba(255,255,255,0.1); color: white; }
.nav-link.active { background: white; color: #667eea; }
.main-content {
  flex: 1;
  max-width: 1200px;
  margin: 0 auto;
  padding: 30px 20px;
  width: 100%;
}
.app-footer {
  background: #1a1a2e;
  color: rgba(255,255,255,0.7);
  text-align: center;
  padding: 20px;
  margin-top: auto;
  font-size: 0.9rem;
}
</style>