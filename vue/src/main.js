/**
 * main.js — Vue 3 应用入口
 * 
 * 创建 Vue 应用实例，注册 Pinia 状态管理和 Vue Router 路由，
 * 最后挂载到 index.html 的 <div id="app"> 上。
 */

import { createApp } from 'vue'
import { createPinia } from 'pinia'
import router from './router'
import App from './App.vue'
import './assets/main.css'

const app = createApp(App)

app.use(createPinia())
app.use(router)

app.mount('#app')