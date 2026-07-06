#include "module_order.h"

vector<Order> orders;
vector<Address> addresses;

// ===== 加载订单 =====
void loadOrders() {
    ifstream file("data/orders.txt");
    if (!file) return;
    int id, bid, sid, gid; double amt; string st, addr; time_t ct, pt;
    while (file >> id >> bid >> sid >> gid >> amt >> st >> addr >> ct >> pt) {
        Order o;
        o.id = id; o.buyerId = bid; o.sellerId = sid; o.goodsId = gid;
        o.amount = amt; o.status = st; o.address = addr;
        o.createTime = ct; o.payTime = pt;
        orders.push_back(o);
    }
    file.close();
}

// ===== 保存订单 =====
void saveOrders() {
    ofstream file("data/orders.txt");
    for (auto& o : orders) {
        file << o.id << " " << o.buyerId << " " << o.sellerId << " "
            << o.goodsId << " " << o.amount << " " << o.status << " "
            << o.address << " " << o.createTime << " " << o.payTime << "\\n";
    }
    file.close();
}

// ===== 加载地址 =====
void loadAddresses() {
    ifstream file("data/addresses.txt");
    if (!file) return;
    int id, uid; string r, p, c, d, det; bool def;
    while (file >> id >> uid >> r >> p >> c >> d >> det >> def) {
        Address a;
        a.id = id; a.userId = uid; a.receiver = r; a.phone = p;
        a.province = p; a.city = c; a.district = d; a.detail = det;
        a.isDefault = def;
        addresses.push_back(a);
    }
    file.close();
}

// ===== 保存地址 =====
void saveAddresses() {
    ofstream file("data/addresses.txt");
    for (auto& a : addresses) {
        file << a.id << " " << a.userId << " " << a.receiver << " "
            << a.phone << " " << a.province << " " << a.city << " "
            << a.district << " " << a.detail << " " << a.isDefault << "\\n";
    }
    file.close();
}

// ===== 创建订单 =====
void createOrder() {
    if (currentUserId == -1) {
        cout << "请先登录！\\n";
        pauseScreen();
        return;
    }

    clearScreen();
    cout << "\\n========== 创建订单 ==========\\n";
    int gid;
    cout << "请输入要购买的商品ID: ";
    cin >> gid;

    Goods* target = nullptr;
    for (auto& g : goods) {
        if (g.id == gid && g.status == "on_sale") {
            target = &g;
            break;
        }
    }
    if (!target) {
        cout << "商品不存在或已下架！\\n";
        pauseScreen();
        return;
    }
    if (target->userId == currentUserId) {
        cout << "不能购买自己的商品！\\n";
        pauseScreen();
        return;
    }

    bool hasAddr = false;
    for (auto& a : addresses) {
        if (a.userId == currentUserId) { hasAddr = true; break; }
    }
    if (!hasAddr) {
        cout << "请先添加收货地址！\\n";
        pauseScreen();
        return;
    }

    Order o;
    o.id = orders.empty() ? 1 : orders.back().id + 1;
    o.buyerId = currentUserId;
    o.sellerId = target->userId;
    o.goodsId = gid;
    o.amount = stod(target->price_new);
    o.status = "pending";

    cout << "选择收货地址:\\n";
    for (auto& a : addresses) {
        if (a.userId == currentUserId) {
            cout << a.id << ". " << a.receiver << " " << a.phone << " "
                << a.province << a.city << a.district << a.detail;
            if (a.isDefault) cout << " (默认)";
            cout << "\\n";
        }
    }
    int addrId;
    cout << "请选择: ";
    cin >> addrId;
    for (auto& a : addresses) {
        if (a.id == addrId && a.userId == currentUserId) {
            o.address = a.province + a.city + a.district + a.detail;
            break;
        }
    }

    time(&o.createTime);
    orders.push_back(o);
    saveOrders();
    cout << "订单创建成功！订单号: " << o.id << "\\n";
    pauseScreen();
}

