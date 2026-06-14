/**
 * UserNav.vue — 通用用户导航组件
 *
 * 封装登录/注册弹窗、用户头像/昵称展示、退出功能。
 * 所有页面只需引入此组件，即可统一处理用户认证 UI。
 *
 * 用法：
 *   import UserNav from '@/components/UserNav.vue'
 *   在 header 中：<UserNav />
 *
 * 样式自定义：
 *   通过 CSS 变量或覆盖类名定制各页面不同风格。
 */

<script setup>
import { ref } from 'vue'
import { useApiStore } from '@/stores/api'

const apiStore = useApiStore()

// ─── 弹窗状态 ───
const isLogin = ref(true)
const showAuth = ref(false)
const authTitle = ref('登 录')
const authSubmitText = ref('登 录')
const toggleText = ref('还没有账号？<span>立即注册</span>')
const showPass2 = ref(false)
const authError = ref('')
const authLoading = ref(false)

function openAuth(mode) {
  authError.value = ''
  isLogin.value = mode === 'login'
  authTitle.value = isLogin.value ? '登 录' : '注 册'
  authSubmitText.value = isLogin.value ? '登 录' : '注 册'
  toggleText.value = isLogin.value
    ? '还没有账号？<span>立即注册</span>'
    : '已有账号？<span>去登录</span>'
  showPass2.value = !isLogin.value
  showAuth.value = true
}

function closeAuth() {
  showAuth.value = false
}

function toggleAuthMode() {
  openAuth(isLogin.value ? 'register' : 'login')
}

function handleOverlayClick(e) {
  if (e.target === e.currentTarget) closeAuth()
}

async function handleSubmit() {
  const user = document.getElementById('authUser').value
  const pass = document.getElementById('authPass').value

  if (!user || !pass) {
    authError.value = '请填写完整信息'
    return
  }

  authLoading.value = true
  authError.value = ''

  if (!isLogin.value) {
    // ─── 注册 ───
    const pass2 = document.getElementById('authPass2').value
    if (pass !== pass2) {
      authError.value = '两次密码不一致'
      authLoading.value = false
      return
    }
    try {
      const response = await fetch('/api/register', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ name: user, password: pass })
      })
      const data = await response.json()
      if (data.status === 'success') {
        alert('注册成功！请登录')
        openAuth('login')
      } else {
        authError.value = data.message || '注册失败'
      }
    } catch {
      authError.value = '注册失败：后端未开放注册接口，请直接登录'
    }
  } else {
    // ─── 登录 ───
    try {
      const data = await apiStore.login({ name: user, password: pass })
      if (data.status === 'success') {
        closeAuth()
      } else {
        authError.value = data.message || '登录失败'
      }
    } catch {
      authError.value = apiStore.error || '登录失败，请检查用户名和密码'
    }
  }
  authLoading.value = false
}
</script>

<template>
  <!-- ─── 未登录：登录/注册按钮 ─── -->
  <div v-if="!apiStore.isLoggedIn" class="usernav-buttons">
    <button class="usernav-btn" @click="openAuth('login')">登 录</button>
    <button class="usernav-btn" @click="openAuth('register')">注 册</button>
  </div>

  <!-- ─── 已登录：头像 + 昵称 + 退出 ─── -->
  <div v-else class="usernav-user">
    <!-- 有头像显示图片，没有则显示首字母 -->
    <img v-if="apiStore.user.avatar" :src="apiStore.user.avatar" class="usernav-avatar-img" alt="头像" />
    <div v-else class="usernav-avatar">{{ apiStore.user.name.charAt(0).toUpperCase() }}</div>
    <span class="usernav-nickname">{{ apiStore.user.name }}</span>
    <button class="usernav-logout-btn" @click="apiStore.logout()">退出</button>
  </div>

  <!-- ─── 登录/注册弹窗（teleport 到 body，避免父容器 transform 影响 fixed） ─── -->
  <Teleport to="body">
    <div class="usernav-overlay" :class="{ show: showAuth }" @click="handleOverlayClick">
      <div class="usernav-modal">
        <button class="usernav-close-btn" @click="closeAuth">✕</button>
        <h2>{{ authTitle }}</h2>
        <input type="text" id="authUser" placeholder="用户名" />
        <input type="password" id="authPass" placeholder="密码" />
        <input type="password" id="authPass2" placeholder="确认密码" :style="{ display: showPass2 ? 'block' : 'none' }" />
        <div v-if="authError" class="usernav-error">{{ authError }}</div>
        <button class="usernav-submit-btn" @click="handleSubmit" :disabled="authLoading">
          {{ authLoading ? '处理中...' : authSubmitText }}
        </button>
        <div class="usernav-toggle" @click="toggleAuthMode" v-html="toggleText"></div>
      </div>
    </div>
  </Teleport>
</template>

