const API_BASE = 'http://localhost:8080/api/v1';

const api = {
    // 用户相关
    register: (data) => fetch(`${API_BASE}/users/register`, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify(data)
    }).then(res => res.json()),

    login: (data) => fetch(`${API_BASE}/users/login`, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify(data)
    }).then(res => res.json()),

    getProfile: () => fetch(`${API_BASE}/users/profile`, {
        headers: { 'Authorization': `Bearer ${localStorage.getItem('token')}` }
    }).then(res => res.json()),

    updateProfile: (data) => fetch(`${API_BASE}/users/profile`, {
        method: 'PUT',
        headers: { 
            'Content-Type': 'application/json',
            'Authorization': `Bearer ${localStorage.getItem('token')}`
        },
        body: JSON.stringify(data)
    }).then(res => res.json()),

    // 商品相关
    getProducts: (params = {}) => {
        const query = new URLSearchParams(params).toString();
        return fetch(`${API_BASE}/products?${query}`, {
            headers: { 'Authorization': `Bearer ${localStorage.getItem('token')}` }
        }).then(res => res.json());
    },

    getProduct: (id) => fetch(`${API_BASE}/products/${id}`, {
        headers: { 'Authorization': `Bearer ${localStorage.getItem('token')}` }
    }).then(res => res.json()),

    createProduct: (data) => fetch(`${API_BASE}/products`, {
        method: 'POST',
        headers: { 
            'Content-Type': 'application/json',
            'Authorization': `Bearer ${localStorage.getItem('token')}`
        },
        body: JSON.stringify(data)
    }).then(res => res.json()),

    updateProduct: (id, data) => fetch(`${API_BASE}/products/${id}`, {
        method: 'PUT',
        headers: { 
            'Content-Type': 'application/json',
            'Authorization': `Bearer ${localStorage.getItem('token')}`
        },
        body: JSON.stringify(data)
    }).then(res => res.json()),

    deleteProduct: (id) => fetch(`${API_BASE}/products/${id}`, {
        method: 'DELETE',
        headers: { 'Authorization': `Bearer ${localStorage.getItem('token')}` }
    }).then(res => res.json()),

    searchProducts: (keyword, page = 1, pageSize = 10) => fetch(
        `${API_BASE}/products/search?keyword=${encodeURIComponent(keyword)}&page=${page}&page_size=${pageSize}`, {
        headers: { 'Authorization': `Bearer ${localStorage.getItem('token')}` }
    }).then(res => res.json()),

    getProductsByCategory: (categoryId, page = 1, pageSize = 10) => fetch(
        `${API_BASE}/products/category/${categoryId}?page=${page}&page_size=${pageSize}`, {
        headers: { 'Authorization': `Bearer ${localStorage.getItem('token')}` }
    }).then(res => res.json()),

    getMyProducts: () => fetch(`${API_BASE}/products/my`, {
        headers: { 'Authorization': `Bearer ${localStorage.getItem('token')}` }
    }).then(res => res.json()),

    // 分类相关
    getCategories: () => fetch(`${API_BASE}/categories`, {
        headers: { 'Authorization': `Bearer ${localStorage.getItem('token')}` }
    }).then(res => res.json()),

    getTopLevelCategories: () => fetch(`${API_BASE}/categories/top`, {
        headers: { 'Authorization': `Bearer ${localStorage.getItem('token')}` }
    }).then(res => res.json()),

    // 订单相关
    createOrder: (data) => fetch(`${API_BASE}/orders`, {
        method: 'POST',
        headers: { 
            'Content-Type': 'application/json',
            'Authorization': `Bearer ${localStorage.getItem('token')}`
        },
        body: JSON.stringify(data)
    }).then(res => res.json()),

    getOrders: (params = {}) => {
        const query = new URLSearchParams(params).toString();
        return fetch(`${API_BASE}/orders?${query}`, {
            headers: { 'Authorization': `Bearer ${localStorage.getItem('token')}` }
        }).then(res => res.json());
    },

    getOrder: (id) => fetch(`${API_BASE}/orders/${id}`, {
        headers: { 'Authorization': `Bearer ${localStorage.getItem('token')}` }
    }).then(res => res.json()),

    updateOrderStatus: (id, status) => fetch(`${API_BASE}/orders/${id}/status`, {
        method: 'PUT',
        headers: { 
            'Content-Type': 'application/json',
            'Authorization': `Bearer ${localStorage.getItem('token')}`
        },
        body: JSON.stringify({ status })
    }).then(res => res.json()),

    // 收藏相关
    addFavorite: (productId) => fetch(`${API_BASE}/favorites`, {
        method: 'POST',
        headers: { 
            'Content-Type': 'application/json',
            'Authorization': `Bearer ${localStorage.getItem('token')}`
        },
        body: JSON.stringify({ product_id: productId })
    }).then(res => res.json()),

    getFavorites: (page = 1, pageSize = 10) => fetch(
        `${API_BASE}/favorites?page=${page}&page_size=${pageSize}`, {
        headers: { 'Authorization': `Bearer ${localStorage.getItem('token')}` }
    }).then(res => res.json()),

    removeFavorite: (productId) => fetch(`${API_BASE}/favorites/${productId}`, {
        method: 'DELETE',
        headers: { 'Authorization': `Bearer ${localStorage.getItem('token')}` }
    }).then(res => res.json()),

    checkFavorite: (productId) => fetch(`${API_BASE}/favorites/${productId}/check`, {
        headers: { 'Authorization': `Bearer ${localStorage.getItem('token')}` }
    }).then(res => res.json())
};