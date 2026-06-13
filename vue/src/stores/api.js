import { defineStore } from 'pinia'
import axios from 'axios'

const api = axios.create({
  baseURL: '/api',
  timeout: 5000,
  headers: {
    'Content-Type': 'application/json'
  }
})

// ============================================================
// axios 拦截器：每次请求自动带上 Authorization: Bearer <token>
// ============================================================
api.interceptors.request.use(config => {
  const token = localStorage.getItem('token')
  if (token) {
    config.headers.Authorization = `Bearer ${token}`
  }
  return config
})

// ============================================================
// Pinia Store
// ============================================================
export const useApiStore = defineStore('api', {
  state: () => ({
    helloMessage: '',
    serverTime: '',
    lastPostData: null,
    lastUpdateData: null,
    lastDeleteData: null,
    loading: false,
    error: null,
    // 用户登录状态
    user: null,           // { user_id, level, name }
    isLoggedIn: false     // 是否已登录
  }),

  actions: {
    // ----------------------------------------------------------
    // initAuth — 从 localStorage 恢复登录状态
    // 放在 App.vue 的 created/mounted 中调用
    // ----------------------------------------------------------
    initAuth() {
      const token = localStorage.getItem('token')
      const savedUser = localStorage.getItem('user')
      if (token && savedUser) {
        try {
          this.user = JSON.parse(savedUser)
          this.isLoggedIn = true
          console.log('[Auth] 从 localStorage 恢复登录状态:', this.user.name)
        } catch (e) {
          // 数据损坏，清除
          localStorage.removeItem('token')
          localStorage.removeItem('user')
        }
      }
    },

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

    // ----------------------------------------------------------
    // login — 登录并保存 token 到 localStorage
    // ----------------------------------------------------------
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

          // 保存 token 和用户信息到 localStorage（刷新页面不丢）
          localStorage.setItem('token', response.data.token)
          localStorage.setItem('user', JSON.stringify(userData))

          // 更新 Pinia state
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

    // ----------------------------------------------------------
    // logout — 退出登录，清除 token
    // ----------------------------------------------------------
    async logout() {
      try {
        // 通知后端 token 失效
        await api.post('/logout')
      } catch (e) {
        // 即使请求失败也要清除本地状态
        console.warn('[Logout] 后端退出请求失败:', e.message)
      }

      // 清除本地存储
      localStorage.removeItem('token')
      localStorage.removeItem('user')

      // 清除 Pinia state
      this.user = null
      this.isLoggedIn = false
    },

    // 清除错误状态
    clearError() {
      this.error = null
    }
  }
})

// 导出 axios 实例，方便其他组件直接使用
export { api }
