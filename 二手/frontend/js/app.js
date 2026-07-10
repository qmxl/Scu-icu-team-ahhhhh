let currentPage = 1;
let currentCategory = 0;
let currentStatus = '';
let currentMinPrice = '';
let currentMaxPrice = '';
let currentKeyword = '';

// 初始化
document.addEventListener('DOMContentLoaded', () => {
    if (!localStorage.getItem('token')) {
        showLogin();
    } else {
        initApp();
    }
});

// 初始化应用
function initApp() {
    loadCategories();
    loadStats();
    loadLatestProducts();
    showHome();
}

// 显示登录页面
function showLogin() {
    document.body.innerHTML = `
        <div class="login-container">
            <h2>欢迎登录校园二手交易系统</h2>
            <form id="login-form">
                <div class="form-group">
                    <label>学号：</label>
                    <input type="text" id="login-student-id" required>
                </div>
                <div class="form-group">
                    <label>密码：</label>
                    <input type="password" id="login-password" required>
                </div>
                <button type="submit">登录</button>
            </form>
            <div class="switch-link">
                还没有账号？<a href="#" onclick="showRegister()">立即注册</a>
            </div>
        </div>
    `;
    document.getElementById('login-form').addEventListener('submit', handleLogin);
}

// 显示注册页面
function showRegister() {
    document.body.innerHTML = `
        <div class="login-container">
            <h2>用户注册</h2>
            <form id="register-form">
                <div class="form-group">
                    <label>学号：</label>
                    <input type="text" id="register-student-id" required>
                </div>
                <div class="form-group">
                    <label>密码：</label>
                    <input type="password" id="register-password" required>
                </div>
                <div class="form-group">
                    <label>姓名：</label>
                    <input type="text" id="register-name" required>
                </div>
                <div class="form-group">
                    <label>手机号：</label>
                    <input type="text" id="register-phone">
                </div>
                <button type="submit">注册</button>
            </form>
            <div class="switch-link">
                已有账号？<a href="#" onclick="showLogin()">立即登录</a>
            </div>
        </div>
    `;
    document.getElementById('register-form').addEventListener('submit', handleRegister);
}

// 处理登录
async function handleLogin(e) {
    e.preventDefault();
    const data = {
        student_id: document.getElementById('login-student-id').value,
        password: document.getElementById('login-password').value
    };
    const result = await api.login(data);
    if (result.code === 200) {
        localStorage.setItem('token', result.data.token);
        location.reload();
    } else {
        alert(result.message || '登录失败');
    }
}

// 处理注册
async function handleRegister(e) {
    e.preventDefault();
    const data = {
        student_id: document.getElementById('register-student-id').value,
        password: document.getElementById('register-password').value,
        name: document.getElementById('register-name').value,
        phone: document.getElementById('register-phone').value
    };
    const result = await api.register(data);
    if (result.code === 200) {
        alert('注册成功，请登录');
        showLogin();
    } else {
        alert(result.message || '注册失败');
    }
}

// 显示首页
function showHome() {
    hideAllSections();
    document.getElementById('home-section').classList.remove('hidden');
}

// 显示商品列表
function showProducts() {
    hideAllSections();
    document.getElementById('products-section').classList.remove('hidden');
    currentPage = 1;
    loadProducts();
}

// 显示搜索页面
function showSearch() {
    hideAllSections();
    document.getElementById('search-section').classList.remove('hidden');
}

// 显示发布页面
function showPublish() {
    hideAllSections();
    document.getElementById('publish-section').classList.remove('hidden');
}

// 显示订单页面
function showOrders() {
    hideAllSections();
    document.getElementById('orders-section').classList.remove('hidden');
    loadOrders();
}

// 显示收藏页面
function showFavorites() {
    hideAllSections();
    document.getElementById('favorites-section').classList.remove('hidden');
    loadFavorites();
}

// 显示个人中心
function showProfile() {
    hideAllSections();
    document.getElementById('profile-section').classList.remove('hidden');
    loadProfile();
}

// 隐藏所有区块
function hideAllSections() {
    document.querySelectorAll('.section').forEach(s => s.classList.add('hidden'));
}

// 加载统计数据
async function loadStats() {
    const products = await api.getProducts({ page_size: 1 });
    const orders = await api.getOrders({ page_size: 1 });
    document.getElementById('product-count').textContent = products.data?.length || 0;
    document.getElementById('order-count').textContent = orders.data?.length || 0;
    document.getElementById('user-count').textContent = '统计中';
}

// 加载分类
async function loadCategories() {
    const result = await api.getTopLevelCategories();
    if (result.code === 200) {
        const container = document.getElementById('category-list');
        container.innerHTML = '';
        result.data.forEach(cat => {
            const item = document.createElement('div');
            item.className = 'category-item';
            item.innerHTML = `<div>${cat.name}</div><div style="font-size:0.8em;opacity:0.8">${cat.icon}</div>`;
            item.addEventListener('click', () => {
                currentCategory = cat.id;
                showProducts();
            });
            container.appendChild(item);
        });

        // 填充分类选择器
        const selector = document.getElementById('category-filter');
        const publishSelector = document.getElementById('product-category');
        result.data.forEach(cat => {
            selector.innerHTML += `<option value="${cat.id}">${cat.name}</option>`;
            publishSelector.innerHTML += `<option value="${cat.id}">${cat.name}</option>`;
        });
    }
}

