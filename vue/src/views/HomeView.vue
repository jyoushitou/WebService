/**
 * HomeView.vue — 首页
 * 
 * 展示四个板块（文章/图片/视频/博客），每个板块包含三个标签和若干图片。
 * 页面加载时显示开屏动画，内容逐渐弹出，并且支持滚动时的入场/退场动画。
 * 登录/注册弹窗集成在首页中。
 */

<script setup>
import { ref, onMounted, onBeforeUnmount, nextTick, watch, computed } from 'vue'
import { useRouter } from 'vue-router'
import { useApiStore } from '@/stores/api'

const router = useRouter()
const apiStore = useApiStore()

function goToArticle() {
  router.push('/article')
}

// ─── 登录/注册 ───
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
function closeAuth() { showAuth.value = false }
function toggleAuthMode() { openAuth(isLogin.value ? 'register' : 'login') }
function handleOverlayClick(e) {
  if (e.target === e.currentTarget) closeAuth()
}

function handleLogout() {
  apiStore.logout()
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
    const pass2 = document.getElementById('authPass2').value
    if (pass !== pass2) {
      authError.value = '两次密码不一致'
      authLoading.value = false
      return
    }
    // 注册请求
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
    } catch (err) {
      authError.value = '注册失败：后端未开放注册接口，请直接登录'
    }
  } else {
    // 登录请求：调用 apiStore 的后端登录
    try {
      const data = await apiStore.login({ name: user, password: pass })
      if (data.status === 'success') {
        closeAuth()
      } else {
        authError.value = data.message || '登录失败'
      }
    } catch (err) {
      authError.value = apiStore.error || '登录失败，请检查用户名和密码'
    }
  }
  authLoading.value = false
}

// ─── 页面数据（从后端 API 获取，失败用默认值） ───
const sectionsData = ref([
  { title: '文章', rows: [
    { label: '标签1', imgs: ['/image/0_0_0.jpg','/image/0_0_1.jpg','/image/0_0_2.jpg','/image/0_0_3.jpg','/image/0_0_4.jpg','/image/0_0_5.jpg'] },
    { label: '标签2', imgs: ['/image/0_1_0.jpg','/image/0_1_1.jpg','/image/0_1_2.jpg','/image/0_1_3.jpg','/image/0_1_4.jpg'] },
    { label: '标签3', imgs: ['/image/0_2_0.jpg','/image/0_2_1.jpg','/image/0_2_2.jpg','/image/0_2_3.jpg'] }
  ]},
  { title: '图片', rows: [
    { label: '标签1', imgs: ['/image/1_0_0.jpg','/image/1_0_1.jpg','/image/1_0_2.jpg','/image/1_0_3.jpg','/image/1_0_4.jpg','/image/1_0_5.jpg'] },
    { label: '标签2', imgs: ['/image/1_1_0.jpg','/image/1_1_1.jpg','/image/1_1_2.jpg','/image/1_1_3.jpg','/image/1_1_4.jpg'] },
    { label: '标签3', imgs: ['/image/1_2_0.jpg','/image/1_2_1.jpg','/image/1_2_2.jpg','/image/1_2_3.jpg'] }
  ]},
  { title: '视频', rows: [
    { label: '标签1', imgs: ['/image/2_0_0.jpg','/image/2_0_1.jpg','/image/2_0_2.jpg','/image/2_0_3.jpg','/image/2_0_4.jpg','/image/2_0_5.jpg'] },
    { label: '标签2', imgs: ['/image/2_1_0.jpg','/image/2_1_1.jpg','/image/2_1_2.jpg','/image/2_1_3.jpg','/image/2_1_4.jpg'] },
    { label: '标签3', imgs: ['/image/2_2_0.jpg','/image/2_2_1.jpg','/image/2_2_2.jpg','/image/2_2_3.jpg'] }
  ]},
  { title: '博客', rows: [
    { label: '标签1', imgs: ['/image/3_0_0.jpg','/image/3_0_1.jpg','/image/3_0_2.jpg','/image/3_0_3.jpg','/image/3_0_4.jpg','/image/3_0_5.jpg'] },
    { label: '标签2', imgs: ['/image/3_1_0.jpg','/image/3_1_1.jpg','/image/3_1_2.jpg','/image/3_1_3.jpg','/image/3_1_4.jpg'] },
    { label: '标签3', imgs: ['/image/3_2_0.jpg','/image/3_2_1.jpg','/image/3_2_2.jpg','/image/3_2_3.jpg'] }
  ]}
])