// ===== 支付订单 =====
void payOrder() {
    if (currentUserId == -1) {
        cout << "请先登录！\\n";
        pauseScreen();
        return;
    }

    clearScreen();
    cout << "\\n========== 支付订单 ==========\\n";
    int oid;
    cout << "请输入要支付的订单号: ";
    cin >> oid;

    for (auto& o : orders) {
        if (o.id == oid && o.buyerId == currentUserId && o.status == "pending") {
            o.status = "paid";
            time(&o.payTime);

            for (auto& g : goods) {
                if (g.id == o.goodsId) {
                    g.status = "sold";
                    break;
                }
            }
            saveOrders();
            saveGoods();
            cout << "支付成功！\\n";
            pauseScreen();
            return;
        }
    }
    cout << "订单不存在或状态异常！\\n";
    pauseScreen();
}

// ===== 我的订单 =====
void myOrders() {
    if (currentUserId == -1) {
        cout << "请先登录！\\n";
        pauseScreen();
        return;
    }

    clearScreen();
    cout << "\\n========== 我的订单 ==========\\n";
    cout << "1. 作为买家\\n";
    cout << "2. 作为卖家\\n";
    cout << "0. 返回\\n";
    cout << "请选择: ";

    int choice; cin >> choice;
    if (choice == 0) return;

    bool hasOrder = false;
    for (auto& o : orders) {
        if ((choice == 1 && o.buyerId == currentUserId) ||
            (choice == 2 && o.sellerId == currentUserId)) {
            hasOrder = true;
            cout << "订单号:" << o.id << " | 金额:¥" << o.amount
                << " | 状态:" << o.status << "\\n";
        }
    }
    if (!hasOrder) cout << "暂无订单\\n";
    pauseScreen();
}

// ===== 地址管理 =====
void manageAddress() {
    if (currentUserId == -1) {
        cout << "请先登录！\\n";
        pauseScreen();
        return;
    }

    clearScreen();
    cout << "\\n========== 地址管理 ==========\\n";
    cout << "1. 添加地址\\n";
    cout << "2. 查看地址\\n";
    cout << "3. 设为默认\\n";
    cout << "0. 返回\\n";
    cout << "请选择: ";

    int choice; cin >> choice;
    switch (choice) {
    case 1: {
        Address a;
        a.id = addresses.empty() ? 1 : addresses.back().id + 1;
        a.userId = currentUserId;
        cout << "收货人: "; cin >> a.receiver;
        cout << "手机号: "; cin >> a.phone;
        cout << "省: "; cin >> a.province;
        cout << "市: "; cin >> a.city;
        cout << "区: "; cin >> a.district;
        cout << "详细地址: "; cin.ignore(); getline(cin, a.detail);
        char def;
        cout << "设为默认?(y/n): "; cin >> def;
        if (def == 'y' || def == 'Y') {
            for (auto& addr : addresses) {
                if (addr.userId == currentUserId) addr.isDefault = false;
            }
            a.isDefault = true;
        }
        addresses.push_back(a);
        saveAddresses();
        cout << "添加成功！\\n";
        pauseScreen();
        break;
    }
    case 2: {
        for (auto& a : addresses) {
            if (a.userId == currentUserId) {
                cout << a.id << ". " << a.receiver << " " << a.phone << " "
                    << a.province << a.city << a.district << a.detail;
                if (a.isDefault) cout << " ★默认";
                cout << "\\n";
            }
        }
        pauseScreen();
        break;
    }
    case 3: {
        int aid;
        cout << "请输入地址ID: ";
        cin >> aid;
        for (auto& a : addresses) {
            if (a.userId == currentUserId) a.isDefault = false;
            if (a.id == aid && a.userId == currentUserId) a.isDefault = true;
        }
        saveAddresses();
        cout << "设置成功！\\n";
        pauseScreen();
        break;
    }
    case 0: return;
    default: cout << "无效选项\\n"; pauseScreen();
    }
}