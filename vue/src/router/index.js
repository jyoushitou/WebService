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