async function fetchContents() {
  try {
    const res = await fetch('/api/contents')
    const data = await res.json()
    if (data.code === 200 && data.data && data.data.length > 0) {
      sectionsData.value = data.data
    }
  } catch (e) {
    console.error('获取内容列表失败，使用默认数据:', e)
  }
}

let observer = null

function runSplashAndAnimations() {
  const allBgs = document.querySelectorAll('.h-backgrand')
  if (allBgs.length === 0) return

  const splash = document.getElementById('splashTitle')
  const h1 = document.getElementById('splashH1')
  if (!splash || !h1) return

  splash.style.display = 'flex'
  splash.style.background = 'rgba(0,0,0,0.3)'
  h1.style.transition = 'all 1.2s ease'
  h1.style.fontSize = '5rem'
  h1.style.opacity = '1'
  h1.style.transform = 'translateY(0)'

  requestAnimationFrame(() => {
    requestAnimationFrame(() => {
      h1.style.transform = 'translateY(calc(-50vh + 40px)) scale(0.5)'
      splash.style.background = 'transparent'
    })
  })

  setTimeout(() => {
    document.getElementById('mainContent').style.display = 'block'

    const header = document.querySelector('.header')
    const firstTitles = document.querySelectorAll('.first-title')
    const sectionEls = document.querySelectorAll('.section')

    if (header) header.classList.add('pop-in')
    for (let ti = 0; ti < firstTitles.length; ti++) {
      firstTitles[ti].classList.add('pop-in')
      if (sectionEls[ti]) sectionEls[ti].classList.add('pop-in')
    }

    let maxRows = 0
    for (let si = 0; si < sectionEls.length; si++) {
      const rc = sectionEls[si].querySelectorAll('.h-backgrand').length
      if (rc > maxRows) maxRows = rc
    }

    let delay = 600
    for (let rj = 0; rj < maxRows; rj++) {
      ;(function(rowIdx, d) {
        setTimeout(() => {
          for (let siA = 0; siA < sectionEls.length; siA++) {
            const bgsA = sectionEls[siA].querySelectorAll('.h-backgrand')
            if (bgsA[rowIdx]) bgsA[rowIdx].classList.add('pop-in')
          }
        }, d)
      })(rj, delay)

      let maxImg = 0
      for (let si2 = 0; si2 < sectionEls.length; si2++) {
        const bgs2 = sectionEls[si2].querySelectorAll('.h-backgrand')
        if (bgs2[rj]) {
          const ic = bgs2[rj].querySelectorAll('.img-box').length
          if (ic > maxImg) maxImg = ic
        }
      }
      for (let bj = 0; bj < maxImg; bj++) {
        ;(function(rowIdx, boxIdx, d) {
          setTimeout(() => {
            for (let si3 = 0; si3 < sectionEls.length; si3++) {
              const bgs3 = sectionEls[si3].querySelectorAll('.h-backgrand')
              if (bgs3[rowIdx]) {
                const boxes = bgs3[rowIdx].querySelectorAll('.img-box')
                if (boxes[boxIdx]) boxes[boxIdx].classList.add('pop-in')
              }
            }
          }, d)
        })(rj, bj, delay + 50 + bj * 80)
      }
      delay += maxImg * 80 + 50
    }

    setTimeout(() => {
      startObserver(allBgs)
    }, delay + 500)

  }, 1300)
}

function startObserver(allBgs) {
  observer = new IntersectionObserver((entries) => {
    entries.forEach(entry => {
      if (entry.isIntersecting) {
        entry.target.classList.remove('pop-in', 'scroll-out')
        entry.target.classList.add('scroll-in')
      } else {
        entry.target.classList.remove('scroll-in')
        entry.target.classList.add('scroll-out')
      }
    })
  }, { threshold: 0.1 })

  allBgs.forEach(bg => observer.observe(bg))
}

// 监听 sectionsData 变化，数据到达后触发动画
watch(sectionsData, async (val) => {
  if (val && val.length > 0) {
    await nextTick()
    runSplashAndAnimations()
  }
}, { immediate: true })

onMounted(async () => {
  // 先加载背景
  const bgList = [
    'url("/image/129442526_p0.png")',
    'url("/image/88515515_p0.png")'
  ]
  const bg = bgList[Math.floor(Math.random() * bgList.length)]
  const hp = document.querySelector('.home-page')
  if (hp) hp.style.backgroundImage = bg

  // 获取数据（API 会覆盖默认值）
  await fetchContents()
  await nextTick()
})

