# Vue 3 框架使用指南

> 基于本项目 `vue/` 目录的 Vue 3 完整教程

---

## 目录

- [一、项目启动](#一项目启动)
- [二、项目结构与入口](#二项目结构与入口)
- [三、单文件组件 (SFC)](#三单文件组件-sfc)
- [四、响应式数据](#四响应式数据)
- [五、模板指令](#五模板指令)
- [六、生命周期](#六生命周期)
- [七、Vue Router 路由](#七vue-router-路由)
- [八、Pinia 状态管理](#八pinia-状态管理)
- [九、HTTP 请求 (Axios)](#九http-请求-axios)
- [十、组件通信](#十组件通信)
- [十一、组合式函数 (Composables)](#十一组合式函数-composables)
- [十二、环境变量与构建](#十二环境变量与构建)
- [十三、Vite 配置](#十三vite-配置)
- [十四、Vue 3 vs Vue 2 区别](#十四vue-3-vs-vue-2-区别)
- [十五、常见问题](#十五常见问题)

---

## 一、项目启动

### 前置条件

- Node.js >= 18
- npm 或 yarn

### 命令

```bash
# 进入项目目录
cd vue

# 安装依赖（首次）
npm install

# 启动开发服务器
npm run dev
# 默认访问 http://localhost:3000

# 构建生产版本
npm run build
# 输出到 dist/ 目录

# 预览构建结果
npm run preview
```

---

## 二、项目结构与入口

### 目录结构

```
vue/
├── index.html              # HTML 入口文件（Vue 挂载点）
├── package.json            # 依赖配置
├── vite.config.js          # Vite 构建配置
├── public/                 # 静态资源
│   └── vite.svg
├── src/
│   ├── main.js             # Vue 应用入口
│   ├── App.vue             # 根组件
│   ├── assets/
│   │   └── main.css        # 全局样式
│   ├── router/
│   │   └── index.js        # 路由配置
│   ├── stores/
│   │   └── api.js          # Pinia 状态管理
│   ├── views/
│   │   ├── HomeView.vue    # 首页
│   │   ├── ApiDemoView.vue # API 演示页
│   │   └── AboutView.vue   # 关于页面
│   └── components/         # （建议）可复用组件目录
└── node_modules/           # 依赖包
```

### 入口文件 `index.html`

```html
<!DOCTYPE html>
<html lang="zh-CN">
  <body>
    <!-- Vue 挂载点 -->
    <div id="app"></div>
    <!-- 引入入口 JS -->
    <script type="module" src="/src/main.js"></script>
  </body>
</html>
```

### 应用入口 `src/main.js`

```js
import { createApp } from 'vue'      // 创建 Vue 应用
import { createPinia } from 'pinia'  // 状态管理
import router from './router'        // 路由
import App from './App.vue'          // 根组件
import './assets/main.css'           // 全局样式

const app = createApp(App)           // ① 创建应用实例

app.use(createPinia())               // ② 安装插件（Pinia）
app.use(router)                      // ③ 安装插件（Router）

app.mount('#app')                    // ④ 挂载到 DOM
```

**流程总结：** `createApp` → `use` 插件 → `mount` 挂载

---

## 三、单文件组件 (SFC)

Vue 3 推荐使用 **组合式 API（Composition API）** + `<script setup>` 语法糖。

### 文件结构

```vue
<script setup>
// JavaScript 逻辑（组合式 API）
</script>

<template>
  <!-- HTML 模板 -->
</template>

<style scoped>
/* CSS 样式（scoped 隔离，仅当前组件生效） */
</style>
```

### 完整示例（来自 HomeView.vue）

```vue
<script setup>
import { ref, onMounted } from 'vue'
import { useApiStore } from '@/stores/api'

// 使用 Pinia store
const apiStore = useApiStore()

// 响应式数据
const helloMessage = ref('')
const loading = ref(true)

// 生命周期钩子
onMounted(async () => {
  try {
    const data = await apiStore.hello()
    helloMessage.value = data.message
  } finally {
    loading.value = false
  }
})
</script>

<template>
  <div v-if="loading">
    <div class="spinner"></div>
    <p>加载中...</p>
  </div>
  <div v-else>
    <p>{{ helloMessage }}</p>
  </div>
</template>

<style scoped>
.spinner {
  width: 40px;
  height: 40px;
  border: 4px solid #f3f3f3;
  border-top: 4px solid #667eea;
  border-radius: 50%;
  animation: spin 1s linear infinite;
}

@keyframes spin {
  0% { transform: rotate(0deg); }
  100% { transform: rotate(360deg); }
}
</style>
```

### 关键规则

| 写法 | 说明 |
|------|------|
| `<script setup>` | 顶层变量/函数直接在模板可用，无需 `return` |
| `ref` 在模板中 | 自动解包，不用写 `.value` |
| `ref` 在 JS 中 | 必须用 `.value` 访问和修改 |
| `scoped` | 样式只作用于当前组件，不影响全局 |

---

## 四、响应式数据

### `ref()` — 基本类型 / 任意值

```js
import { ref } from 'vue'

const count = ref(0)          // 数字
const name = ref('张三')      // 字符串
const list = ref([1, 2, 3])   // 数组
const data = ref(null)        // 初始为空

// JS 中操作必须 .value
count.value++                 // 修改
console.log(count.value)      // 读取
```

### `reactive()` — 对象（深层响应式）

```js
import { reactive } from 'vue'

const user = reactive({
  name: '张三',
  age: 25,
  address: { city: '北京' }
})

// 直接修改，不需要 .value
user.age = 26
user.address.city = '上海'

// 注意：reactive 不能直接替换整个对象
// ❌ user = reactive({ ... })  会丢失响应
// ✅ Object.assign(user, { name: '李四' })
```

### `computed()` — 计算属性

```js
import { ref, computed } from 'vue'

const count = ref(10)
const doubleCount = computed(() => count.value * 2)
const status = computed(() => count.value > 5 ? '大' : '小')
```

### `watch()` — 监听变化

```js
import { ref, watch } from 'vue'

const keyword = ref('')

// 监听单个数据
watch(keyword, (newVal, oldVal) => {
  console.log(`从 "${oldVal}" 变为 "${newVal}"`)
})

// 监听多个数据
watch([count, keyword], ([newCount, newKeyword]) => {
  console.log('count 或 keyword 发生了变化')
})

// 深度监听对象
watch(() => user.value, (newVal) => {
  console.log('user 变了', newVal)
}, { deep: true })
```

---

## 五、模板指令

### 指令速查表

| 指令 | 简写 | 作用 | 示例 |
|------|------|------|------|
| `v-bind` | `:` | 动态绑定属性 | `<img :src="url" />` |
| `v-on` | `@` | 事件监听 | `<button @click="fn">` |
| `v-if` | - | 条件渲染（移除 DOM） | `<div v-if="ok">` |
| `v-else` | - | 否则 | `<div v-else>` |
| `v-show` | - | 条件渲染（切换 display） | `<div v-show="ok">` |
| `v-for` | - | 列表渲染 | `<li v-for="item in list">` |
| `v-model` | - | 双向绑定 | `<input v-model="name" />` |
| `v-html` | - | 渲染 HTML | `<div v-html="html" />` |
| `v-once` | - | 仅渲染一次 | `<div v-once>` |

### 条件渲染

```html
<!-- v-if / v-else-if / v-else -->
<div v-if="loading">加载中...</div>
<div v-else-if="error">出错了：{{ error }}</div>
<div v-else>加载完成</div>

<!-- v-show（频繁切换用这个，性能更好） -->
<div v-show="isVisible">显示或隐藏</div>
```

### 列表渲染

```html
<!-- 遍历数组 -->
<button
  v-for="tab in tabs"
  :key="tab.id"
  :class="{ active: activeTab === tab.id }"
  @click="activeTab = tab.id"
>
  {{ tab.label }}
</button>

<!-- 遍历对象 -->
<li v-for="(value, key) in user" :key="key">
  {{ key }}: {{ value }}
</li>

<!-- 带索引 -->
<li v-for="(item, index) in list" :key="index">
  {{ index }}: {{ item }}
</li>
```

### 事件绑定

```html
<!-- 基本用法 -->
<button @click="handleClick">点击</button>

<!-- 传参 -->
<button @click="deleteItem(item.id)">删除</button>

<!-- 事件对象 -->
<button @click="handleClick($event, '参数')">点击</button>

<!-- 修饰符 -->
<form @submit.prevent="onSubmit">    <!-- 阻止默认行为 -->
<a @click.stop="handleClick">...</a> <!-- 阻止冒泡 -->
<input @keyup.enter="submit" />       <!-- 按回车触发 -->
```

### 样式绑定

```html
<!-- 绑定 class（来自 App.vue） -->
<span :class="['status-indicator', 
  serverStatus.includes('在线') ? 'online' : 'offline']">

<!-- 对象语法 -->
<div :class="{ active: isActive, disabled: isDisabled }">

<!-- 绑定 style -->
<div :style="{ color: textColor, fontSize: size + 'px' }">
```

### 表单绑定 `v-model`

```html
<!-- 文本输入 -->
<input v-model="username" type="text" />

<!-- 数字 -->
<input v-model.number="age" type="number" />

<!-- 多行文本 -->
<textarea v-model="description"></textarea>

<!-- 复选框 -->
<input type="checkbox" v-model="agreed" />

<!-- 下拉选择 -->
<select v-model="selected">
  <option value="a">A</option>
  <option value="b">B</option>
</select>
```

**修饰符：**

| 修饰符 | 作用 |
|--------|------|
| `.lazy` | 在 change 事件后同步 |
| `.number` | 自动转为数字 |
| `.trim` | 去除首尾空格 |

---

## 六、生命周期

### 生命周期钩子

| 钩子 | 触发时机 | 常见用途 |
|------|---------|---------|
| `onBeforeMount()` | 挂载前 | 最后一次修改数据 |
| `onMounted()` | **挂载到 DOM 后** | **发起请求、操作 DOM** |
| `onBeforeUpdate()` | 更新前 | 获取更新前的状态 |
| `onUpdated()` | 更新后 | 操作更新后的 DOM |
| `onBeforeUnmount()` | 卸载前 | 清理定时器 |
| `onUnmounted()` | 卸载后 | 清理事件监听 |

### 本项目中实际使用

```js
import { onMounted } from 'vue'

onMounted(async () => {
  // 页面加载时自动请求数据
  const data = await apiStore.hello()
  helloMessage.value = data.message
})
```

---

## 七、Vue Router 路由

### 路由配置 `src/router/index.js`

```js
import { createRouter, createWebHistory } from 'vue-router'
import HomeView from '@/views/HomeView.vue'

const routes = [
  {
    path: '/',              // URL 路径
    name: 'home',           // 路由名称（用于编程式导航）
    component: HomeView     // 对应的组件
  },
  {
    path: '/api-demo',
    name: 'api-demo',
    component: () => import('@/views/ApiDemoView.vue') // 懒加载
  },
  {
    path: '/about',
    name: 'about',
    component: AboutView
  }
]

const router = createRouter({
  history: createWebHistory(), // HTML5 History 模式（无 # 号）
  routes
})

export default router
```

### 在模板中使用

```html
<!-- 声明式导航（类似 <a> 标签） -->
<RouterLink to="/">首页</RouterLink>
<RouterLink to="/api-demo" active-class="active">API 演示</RouterLink>
<RouterLink :to="{ name: 'about' }">关于</RouterLink>

<!-- 路由出口（当前路由对应的组件渲染到这里） -->
<RouterView />
```

### 编程式导航

```js
import { useRouter, useRoute } from 'vue-router'

const router = useRouter()  // 导航
const route = useRoute()    // 当前路由信息

// 跳转
router.push('/api-demo')
router.push({ name: 'api-demo' })
router.push({ path: '/user/1', query: { tab: 'profile' } })

// 替换（不产生历史记录）
router.replace('/login')

// 前进 / 后退
router.go(1)     // 前进 1 步
router.back()    // 后退
```

### 动态路由

```js
// 路由配置
{ path: '/user/:id', component: UserView }

// 组件中获取参数
import { useRoute } from 'vue-router'
const route = useRoute()
console.log(route.params.id)   // /user/42 → "42"
```

### 路由守卫

```js
// 全局前置守卫
router.beforeEach((to, from) => {
  const isLoggedIn = localStorage.getItem('token')
  if (to.name !== 'login' && !isLoggedIn) {
    return { name: 'login' }  // 重定向到登录页
  }
})
```

---

## 八、Pinia 状态管理

### 定义 Store `src/stores/api.js`

```js
import { defineStore } from 'pinia'
import axios from 'axios'

// 封装 Axios
const api = axios.create({
  baseURL: '/api',
  timeout: 5000
})

// 请求拦截器 — 自动带 token
api.interceptors.request.use(config => {
  const token = localStorage.getItem('token')
  if (token) {
    config.headers.Authorization = `Bearer ${token}`
  }
  return config
})

// 定义 Store
export const useApiStore = defineStore('api', {
  // state — 状态（数据）
  state: () => ({
    user: null,
    loading: false,
    error: null,
    isLoggedIn: false
  }),

  // actions — 方法（同步/异步）
  actions: {
    initAuth() {
      const token = localStorage.getItem('token')
      const savedUser = localStorage.getItem('user')
      if (token && savedUser) {
        this.user = JSON.parse(savedUser)
        this.isLoggedIn = true
      }
    },

    async hello() {
      this.loading = true
      this.error = null
      try {
        const res = await api.get('/hello')
        return res.data
      } catch (err) {
        this.error = err.message
        throw err
      } finally {
        this.loading = false
      }
    },

    async login(credentials) {
      this.loading = true
      try {
        const res = await api.post('/login', credentials)
        if (res.data.status === 'success') {
          localStorage.setItem('token', res.data.token)
          localStorage.setItem('user', JSON.stringify({
            user_id: res.data.user_id,
            level: res.data.level,
            name: credentials.name
          }))
          this.user = res.data
          this.isLoggedIn = true
        }
        return res.data
      } catch (err) {
        this.error = err.response?.data?.message || err.message
        throw err
      } finally {
        this.loading = false
      }
    },

    logout() {
      localStorage.removeItem('token')
      localStorage.removeItem('user')
      this.user = null
      this.isLoggedIn = false
    },

    async postData(data) {
      this.loading = true
      try {
        const res = await api.post('/data', data)
        return res.data
      } finally {
        this.loading = false
      }
    }
  }
})
```

### 在组件中使用

```js
import { useApiStore } from '@/stores/api'

// 获取 store 实例（同一份数据）
const apiStore = useApiStore()

// 读取状态（模板中直接使用）
console.log(apiStore.loading)     // false
console.log(apiStore.user)        // { name: '...', ... }
console.log(apiStore.isLoggedIn)  // true/false

// 调用 action
await apiStore.hello()
await apiStore.login({ name: 'admin', password: '123456' })
apiStore.logout()
```

### Pinia 三大概念

| 概念 | 作用 | 项目中示例 |
|------|------|-----------|
| `state` | 存储数据 | `user`、`loading`、`error` |
| `actions` | 定义方法 | `hello()`、`login()`、`postData()` |
| `getters` | 计算属性（可选） | 未使用，可按需添加 |

### 添加 Getter 示例

```js
export const useApiStore = defineStore('api', {
  state: () => ({ count: 0 }),
  getters: {
    doubleCount: (state) => state.count * 2
  },
  actions: {
    increment() { this.count++ }
  }
})
```

---

## 九、HTTP 请求 (Axios)

### 基本用法

```js
import axios from 'axios'

// GET 请求
const res = await axios.get('/api/hello')

// POST 请求（提交 JSON 数据）
const res = await axios.post('/api/data', {
  name: '张三',
  age: 25,
  email: 'zhangsan@example.com'
})

// PUT 请求（更新数据）
const res = await axios.put('/api/update', {
  id: 1,
  name: '新名称'
})

// DELETE 请求
const res = await axios.delete('/api/delete/1')
```

### 创建实例（推荐）

```js
const api = axios.create({
  baseURL: '/api',       // 基础路径
  timeout: 5000,         // 超时时间
  headers: {
    'Content-Type': 'application/json'
  }
})

// 拦截器 — 请求前自动带 token
api.interceptors.request.use(config => {
  const token = localStorage.getItem('token')
  if (token) {
    config.headers.Authorization = `Bearer ${token}`
  }
  return config
})

// 拦截器 — 统一处理错误
api.interceptors.response.use(
  response => response,
  error => {
    console.error('请求失败:', error.message)
    return Promise.reject(error)
  }
)

export { api }
```

---

## 十、组件通信

### 1️⃣ 父传子 — Props

```vue
<!-- 父组件 -->
<script setup>
import Child from './Child.vue'
const title = ref('Hello')
</script>

<template>
  <Child :title="title" :count="5" />
</template>
```

```vue
<!-- 子组件 Child.vue -->
<script setup>
const props = defineProps({
  title: {
    type: String,
    required: true
  },
  count: {
    type: Number,
    default: 0
  }
})
</script>

<template>
  <h2>{{ props.title }}</h2>
  <p>计数：{{ count }}</p>
</template>
```

### 2️⃣ 子传父 — Emit

```vue
<!-- 子组件 -->
<script setup>
const emit = defineEmits(['update', 'delete'])

function handleClick() {
  emit('update', { id: 1, name: '新数据' })
}
</script>

<template>
  <button @click="handleClick">更新</button>
</template>
```

```vue
<!-- 父组件 -->
<template>
  <Child @update="onUpdate" @delete="onDelete" />
</template>

<script setup>
function onUpdate(data) {
  console.log('收到更新:', data)
}
function onDelete(id) {
  console.log('删除:', id)
}
</script>
```

### 3️⃣ 跨组件通信 — Pinia（推荐）

任意组件之间共享数据，无需层层传递：

```js
// 组件 A — 写入
import { useApiStore } from '@/stores/api'
const store = useApiStore()
store.user = { name: '张三' }

// 组件 B（任意位置）— 读取
import { useApiStore } from '@/stores/api'
const store = useApiStore()
console.log(store.user.name) // '张三'
```

### 4️⃣ 跨层级透传 — provide / inject

```js
// 祖先组件
import { provide } from 'vue'
provide('theme', 'dark')

// 后代组件
import { inject } from 'vue'
const theme = inject('theme') // 'dark'
```

---

## 十一、组合式函数 (Composables)

将可复用逻辑抽取到独立文件，类似 React 的 Hooks。

### 定义 `src/composables/useServerStatus.js`

```js
import { ref, onMounted } from 'vue'
import { useApiStore } from '@/stores/api'

export function useServerStatus() {
  const status = ref('检查中...')
  const isOnline = ref(false)

  onMounted(async () => {
    try {
      const apiStore = useApiStore()
      await apiStore.hello()
      status.value = '🟢 在线'
      isOnline.value = true
    } catch {
      status.value = '🔴 离线'
      isOnline.value = false
    }
  })

  return { status, isOnline }
}
```

### 在组件中使用

```vue
<script setup>
import { useServerStatus } from '@/composables/useServerStatus'

const { status, isOnline } = useServerStatus()
</script>

<template>
  <div :class="isOnline ? 'online' : 'offline'">
    服务器状态：{{ status }}
  </div>
</template>
```

---

## 十二、环境变量与构建

### 环境变量文件

在项目根目录创建（与 `vite.config.js` 同级）：

```bash
# .env.development — 开发环境
VITE_API_BASE_URL=http://localhost:60906
VITE_APP_TITLE=开发环境

# .env.production — 生产环境
VITE_API_BASE_URL=/api
VITE_APP_TITLE=Native Socket Server
```

### 在代码中使用

```js
console.log(import.meta.env.VITE_API_BASE_URL)
console.log(import.meta.env.VITE_APP_TITLE)
```

注意：变量名必须以 `VITE_` 开头才会暴露给前端。

### 构建部署

```bash
npm run build     # 构建 → 生成 dist/ 目录
```

`dist/` 中的静态文件可以部署到：
- Nginx / Apache
- 本项目的 C++ 后端服务器（直接提供静态文件服务）
- 任何静态文件托管服务（Vercel、Netlify 等）

---

## 十三、Vite 配置

### `vite.config.js`

```js
import { defineConfig } from 'vite'
import vue from '@vitejs/plugin-vue'
import { fileURLToPath, URL } from 'node:url'

export default defineConfig({
  plugins: [vue()],            // Vue 插件

  resolve: {
    alias: {
      '@': fileURLToPath(new URL('./src', import.meta.url))
      // 让 @ 指向 src/ 目录，例如 import xxx from '@/stores/api'
    }
  },

  server: {
    port: 3000,                // 开发服务器端口
    proxy: {
      '/api': {                // 代理 /api 开头的请求
        target: 'http://localhost:60906',
        changeOrigin: true
      }
    }
  }
})
```

### 路径别名

配置 `@` 别名后，可以这样引入：

```js
// 不用别名
import HomeView from '../../views/HomeView.vue'

// 用别名
import HomeView from '@/views/HomeView.vue'
import { useApiStore } from '@/stores/api'
```

---

## 十四、Vue 3 vs Vue 2 区别

| 特性 | Vue 2（选项式 API） | Vue 3（组合式 API） |
|------|--------------------|--------------------|
| 数据定义 | `data() { return {} }` | `ref()` / `reactive()` |
| 方法 | `methods: { fn() {} }` | 直接在 `<script setup>` 定义函数 |
| 计算属性 | `computed: { }` | `computed(() => ...)` |
| 侦听器 | `watch: { }` | `watch(() => val, callback)` |
| 生命周期 | `created()`、`mounted()` | `onMounted()` 需导入 |
| 响应式原理 | `Object.defineProperty` | `Proxy`（性能更好） |
| 多根节点 | ❌ 必须有一个根元素 | ✅ 支持多个根节点 |
| TypeScript | 支持有限 | 原生完美支持 |
| 逻辑复用 | Mixins（有命名冲突） | Composables（无冲突） |

---

## 十五、常见问题

### Q1: `ref` 在模板中不更新

```
原因：在 JS 中修改时忘了加 .value
✅ count.value++
❌ count++
```

### Q2: 跨域请求失败

```
原因：前端端口与后端不同
解决：在 vite.config.js 中配置 proxy
```

### Q3: 刷新页面后 Pinia 状态丢失

```
原因：Pinia 默认存储在内存中
解决：手动保存到 localStorage（本项目已实现）
```

### Q4: scoped 样式影响不到子组件深层元素

```css
/* 使用 :deep() 穿透 */
.parent :deep(.child-class) {
  color: red;
}
```

### Q5: v-for 和 v-if 不能同时使用

```html
<!-- ❌ 错误 -->
<li v-for="item in list" v-if="item.visible">

<!-- ✅ 正确：用计算属性过滤 -->
<li v-for="item in visibleList" :key="item.id">
```

```js
const visibleList = computed(() => list.value.filter(item => item.visible))
```

### Q6: 动态组件渲染

```html
<component :is="currentComponent" />
```

---

## 附录：本项目 API 接口

| 方法 | 路径 | 描述 |
|------|------|------|
| GET | /api/hello | 获取问候消息和工作进程 PID |
| POST | /api/login | 用户登录（获取 token） |
| POST | /api/logout | 退出登录 |
| POST | /api/data | 提交 name、age、email 数据 |
| PUT | /api/update | 更新指定 ID 的记录 |
| DELETE | /api/delete/:id | 删除指定 ID 的记录 |

---

> **提示**：如果需要在组件中添加 `@` 路径别名支持，确保 `vite.config.js` 中已配置 resolve.alias。
> 开发时若遇到跨域问题，检查 `vite.config.js` 中的 proxy 配置。
