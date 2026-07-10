const API_BASE = 'http://127.0.0.1:8080/api';

function getToken() {
    return localStorage.getItem('token');
}

function setToken(token) {
    localStorage.setItem('token', token);
}

function removeToken() {
    localStorage.removeItem('token');
}

function setUser(user) {
    localStorage.setItem('user', JSON.stringify(user));
}

function getUser() {
    const user = localStorage.getItem('user');
    return user ? JSON.parse(user) : null;
}

function removeUser() {
    localStorage.removeItem('user');
}

function isLoggedIn() {
    return !!getToken();
}

function request(method, url, data = null) {
    const options = {
        method: method,
        headers: {
            'Content-Type': 'application/json'
        }
    };
    
    const token = getToken();
    if (token) {
        options.headers['Authorization'] = 'Bearer ' + token;
    }
    
    if (data) {
        options.body = JSON.stringify(data);
    }
    
    return fetch(API_BASE + url, options)
        .then(response => response.json())
        .then(result => {
            if (result.code === 1 && result.message === '未登录') {
                logout();
                window.location.href = 'login.html';
            }
            return result;
        });
}

function login(username, password) {
    return request('POST', '/user/login', { username, password });
}

function register(username, password, phone, qq) {
    return request('POST', '/user/register', { username, password, phone, qq });
}

function getProfile(id) {
    return request('GET', `/user/profile?id=${id}`);
}

function updateProfile(id, phone, qq) {
    return request('POST', '/user/update', { id, phone, qq });
}

function getUserList(page, pageSize) {
    return request('GET', `/user/list?page=${page}&pageSize=${pageSize}`);
}

function updateUserStatus(id, status) {
    return request('POST', '/user/status', { id, status });
}

function getGoodsList(page, pageSize, category = '', keyword = '', sellerId = 0) {
    let url = `/goods/list?page=${page}&pageSize=${pageSize}`;
    if (category) url += `&category=${encodeURIComponent(category)}`;
    if (keyword) url += `&keyword=${encodeURIComponent(keyword)}`;
    if (sellerId > 0) url += `&sellerId=${sellerId}`;
    return request('GET', url);
}

function getGoodsDetail(id) {
    return request('GET', `/goods/detail?id=${id}`);
}

function createGoods(data) {
    return request('POST', '/goods/publish', data);
}

function updateGoods(data) {
    return request('POST', '/goods/update', data);
}

function deleteGoods(id) {
    return request('POST', '/goods/delete', { id });
}

function changeGoodsStatus(id, status) {
    return request('POST', '/goods/status', { id, status });
}

function toggleLock(id, isLocked) {
    return request('POST', '/goods/lock', { id, is_locked: isLocked });
}

function applyTransaction(goods_id, buyer_id, seller_id) {
    return request('POST', '/transaction/create', { goods_id, buyer_id, seller_id });
}

function confirmTransaction(transaction_id) {
    return request('POST', '/transaction/confirm', { id: transaction_id });
}

function completeTransaction(transaction_id) {
    return request('POST', '/transaction/complete', { id: transaction_id });
}

function cancelTransaction(transaction_id, reason) {
    return request('POST', '/transaction/cancel', { id: transaction_id, reason });
}

function getTransactionList(userId, type, page, pageSize) {
    return request('GET', `/transaction/list?userId=${userId}&type=${type}&page=${page}&pageSize=${pageSize}`);
}

function getConfigList() {
    return request('GET', '/config/list');
}

function updateConfig(key, value, description) {
    return request('POST', '/config/update', { key, value, description });
}

function getStatistics(type, year = new Date().getFullYear()) {
    return request('GET', `/statistics/data?type=${type}&year=${year}`);
}

function logout() {
    removeToken();
    removeUser();
}