// 加载最新商品
async function loadLatestProducts() {
    const result = await api.getProducts({ page_size: 6 });
    if (result.code === 200) {
        renderProducts(result.data, 'latest-products');
    }
}

// 加载商品列表
async function loadProducts() {
    const params = {
        page: currentPage,
        page_size: 12,
        category_id: currentCategory || undefined,
        status: currentStatus || undefined,
        min_price: currentMinPrice || undefined,
        max_price: currentMaxPrice || undefined
    };
    const result = await api.getProducts(params);
    if (result.code === 200) {
        renderProducts(result.data, 'product-list');
        document.getElementById('page-info').textContent = `第 ${currentPage} 页`;
        document.getElementById('prev-btn').disabled = currentPage <= 1;
        document.getElementById('next-btn').disabled = result.data.length < 12;
    }
}

// 渲染商品列表
function renderProducts(products, containerId) {
    const container = document.getElementById(containerId);
    container.innerHTML = '';
    if (!products || products.length === 0) {
        container.innerHTML = '<p style="text-align:center;color:#999;">暂无商品</p>';
        return;
    }
    products.forEach(product => {
        const card = document.createElement('div');
        card.className = 'product-card';
        card.innerHTML = `
            <img src="${product.image_url || 'https://via.placeholder.com/200x150'}" alt="${product.name}">
            <div class="product-info">
                <div class="product-name">${product.name}</div>
                <div class="product-price">¥${product.price}</div>
                <div class="product-seller">发布者: ${product.seller_name || '未知'}</div>
            </div>
        `;
        card.addEventListener('click', () => showProductDetail(product.id));
        container.appendChild(card);
    });
}

// 显示商品详情
async function showProductDetail(productId) {
    const result = await api.getProduct(productId);
    if (result.code === 200) {
        const product = result.data;
        const modal = document.getElementById('product-detail-modal');
        const detail = document.getElementById('product-detail');
        detail.innerHTML = `
            <img src="${product.image_url || 'https://via.placeholder.com/400x300'}" alt="${product.name}">
            <h3>${product.name}</h3>
            <div class="detail-price">¥${product.price}</div>
            <div class="detail-info"><span>成色:</span>${product.condition}</div>
            <div class="detail-info"><span>分类:</span>${product.category_name}</div>
            <div class="detail-info"><span>卖家:</span>${product.seller_name}</div>
            <div class="detail-info"><span>发布时间:</span>${product.created_at}</div>
            <div class="detail-info"><span>描述:</span>${product.description}</div>
            <button onclick="buyProduct(${product.id})">立即购买</button>
            <button onclick="toggleFavorite(${product.id})">收藏/取消收藏</button>
        `;
        modal.classList.remove('hidden');
    }
}

// 关闭弹窗
function closeModal() {
    document.querySelectorAll('.modal').forEach(m => m.classList.add('hidden'));
}

// 购买商品
async function buyProduct(productId) {
    const result = await api.createOrder({ product_id: productId });
    if (result.code === 200) {
        alert('下单成功！');
        closeModal();
        showOrders();
    } else {
        alert(result.message || '下单失败');
    }
}

// 切换收藏状态
async function toggleFavorite(productId) {
    const check = await api.checkFavorite(productId);
    if (check.code === 200) {
        if (check.data.favorited) {
            const result = await api.removeFavorite(productId);
            alert(result.code === 200 ? '已取消收藏' : '操作失败');
        } else {
            const result = await api.addFavorite(productId);
            alert(result.code === 200 ? '收藏成功' : '操作失败');
        }
    }
}

// 搜索商品
async function searchProducts() {
    currentKeyword = document.getElementById('search-keyword').value;
    if (!currentKeyword.trim()) {
        alert('请输入搜索关键词');
        return;
    }
    const result = await api.searchProducts(currentKeyword);
    if (result.code === 200) {
        renderProducts(result.data, 'search-results');
    }
}

// 筛选商品
function filterProducts() {
    currentCategory = document.getElementById('category-filter').value;
    currentStatus = document.getElementById('status-filter').value;
    currentMinPrice = document.getElementById('min-price').value;
    currentMaxPrice = document.getElementById('max-price').value;
    currentPage = 1;
    loadProducts();
}

// 分页
function prevPage() {
    if (currentPage > 1) {
        currentPage--;
        loadProducts();
    }
}

function nextPage() {
    currentPage++;
    loadProducts();
}

// 发布商品
document.getElementById('publish-form')?.addEventListener('submit', async (e) => {
    e.preventDefault();
    const data = {
        name: document.getElementById('product-name').value,
        description: document.getElementById('product-description').value,
        category_id: parseInt(document.getElementById('product-category').value),
        price: parseFloat(document.getElementById('product-price').value),
        image_url: document.getElementById('product-image').value,
        condition: document.getElementById('product-condition').value
    };
    const result = await api.createProduct(data);
    if (result.code === 200) {
        alert('发布成功！');
        document.getElementById('publish-form').reset();
        showProducts();
    } else {
        alert(result.message || '发布失败');
    }
});

