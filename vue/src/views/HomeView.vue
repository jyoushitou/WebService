/**
 * HomeView.vue — 首页
 * 
 * 展示四个板块（文章/图片/视频/博客），每个板块包含三个标签和若干图片。
 * 页面加载时显示开屏动画，内容逐渐弹出，并且支持滚动时的入场/退场动画。
 * 登录/注册弹窗集成在首页中。
 */

<script setup>
import { ref, onMounted, onBeforeUnmount, nextTick, watch } from 'vue'
import { useRouter } from 'vue-router'
import UserNav from '@/components/UserNav.vue'

const router = useRouter()

function goToArticle() {
  router.push('/article')
}

// ─── 页面数据（从后端 API 获取，失败用默认值） ───
const sectionsData = ref([
  { title: '文章', rows: [
    { label: '标签1', imgs: ['/image/129442526_p0.png','/image/88515515_p0.png','/image/129442526_p0.png','/image/88515515_p0.png','/image/129442526_p0.png','/image/88515515_p0.png'] },
    { label: '标签2', imgs: ['/image/129442526_p0.png','/image/88515515_p0.png','/image/129442526_p0.png','/image/88515515_p0.png','/image/129442526_p0.png'] },
    { label: '标签3', imgs: ['/image/129442526_p0.png','/image/88515515_p0.png','/image/129442526_p0.png','/image/88515515_p0.png'] }
  ]},
  { title: '图片', rows: [
    { label: '标签1', imgs: ['/image/129442526_p0.png','/image/88515515_p0.png','/image/129442526_p0.png','/image/88515515_p0.png','/image/129442526_p0.png','/image/88515515_p0.png'] },
    { label: '标签2', imgs: ['/image/129442526_p0.png','/image/88515515_p0.png','/image/129442526_p0.png','/image/88515515_p0.png','/image/129442526_p0.png'] },
    { label: '标签3', imgs: ['/image/129442526_p0.png','/image/88515515_p0.png','/image/129442526_p0.png','/image/88515515_p0.png'] }
  ]},
  { title: '视频', rows: [
    { label: '标签1', imgs: ['/image/129442526_p0.png','/image/88515515_p0.png','/image/129442526_p0.png','/image/88515515_p0.png','/image/129442526_p0.png','/image/88515515_p0.png'] },
    { label: '标签2', imgs: ['/image/129442526_p0.png','/image/88515515_p0.png','/image/129442526_p0.png','/image/88515515_p0.png','/image/129442526_p0.png'] },
    { label: '标签3', imgs: ['/image/129442526_p0.png','/image/88515515_p0.png','/image/129442526_p0.png','/image/88515515_p0.png'] }
  ]},
    { title: '博客', rows: [
    { label: '标签1', imgs: ['/image/129442526_p0.png','/image/88515515_p0.png','/image/129442526_p0.png','/image/88515515_p0.png','/image/129442526_p0.png','/image/88515515_p0.png'] },
    { label: '标签2', imgs: ['/image/129442526_p0.png','/image/88515515_p0.png','/image/129442526_p0.png','/image/88515515_p0.png','/image/129442526_p0.png'] },
    { label: '标签3', imgs: ['/image/129442526_p0.png','/image/88515515_p0.png','/image/129442526_p0.png','/image/88515515_p0.png'] },
    { label: '空标签(无图片)', imgs: [] }
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
        <UserNav />
      </header>

      <main>
        <template v-for="(sec, si) in sectionsData" :key="si">
          <h2 class="first-title" :class="{ clickable: sec.title === '文章' }" @click="sec.title === '文章' && goToArticle()">{{ sec.title }}</h2>
          <section class="section">
                        <template v-for="(row, ri) in sec.rows" :key="ri">
                          <div v-if="row.imgs && row.imgs.length > 0" class="h-backgrand" :class="{ clickable: sec.title === '文章' }" @click="sec.title === '文章' && goToArticle()">
                            <div class="second-title" :class="{ clickable: sec.title === '文章' }" @click.stop="sec.title === '文章' && goToArticle()">{{ row.label }}</div>
                            <div class="content-col">
                              <div class="img-row">
                                <div v-for="(img, bi) in row.imgs.slice(0, 10)" :key="bi" class="img-box" :class="{ clickable: sec.title === '文章' }" @click.stop="sec.title === '文章' && goToArticle()"><img :src="img" alt="" /></div>
                              </div>
                            </div>
                          </div>
                        </template>
          </section>
        </template>
      </main>
        </div>
  </div>
</template>

<style>
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
