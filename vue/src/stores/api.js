/**
 * stores/api.js — API 请求封装和全局状态管理
 * 
 * 基于 axios 封装 HTTP 请求，使用 Pinia 管理全局状态。
 * 请求拦截器自动注入 Authorization 头，实现登录后自动鉴权。
 * 登录状态持久化到 localStorage，刷新页面不丢失。
 */

import { defineStore } from 'pinia'
import axios from 'axios'

// 创建 axios 实例，配置基础 URL 和超时时间
const api = axios.create({
  baseURL: '/api',
  timeout: 5000,
  headers: {
    'Content-Type': 'application/json'
  }
})

/**
 * axios 请求拦截器
 * 每次请求自动检查 localStorage 中是否有 token，
 * 如果有则添加到 Authorization 请求头，格式为 Bearer <token>。
 */
api.interceptors.request.use(config => {
  const token = localStorage.getItem('token')
  if (token) {
    config.headers.Authorization = `Bearer ${token}`
  }
  return config
})

/**
 * useApiStore — 全局 API 状态管理
 * 
 * 管理所有后端 API 调用的状态和数据，包括：
 * - 各个接口的返回数据
 * - 加载状态和错误信息
 * - 用户登录状态
 */
export const useApiStore = defineStore('api', {
  state: () => ({
    helloMessage: '',
    serverTime: '',
    lastPostData: null,
    lastUpdateData: null,
    lastDeleteData: null,
    loading: false,
    error: null,
    user: null,           // 当前登录用户信息 { user_id, level, name }
    isLoggedIn: false     // 是否已登录
  }),

  actions: {
    /**
     * initAuth — 应用启动时从 localStorage 恢复登录状态
     * 在 App.vue 的 onMounted 中调用，刷新页面后自动登录。
     */
    initAuth() {
      const token = localStorage.getItem('token')
      const savedUser = localStorage.getItem('user')
      if (token && savedUser) {
        try {
          this.user = JSON.parse(savedUser)
          this.isLoggedIn = true
          console.log('[Auth] 从 localStorage 恢复登录状态:', this.user.name)
        } catch (e) {
          localStorage.removeItem('token')
          localStorage.removeItem('user')
        }
      }
    },

    /**
     * hello — GET /api/hello
     * 向服务器发送问候请求，验证后端服务是否在线。
     */
    async hello() {
      this.loading = true
      this.error = null
      try {
        const response = await api.get('/hello')
        this.helloMessage = response.data.message
        return response.data
      } catch (err) {
        this.error = err.message
        throw err
      } finally {
        this.loading = false
      }
    },

    /**
     * getTime — 获取服务器时间
     * 实际上调用 /api/hello 接口，用返回的 path 字段作为时间展示。
     */
    async getTime() {
      this.loading = true
      this.error = null
      try {
        const response = await api.get('/hello')
        this.serverTime = response.data.path || new Date().toLocaleString()
        return response.data
      } catch (err) {
        this.error = err.message
        throw err
      } finally {
        this.loading = false
      }
    },

    /**
     * postData — POST /api/data
     * 向服务器提交数据（如 name, age, email），由后端异步处理。
     */
    async postData(data) {
      this.loading = true
      this.error = null
      try {
        const response = await api.post('/data', data)
        this.lastPostData = response.data
        return response.data
      } catch (err) {
        this.error = err.message
        throw err
      } finally {
        this.loading = false
      }
    },

    /**
     * updateData — PUT /api/update
     * 向服务器发送更新请求，更新指定记录的数据。
     */
    async updateData(data) {
      this.loading = true
      this.error = null
      try {
        const response = await api.put('/update', data)
        this.lastUpdateData = response.data
        return response.data
      } catch (err) {
        this.error = err.message
        throw err
      } finally {
        this.loading = false
      }
    },

    /**
     * deleteData — DELETE /api/delete/:id
     * 向服务器发送删除请求，删除指定 ID 的记录。
     */
    async deleteData(id) {
      this.loading = true
      this.error = null
      try {
        const response = await api.delete(`/delete/${id}`)
        this.lastDeleteData = response.data
        return response.data
      } catch (err) {
        this.error = err.message
        throw err
      } finally {
        this.loading = false
      }
    },

    /**
     * login — POST /api/login
     * 登录鉴权，成功后保存 token 和用户信息到 localStorage。
     */
    async login(credentials) {
      this.loading = true
      this.error = null
      try {
        const response = await api.post('/login', {
          name: credentials.name,
          password: credentials.password
        })

        if (response.data.status === 'success') {
          const userData = {
            user_id: response.data.user_id,
            level: response.data.level,
            name: credentials.name
          }

          localStorage.setItem('token', response.data.token)
          localStorage.setItem('user', JSON.stringify(userData))

          this.user = userData
          this.isLoggedIn = true
        }

        return response.data
      } catch (err) {
        this.error = err.response?.data?.message || err.message
        throw err
      } finally {
        this.loading = false
      }
    },

    /**
     * logout — POST /api/logout
     * 退出登录，通知后端失效并清除本地存储的用户信息。
     */
    async logout() {
      try {
        await api.post('/logout')
      } catch (e) {
        console.warn('[Logout] 后端退出请求失败:', e.message)
      }

      localStorage.removeItem('token')
      localStorage.removeItem('user')

      this.user = null
      this.isLoggedIn = false
    },

    clearError() {
      this.error = null
    }
  }
})

export { api }
