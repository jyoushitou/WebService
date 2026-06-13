/**
 * router/index.js — Vue Router 路由配置
 * 
 * 使用 HTML5 History 模式（无 # 号），配置四个页面路由：
 *   /          → HomeView      首页
 *   /article   → ArticleView   文章阅读
 *   /api-demo  → ApiDemoView   API 接口测试
 *   /about     → AboutView     项目介绍
 */

import { createRouter, createWebHistory } from 'vue-router'
import HomeView from '@/views/HomeView.vue'
import ApiDemoView from '@/views/ApiDemoView.vue'
import AboutView from '@/views/AboutView.vue'
import ArticleView from '@/views/ArticleView.vue'

const router = createRouter({
  history: createWebHistory(import.meta.env.BASE_URL),
  routes: [
    {
      path: '/',
      name: 'home',
      component: HomeView
    },
    {
      path: '/article',
      name: 'article',
      component: ArticleView
    },
    {
      path: '/api-demo',
      name: 'api-demo',
      component: ApiDemoView
    },
    {
      path: '/about',
      name: 'about',
      component: AboutView
    }
  ]
})

export default router