onBeforeUnmount(() => {
  if (observer) observer.disconnect()
})
</script>

<template>
  <div class="home-page index-body">
    <!-- 开屏大标题 -->
    <div class="splash-title" id="splashTitle">
      <h1 class="title" id="splashH1">jyoushitou</h1>
    </div>

    <!-- 主内容区 -->
    <div class="main-content" id="mainContent">
            <header class="header">
        <!-- 未登录：显示登录/注册按钮 -->
        <div v-if="!apiStore.isLoggedIn" class="auth-buttons">
          <button class="auth-btn" @click="openAuth('login')">登 录</button>
          <button class="auth-btn" @click="openAuth('register')">注 册</button>
        </div>
                <!-- 已登录：显示用户头像和昵称 -->
        <div v-else class="user-menu">
          <img v-if="apiStore.user.avatar" :src="apiStore.user.avatar" class="user-avatar-img" alt="头像" />
          <div v-else class="user-avatar">{{ apiStore.user.name.charAt(0).toUpperCase() }}</div>
          <span class="user-nickname">{{ apiStore.user.name }}</span>
          <button class="auth-btn logout-btn" @click="handleLogout">退出</button>
        </div>
      </header>

      <main>
        <template v-for="(sec, si) in sectionsData" :key="si">
          <h2 class="first-title" :class="{ clickable: sec.title === '文章' }" @click="sec.title === '文章' && goToArticle()">{{ sec.title }}</h2>
          <section class="section">
            <div v-for="(row, ri) in sec.rows" :key="ri" class="h-backgrand" :class="{ clickable: sec.title === '文章' }" @click="sec.title === '文章' && goToArticle()">
              <div class="second-title" :class="{ clickable: sec.title === '文章' }" @click.stop="sec.title === '文章' && goToArticle()">{{ row.label }}</div>
              <div class="content-col">
                <div class="img-row">
                  <div v-for="(img, bi) in row.imgs" :key="bi" class="img-box" :class="{ clickable: sec.title === '文章' }" @click.stop="sec.title === '文章' && goToArticle()"><img :src="img" alt="" /></div>
                </div>
              </div>
            </div>
          </section>
        </template>
      </main>
    </div>

    <!-- 登录/注册弹窗 -->
    <div class="overlay" id="authOverlay" :class="{ show: showAuth }" @click="handleOverlayClick">
      <div class="modal">
        <button class="close-btn" id="closeAuthBtn" @click="closeAuth">✕</button>
        <h2 id="authTitle">{{ authTitle }}</h2>
        <input type="text" id="authUser" placeholder="用户名" />
        <input type="password" id="authPass" placeholder="密码" />
                <input type="password" id="authPass2" placeholder="确认密码" :style="{ display: showPass2 ? 'block' : 'none' }" />
        <div v-if="authError" class="auth-error">{{ authError }}</div>
        <button class="submit-btn" id="authSubmit" @click="handleSubmit" :disabled="authLoading">{{ authLoading ? '处理中...' : authSubmitText }}</button>
        <div class="toggle-auth" id="toggleAuth" @click="toggleAuthMode" v-html="toggleText"></div>
      </div>
    </div>
  </div>
</template>

<style>
/* ================================================================
   全局样式（移植自 common.css）
   ================================================================ */
* { margin: 0; padding: 0; box-sizing: border-box; }
body { overflow-x: hidden; }

