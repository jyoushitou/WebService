// ========== ☰ 导航展开/收起（在标题行展开） ==========
const menuBtn = document.getElementById('menuBtn');
const navDropdown = document.getElementById('navDropdown');
const headerRight = document.getElementById('headerRight');

if (menuBtn && navDropdown) {
    menuBtn.addEventListener('click', () => {
        const isOpen = navDropdown.classList.toggle('show');
        // 展开时隐藏 ☰ 按钮，收起时恢复
        menuBtn.style.display = isOpen ? 'none' : 'inline-flex';
        // 展开时隐藏右侧登录按钮，收起时恢复
        if (headerRight) {
            headerRight.style.display = isOpen ? 'none' : 'flex';
        }
    });

    // 关闭函数
    function closeNav() {
        navDropdown.classList.remove('show');
        menuBtn.style.display = 'inline-flex';
        if (headerRight) {
            headerRight.style.display = 'flex';
        }
    }

    // 点击导航项后收起
    navDropdown.querySelectorAll('.nav-box').forEach(box => {
        box.addEventListener('click', closeNav);
    });

    // 点击 ✕ 关闭
    const closeBtn = document.getElementById('navCloseBtn');
    if (closeBtn) closeBtn.addEventListener('click', closeNav);
}

// ========== 手机端章节目录 展开/收起 ==========
const chapterToggle = document.getElementById('chapterToggle');
const chapterBox = document.getElementById('mobileChapter');

if (chapterToggle) {
    chapterToggle.addEventListener('click', () => {
        chapterBox.classList.toggle('show');
        chapterToggle.textContent = chapterBox.classList.contains('show') ? '收起章节目录' : '展开章节目录';
    });
}

// ========== 开屏标题动画 + 内容弹出 ==========
document.addEventListener('DOMContentLoaded', function() {
    const splash = document.getElementById('articleSplash');
    const h1 = document.getElementById('articleSplashH1');

    if (splash && h1) {
        splash.style.display = 'flex';
        splash.style.background = 'rgba(0,0,0,0.3)';
        h1.style.transition = 'all 1.2s ease';
        h1.style.fontSize = '5rem';
        h1.style.opacity = '1';
        h1.style.transform = 'translateY(0)';

        requestAnimationFrame(function() {
            requestAnimationFrame(function() {
                h1.style.transform = 'translateY(calc(-50vh + 40px)) scale(0.5)';
                splash.style.background = 'transparent';
            });
        });

        setTimeout(function() {
            document.getElementById('mainContent').style.display = 'block';

            const header = document.querySelector('.header');
            if (header) header.classList.add('pop-in');

            setTimeout(function() {
                const breadcrumb = document.querySelector('.breadcrumb');
                if (breadcrumb) breadcrumb.classList.add('pop-in');
            }, 200);

            setTimeout(function() {
                const sidebar = document.querySelector('.sidebar');
                if (sidebar) sidebar.classList.add('pop-in');
                const content = document.querySelector('.content-area');
                if (content) content.classList.add('pop-in');
            }, 400);

            setTimeout(function() {
                splash.style.display = 'none';
            }, 100);
        }, 1200);
    } else {
        document.getElementById('mainContent').style.display = 'block';
        const header = document.querySelector('.header');
        if (header) header.classList.add('pop-in');
    }
});