<style>
/* ═══════════════════════════════════════════
   未登录：按钮样式
   ═══════════════════════════════════════════ */
.usernav-buttons {
  display: flex;
  gap: 14px;
}
.usernav-btn {
  padding: 8px 30px;
  font-size: 1rem;
  border: 1px solid rgba(255,255,255,0.45);
  border-radius: 8px;
  background: rgba(255,255,255,0.35);
  color: rgba(255,255,255,0.95);
  cursor: pointer;
  transition: all 0.25s;
  letter-spacing: 2px;
}
.usernav-btn:hover {
  background: rgba(255,255,255,0.5);
}

/* ═══════════════════════════════════════════
   已登录：头像 + 昵称 + 退出
   ═══════════════════════════════════════════ */
.usernav-user {
  display: flex;
  align-items: center;
  gap: 10px;
}
.usernav-avatar {
  width: 36px;
  height: 36px;
  border-radius: 50%;
  background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
  color: white;
  display: flex;
  align-items: center;
  justify-content: center;
  font-size: 1rem;
  font-weight: 700;
  border: 2px solid rgba(255,255,255,0.5);
  box-shadow: 0 2px 6px rgba(0,0,0,0.15);
  flex-shrink: 0;
}
.usernav-avatar-img {
  width: 36px;
  height: 36px;
  border-radius: 50%;
  object-fit: cover;
  border: 2px solid rgba(255,255,255,0.5);
  box-shadow: 0 2px 6px rgba(0,0,0,0.15);
  flex-shrink: 0;
}
.usernav-nickname {
  font-size: 1rem;
  font-weight: 600;
  color: rgba(255,255,255,0.95);
  letter-spacing: 1px;
  white-space: nowrap;
}
.usernav-logout-btn {
  padding: 5px 16px;
  font-size: 0.85rem;
  border: 1px solid rgba(255,255,255,0.4);
  border-radius: 12px;
  background: rgba(255,255,255,0.2);
  color: rgba(255,255,255,0.95);
  cursor: pointer;
  transition: background 0.2s;
  white-space: nowrap;
}
.usernav-logout-btn:hover {
  background: rgba(239, 68, 68, 0.45);
}

/* ═══════════════════════════════════════════
   弹窗（遮罩 + 模态框）
   ═══════════════════════════════════════════ */
.usernav-overlay {
  display: none;
  position: fixed;
  top: 0; left: 0;
  width: 100%; height: 100%;
  background: rgba(0,0,0,0.4);
  z-index: 1001;
}
.usernav-overlay.show {
  display: block;
}

.usernav-modal {
  background: #fff;
  border: 2px solid #000;
  border-radius: 16px;
  padding: 40px 50px;
  position: absolute;
  min-width: 320px;
  text-align: center;
  top: 50%;
  left: 50%;
  transform: translate(-50%, -50%);
}
.usernav-modal .usernav-close-btn {
  position: absolute;
  top: 12px; right: 16px;
  border: none; background: none;
  font-size: 24px; cursor: pointer;
  color: #999;
}
.usernav-modal .usernav-close-btn:hover { color: #000; }
.usernav-modal h2 {
  margin-bottom: 20px;
  font-size: 1.8rem;
  color: #000;
}
.usernav-modal input {
  display: block; width: 100%;
  padding: 10px 14px; margin: 12px 0;
  border: 2px solid #000; border-radius: 8px;
  font-size: 1rem; outline: none;
}
.usernav-modal input:focus { border-color: #666; }
.usernav-modal .usernav-error {
  background: #fef2f2;
  border: 1px solid #fecaca;
  color: #dc2626;
  padding: 8px 12px;
  border-radius: 6px;
  font-size: 0.85rem;
  margin: 8px 0;
  text-align: center;
}
.usernav-modal .usernav-submit-btn {
  width: 100%; padding: 10px; margin-top: 10px;
  border: 2px solid #000; border-radius: 8px;
  background: #000; color: #fff;
  font-size: 1.1rem; cursor: pointer;
  transition: background 0.2s;
}
.usernav-modal .usernav-submit-btn:hover { background: #333; }
.usernav-modal .usernav-submit-btn:disabled { opacity: 0.6; cursor: not-allowed; }
.usernav-modal .usernav-toggle {
  margin-top: 14px;
  font-size: 0.95rem;
  color: #666;
  cursor: pointer;
}
.usernav-modal .usernav-toggle span {
  color: #0066cc;
  text-decoration: underline;
}

/* ═══════════════════════════════════════════
   响应式
   ═══════════════════════════════════════════ */
@media (max-width: 768px) {
  .usernav-btn {
    padding: 6px 18px;
    font-size: 0.9rem;
  }
  .usernav-modal {
    min-width: 280px;
    padding: 30px 24px;
    margin: 0 10px;
  }
}
</style>