/* 弹窗 */
.overlay {
  display: none;
  position: fixed;
  top: 0; left: 0;
  width: 100%; height: 100%;
  background: rgba(0,0,0,0.4);
  z-index: 999;
  justify-content: center;
  align-items: center;
}
.overlay.show { display: flex; }
.modal {
  background: #fff;
  border: 2px solid #000;
  border-radius: 16px;
  padding: 40px 50px;
  position: relative;
  min-width: 320px;
  text-align: center;
}
.modal .close-btn {
  position: absolute;
  top: 12px; right: 16px;
  border: none; background: none;
  font-size: 24px; cursor: pointer;
  color: #999;
}
.modal .close-btn:hover { color: #000; }
.modal h2 { margin-bottom: 20px; font-size: 1.8rem; }
.modal input {
  display: block; width: 100%;
  padding: 10px 14px; margin: 12px 0;
  border: 2px solid #000; border-radius: 8px;
  font-size: 1rem; outline: none;
}
.modal input:focus { border-color: #666; }
.modal .submit-btn {
  width: 100%; padding: 10px; margin-top: 10px;
  border: 2px solid #000; border-radius: 8px;
  background: #000; color: #fff;
  font-size: 1.1rem; cursor: pointer;
  transition: background 0.2s;
}
.modal .submit-btn:hover { background: #333; }
.modal .toggle-auth { margin-top: 14px; font-size: 0.95rem; color: #666; cursor: pointer; }
.modal .toggle-auth span { color: #0066cc; text-decoration: underline; }

/* ================================================================
   首页独有样式（移植自 index.css）
   ================================================================ */
.index-body, .index-body * { font-family: "KaiTi", "STKaiti", serif; }
.index-body {
  padding: 20px;
  background-size: cover;
  background-position: center;
  background-repeat: no-repeat;
  background-attachment: fixed;
  min-height: 100vh;
}

/* header */
.index-body .header {
  display: flex;
  justify-content: flex-end;
  align-items: center;
  margin-bottom: 24px;
  margin-top: 60px;
  opacity: 0;
  transform: translateY(40px);
  transition: opacity 0.6s ease, transform 0.6s ease;
}
.index-body .header.pop-in { opacity: 1; transform: translateY(0); }
.index-body .auth-buttons { display: flex; gap: 14px; }
.index-body .auth-buttons .auth-btn {
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
.index-body .auth-buttons .auth-btn:hover { background: rgba(255,255,255,0.5); }

/* 用户菜单（已登录） */
.index-body .user-menu {
  display: flex;
  align-items: center;
  gap: 12px;
}
.index-body .user-avatar {
  width: 38px;
  height: 38px;
  border-radius: 50%;
  background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
  color: white;
  display: flex;
  align-items: center;
  justify-content: center;
  font-size: 1.1rem;
  font-weight: 700;
  border: 2px solid rgba(255,255,255,0.6);
  box-shadow: 0 2px 8px rgba(0,0,0,0.2);
}
.index-body .user-avatar-img {
  width: 38px;
  height: 38px;
  border-radius: 50%;
  object-fit: cover;
  border: 2px solid rgba(255,255,255,0.6);
  box-shadow: 0 2px 8px rgba(0,0,0,0.2);
}
.index-body .user-nickname {
  color: rgba(255,255,255,0.95);
  font-size: 1.05rem;
  font-weight: 600;
  letter-spacing: 1px;
}
.index-body .logout-btn {
  padding: 6px 18px !important;
  font-size: 0.85rem !important;
  background: rgba(255,255,255,0.2) !important;
  border-color: rgba(255,255,255,0.3) !important;
}
.index-body .logout-btn:hover {
  background: rgba(239, 68, 68, 0.4) !important;
}

/* 登录错误消息 */
.modal .auth-error {
  background: #fef2f2;
  border: 1px solid #fecaca;
  color: #dc2626;
  padding: 8px 12px;
  border-radius: 6px;
  font-size: 0.85rem;
  margin: 8px 0;
  text-align: center;
}
.modal .submit-btn:disabled {
  opacity: 0.6;
  cursor: not-allowed;
}

/* first-title */
.first-title {
  text-align: center;
  padding: 28px 0 12px 0;
  font-size: 1.6rem;
  font-weight: normal;
  color: rgba(255,255,255,0.9);
  letter-spacing: 4px;
  opacity: 0;
  transform: translateY(20px);
  transition: opacity 0.8s cubic-bezier(0.34, 1.56, 0.64, 1),
              transform 0.8s cubic-bezier(0.34, 1.56, 0.64, 1);
}
.first-title.pop-in { opacity: 1; transform: translateY(0); }
.first-title.clickable { cursor: pointer; transition: all 0.3s ease; }
.first-title.clickable:hover { text-shadow: 0 0 20px rgba(255,255,255,0.6); transform: scale(1.05); }
.h-backgrand.clickable { cursor: pointer; transition: all 0.3s ease; }
.h-backgrand.clickable:hover { background: rgba(255,255,255,0.55); }
.second-title.clickable { cursor: pointer; transition: all 0.3s ease; }
.second-title.clickable:hover { text-shadow: 0 0 12px rgba(255,255,255,0.5); }
.img-box.clickable { cursor: pointer; transition: all 0.3s ease; }
.img-box.clickable:hover { transform: scale(1.08); box-shadow: 0 0 15px rgba(255,255,255,0.4); }

/* section */
.section {
  margin-bottom: 12px;
  display: flex;
  flex-direction: column;
  opacity: 0;
  transform: translateY(30px);
  transition: opacity 0.8s cubic-bezier(0.34, 1.56, 0.64, 1),
              transform 0.8s cubic-bezier(0.34, 1.56, 0.64, 1);
}
.section.pop-in { opacity: 1; transform: translateY(0); }

/* h-backgrand */
.h-backgrand {
  display: flex;
  align-items: center;
  gap: 0;
  margin-bottom: 6px;
  background: rgba(255,255,255,0.45);
  border-radius: 6px;
  opacity: 0;
  transform: translateY(15px);
  transition: opacity 0.9s cubic-bezier(0.34, 1.56, 0.64, 1),
              transform 0.9s cubic-bezier(0.34, 1.56, 0.64, 1);
  /* 左右各拉宽 4cm */
  width: calc(100% + 4cm + 4cm);
  margin-left: -4cm;
  padding: 4px 4cm 4px 0;
}
.h-backgrand.pop-in { opacity: 1; transform: translateY(0); }
.h-backgrand.scroll-in {
  opacity: 1;
  transform: translateY(0) scale(1);
  transition: opacity 0.5s ease, transform 0.5s ease;
}
.h-backgrand.scroll-out {
  opacity: 0;
  transform: translateY(-15px) scale(0.5);
  transition: opacity 0.5s ease, transform 0.5s ease;
}

/* second-title */
.second-title {
  min-width: 60px;
  padding: 8px 12px;
  font-size: 1rem;
  flex-shrink: 0;
  display: flex;
  align-items: center;
  justify-content: center;
  color: rgba(255,255,255,0.95);
  white-space: nowrap;
}
.content-col {
  flex: 1;
  padding: 4px 8px;
  font-size: 1rem;
  min-width: 0;
}

/* img-row */
.img-row {
  display: flex;
  gap: 10px;
  overflow-x: auto;
  padding: 2px 0;
  width: 100%;
}
.img-row::-webkit-scrollbar { height: 3px; }
.img-row::-webkit-scrollbar-thumb { background: rgba(255,255,255,0.35); border-radius: 2px; }
.img-box {
  min-width: 100px;
  width: 110px;
  aspect-ratio: 16 / 10;
  border: 1px solid rgba(255,255,255,0.4);
  border-radius: 6px;
  display: flex;
  align-items: center;
  justify-content: center;
  flex-shrink: 0;
  background: rgba(255,255,255,0.3);
  overflow: hidden;
  opacity: 0;
  transform: translateY(12px) scale(0.95);
  transition: opacity 0.4s cubic-bezier(0.34, 1.56, 0.64, 1),
              transform 0.4s cubic-bezier(0.34, 1.56, 0.64, 1);
}
.img-box img {
  width: 100%;
  height: 100%;
  object-fit: cover;
}
.img-box.pop-in { opacity: 1; transform: translateY(0) scale(1); }

/* splash-title */
.splash-title {
  position: fixed;
  top: 0; left: 0;
  width: 100%; height: 100%;
  display: none;
  align-items: center;
  justify-content: center;
  z-index: 1000;
  pointer-events: none;
  user-select: text;
  transition: background 0.5s ease;
}
.splash-title .title {
  font-size: 4.5rem;
  font-weight: normal;
  font-family: "KaiTi", "STKaiti", serif;
  color: rgba(255,255,255,0.9);
  text-shadow: 0 2px 12px rgba(0,0,0,0.2);
  margin: 0;
  position: relative;
}
.splash-title .title::after {
  content: '';
  position: absolute;
  bottom: -8px;
  left: 50%;
  transform: translateX(-50%);
  width: 60px;
  height: 2px;
  background: rgba(255,255,255,0.5);
  border-radius: 1px;
}
.main-content { display: none; }

/* 响应式 */
@media (max-width: 767px) {
  .index-body { padding: 10px; }
  .index-body .auth-buttons .auth-btn { padding: 6px 22px; font-size: 0.95rem; }
  .first-title { font-size: 1.3rem; padding: 20px 0 8px 0; }
  .second-title { min-width: 40px; font-size: 0.9rem; padding: 6px 8px; }
  .content-col { font-size: 0.9rem; }
  .img-box { min-width: 72px; width: 80px; }
}
</style>