// 加载订单
async function loadOrders() {
    const status = document.getElementById('order-status-filter').value;
    const type = document.getElementById('order-type').value;
    const params = {
        status: status || undefined,
        type: type
    };
    const result = await api.getOrders(params);
    if (result.code === 200) {
        renderOrders(result.data);
    }
}

// 渲染订单列表
function renderOrders(orders) {
    const container = document.getElementById('order-list');
    container.innerHTML = '';
    if (!orders || orders.length === 0) {
        container.innerHTML = '<p style="text-align:center;color:#999;">暂无订单</p>';
        return;
    }
    orders.forEach(order => {
        const item = document.createElement('div');
        item.className = 'order-item';
        item.innerHTML = `
            <div class="order-header">
                <span>订单号: ${order.order_no}</span>
                <span class="order-status ${order.status}">${getStatusText(order.status)}</span>
            </div>
            <div class="order-info">
                <div><span>商品:</span>${order.product_name}</div>
                <div><span>价格:</span>¥${order.price}</div>
                <div><span>创建时间:</span>${order.created_at}</div>
            </div>
            <div style="margin-top:15px;">
                ${order.type === 'buyer' && order.status === 'pending' ? 
                    '<button onclick="updateOrderStatus(' + order.id + ', \'confirmed\')">确认收货</button>' : ''}
                ${order.type === 'seller' && order.status === 'pending' ? 
                    '<button onclick="updateOrderStatus(' + order.id + ', \'confirmed\')">确认订单</button>' : ''}
                ${order.status === 'confirmed' && order.type === 'buyer' ? 
                    '<button onclick="updateOrderStatus(' + order.id + ', \'completed\')">完成交易</button>' : ''}
            </div>
        `;
        container.appendChild(item);
    });
}

// 获取状态文本
function getStatusText(status) {
    const map = {
        'pending': '待确认',
        'confirmed': '已确认',
        'completed': '已完成',
        'cancelled': '已取消'
    };
    return map[status] || status;
}

// 更新订单状态
async function updateOrderStatus(orderId, status) {
    const result = await api.updateOrderStatus(orderId, status);
    if (result.code === 200) {
        alert('状态更新成功');
        loadOrders();
    } else {
        alert(result.message || '操作失败');
    }
}

// 加载收藏列表
async function loadFavorites() {
    const result = await api.getFavorites();
    if (result.code === 200) {
        const container = document.getElementById('favorite-list');
        container.innerHTML = '';
        if (!result.data || result.data.length === 0) {
            container.innerHTML = '<p style="text-align:center;color:#999;">暂无收藏</p>';
            return;
        }
        result.data.forEach(fav => {
            api.getProduct(fav.product_id).then(res => {
                if (res.code === 200) {
                    const product = res.data;
                    const card = document.createElement('div');
                    card.className = 'product-card';
                    card.innerHTML = `
                        <img src="${product.image_url || 'https://via.placeholder.com/200x150'}" alt="${product.name}">
                        <div class="product-info">
                            <div class="product-name">${product.name}</div>
                            <div class="product-price">¥${product.price}</div>
                        </div>
                    `;
                    card.addEventListener('click', () => showProductDetail(product.id));
                    container.appendChild(card);
                }
            });
        });
    }
}

// 加载个人资料
async function loadProfile() {
    const result = await api.getProfile();
    if (result.code === 200) {
        const profile = result.data;
        document.getElementById('user-profile').innerHTML = `
            <p><strong>学号:</strong> ${profile.student_id}</p>
            <p><strong>姓名:</strong> ${profile.name}</p>
            <p><strong>手机号:</strong> ${profile.phone || '未填写'}</p>
            <p><strong>宿舍地址:</strong> ${profile.dormitory || '未填写'}</p>
            <p><strong>注册时间:</strong> ${profile.created_at}</p>
        `;
    }
}

// 显示编辑资料弹窗
function showEditProfile() {
    const modal = document.getElementById('edit-profile-modal');
    modal.classList.remove('hidden');
}

// 保存资料修改
document.getElementById('edit-profile-form')?.addEventListener('submit', async (e) => {
    e.preventDefault();
    const data = {
        name: document.getElementById('edit-name').value,
        phone: document.getElementById('edit-phone').value,
        dormitory: document.getElementById('edit-dormitory').value
    };
    const result = await api.updateProfile(data);
    if (result.code === 200) {
        alert('修改成功');
        closeModal();
        loadProfile();
    } else {
        alert(result.message || '修改失败');
    }
});

// 显示我的商品
function showMyProducts() {
    hideAllSections();
    document.getElementById('products-section').classList.remove('hidden');
    api.getMyProducts().then(result => {
        if (result.code === 200) {
            renderProducts(result.data, 'product-list');
        }
    });
}

// 退出登录
function logout() {
    localStorage.removeItem('token');
    location.reload();
}