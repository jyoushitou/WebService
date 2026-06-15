// ============================================================
// vite.config.js — Vite 构建配置
// 功能：Vue 3 插件、@ 路径别名、开发服务器代理
// 学习重点：Vite 配置、路径别名、API 代理转发
// ============================================================

import { defineConfig } from 'vite'           // Vite 配置函数
import vue from '@vitejs/plugin-vue'           // Vue 3 官方插件

// URL 工具（将文件 URL 转为路径字符串）
import { fileURLToPath, URL } from 'node:url'

export default defineConfig({
  // ===== 插件 =====
  plugins: [vue()],

  // ===== 路径解析 =====
  resolve: {
    alias: {
      // '@' 指向 './src' 目录，这样可以在代码中用 import X from '@/xxx'
      '@': fileURLToPath(new URL('./src', import.meta.url))
    }
  },

  // ===== 开发服务器 =====
  server: {
    port: 60907,               // 开发服务器端口（默认 5173）
    proxy: {
      // API 代理：前端发往 /api/* 的请求转发到后端 C++ 服务器
      '/api': {
        target: 'http://localhost:60906',  // 后端服务器地址
        changeOrigin: true                 // 修改请求来源头，避免跨域问题
      }
    }
  }
})