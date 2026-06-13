/* ================================================================
   common.js
   ================================================================ */

var isLogin = true;

function openAuth(mode) {
    var overlay = document.getElementById('authOverlay');
    var title = document.getElementById('authTitle');
    var submit = document.getElementById('authSubmit');
    var toggle = document.getElementById('toggleAuth');
    var pass2 = document.getElementById('authPass2');

    isLogin = (mode === 'login');
    title.textContent = isLogin ? '登 录' : '注 册';
    submit.textContent = isLogin ? '登 录' : '注 册';
    toggle.innerHTML = isLogin
        ? '还没有账号？<span>立即注册</span>'
        : '已有账号？<span>去登录</span>';
    pass2.style.display = isLogin ? 'none' : 'block';
    overlay.classList.add('show');
    document.getElementById('authUser').value = '';
    document.getElementById('authPass').value = '';
    document.getElementById('authPass2').value = '';
}

function closeAuth() {
    document.getElementById('authOverlay').classList.remove('show');
}

document.addEventListener('DOMContentLoaded', function() {

    // ===== 1. 随机背景 =====
    var bgList = [
        'url("./image/129442526_p0.png")',
        'url("./image/88515515_p0.png")'
    ];
    var bg = bgList[Math.floor(Math.random() * bgList.length)];
    var ib = document.querySelector('.index-body');
    if (ib) ib.style.backgroundImage = bg;
    var ab = document.querySelector('.article-body');
    if (ab) ab.style.backgroundImage = bg;

    // ===== 2. 开屏大标题 =====
    // 预先获取所有 h-backgrand（后面开屏和 observer 都用）
    var allBgs = document.querySelectorAll('.h-backgrand');

    var splash = document.getElementById('splashTitle');
    var h1 = document.getElementById('splashH1');

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

        // 开屏结束后
        setTimeout(function() {
            document.getElementById('mainContent').style.display = 'block';

            var header = document.querySelector('.header');
            var firstTitles = document.querySelectorAll('.first-title');
            var sections = document.querySelectorAll('.section');

            // ===== 第一批：header + 所有 first-title + 板块 同时弹出 =====
            if (header) header.classList.add('pop-in');

            for (var ti = 0; ti < firstTitles.length; ti++) {
                firstTitles[ti].classList.add('pop-in');
                if (sections[ti]) {
                    sections[ti].classList.add('pop-in');
                }
            }

            // ===== 第二批：h-backgrand 和图片框混合逐个弹出 =====
            var maxRows = 0;
            for (var si = 0; si < sections.length; si++) {
                var rc = sections[si].querySelectorAll('.h-backgrand').length;
                if (rc > maxRows) maxRows = rc;
            }

            var delay = 600;
            for (var rj = 0; rj < maxRows; rj++) {
                (function(rowIdx, d) {
                    setTimeout(function() {
                        for (var siA = 0; siA < sections.length; siA++) {
                            var bgsA = sections[siA].querySelectorAll('.h-backgrand');
                            if (bgsA[rowIdx]) {
                                bgsA[rowIdx].classList.add('pop-in');
                            }
                        }
                    }, d);
                })(rj, delay);

                var maxImg = 0;
                for (var si2 = 0; si2 < sections.length; si2++) {
                    var bgs2 = sections[si2].querySelectorAll('.h-backgrand');
                    if (bgs2[rj]) {
                        var ic = bgs2[rj].querySelectorAll('.img-box').length;
                        if (ic > maxImg) maxImg = ic;
                    }
                }
                for (var bj = 0; bj < maxImg; bj++) {
                    (function(rowIdx, boxIdx, d) {
                        setTimeout(function() {
                            for (var si3 = 0; si3 < sections.length; si3++) {
                                var bgs3 = sections[si3].querySelectorAll('.h-backgrand');
                                if (bgs3[rowIdx]) {
                                    var boxes = bgs3[rowIdx].querySelectorAll('.img-box');
                                    if (boxes[boxIdx]) {
                                        boxes[boxIdx].classList.add('pop-in');
                                    }
                                }
                            }
                        }, d);
                    })(rj, bj, delay + 50 + bj * 80);
                }
                delay += maxImg * 80 + 50;
            }

            // 全部动画完成后启动 observer（不影响开屏弹出）
            var totalDelay = delay;
            setTimeout(function() {
                startObserver();
            }, totalDelay + 500);

        }, 1300);
    }

    // ===== 3. 滚动视口观察（h-backgrand 进出动画） =====
    var observer = null;

    function startObserver() {
        observer = new IntersectionObserver(function(entries) {
            entries.forEach(function(entry) {
                if (entry.isIntersecting) {
                    // 进入视口：从小到大弹出（移除 pop-in 避免冲突）
                    entry.target.classList.remove('pop-in', 'scroll-out');
                    entry.target.classList.add('scroll-in');
                } else {
                    // 离开视口：缩小消失
                    entry.target.classList.remove('scroll-in');
                    entry.target.classList.add('scroll-out');
                }
            });
        }, { threshold: 0.1 });

        allBgs.forEach(function(bg) {
            observer.observe(bg);
        });
    }

    // ===== 4. 登录/注册 =====
    var btns = document.querySelectorAll('.auth-btn');
    btns.forEach(function(btn, idx) {
        btn.addEventListener('click', function() { openAuth(idx === 0 ? 'login' : 'register'); });
    });
    var closeBtn = document.getElementById('closeAuthBtn');
    if (closeBtn) closeBtn.addEventListener('click', closeAuth);
    var overlay = document.getElementById('authOverlay');
    if (overlay) overlay.addEventListener('click', function(e) { if (e.target === overlay) closeAuth(); });
    var toggle = document.getElementById('toggleAuth');
    if (toggle) toggle.addEventListener('click', function() { openAuth(isLogin ? 'register' : 'login'); });
    var submit = document.getElementById('authSubmit');
    if (submit) {
        submit.addEventListener('click', function() {
            var user = document.getElementById('authUser').value;
            var pass = document.getElementById('authPass').value;
            if (!user || !pass) { alert('请填写完整信息'); return; }
            if (!isLogin) {
                var pass2 = document.getElementById('authPass2').value;
                if (pass !== pass2) { alert('两次密码不一致'); return; }
                alert('注册成功！');
            } else { alert('登录成功！'); }
            closeAuth();
        });
    }
});
