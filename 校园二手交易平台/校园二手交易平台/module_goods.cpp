#include "module_goods.h"

vector<Goods> goods;
vector<Category> categories;
vector<int> favorites;

// ===== 构造函数 =====
Goods::Goods() : id(0), userId(0), categoryId(0), viewCount(0), collectCount(0) {}

Goods::Goods(int i, int uid, int cid, string n, string d, string po, string pn, string s, string img)
    : id(i), userId(uid), categoryId(cid), name(n), description(d),
    price_old(po), price_new(pn), status(s), imagePath(img),
    viewCount(0), collectCount(0) {
    time(&createTime);
}

// ===== 初始化分类 =====
void initCategories() {
    if (categories.empty()) {
        categories.push_back(Category(1, "教材教辅"));
        categories.push_back(Category(2, "数码电子"));
        categories.push_back(Category(3, "生活用品"));
        categories.push_back(Category(4, "服饰鞋包"));
        categories.push_back(Category(5, "运动户外"));
        categories.push_back(Category(6, "其他"));
    }
}

// ===== 加载商品 =====
void loadGoods() {
    ifstream file("data/goods.txt");
    if (!file) return;
    int id, uid, cid, view, collect; time_t t;
    string n, d, po, pn, s, img;
    while (file >> id >> uid >> cid >> n >> d >> po >> pn >> s >> img >> view >> collect >> t) {
        Goods g(id, uid, cid, n, d, po, pn, s, img);
        g.viewCount = view;
        g.collectCount = collect;
        g.createTime = t;
        goods.push_back(g);
    }
    file.close();
}

// ===== 保存商品 =====
void saveGoods() {
    ofstream file("data/goods.txt");
    for (auto& g : goods) {
        file << g.id << " " << g.userId << " " << g.categoryId << " "
            << g.name << " " << g.description << " " << g.price_old << " "
            << g.price_new << " " << g.status << " " << g.imagePath << " "
            << g.viewCount << " " << g.collectCount << " " << g.createTime << "\\n";
    }
    file.close();
}

// ===== 加载收藏 =====
void loadFavorites() {
    ifstream file("data/favorites.txt");
    if (!file) return;
    int uid, gid;
    while (file >> uid >> gid) {
        if (uid == currentUserId) favorites.push_back(gid);
    }
    file.close();
}

// ===== 保存收藏 =====
void saveFavorites() {
    ofstream file("data/favorites.txt");
    for (auto& gid : favorites) {
        file << currentUserId << " " << gid << "\\n";
    }
    file.close();
}

// ===== 发布商品 =====
void publishGoods() {
    if (currentUserId == -1) {
        cout << "请先登录！\\n";
        pauseScreen();
        return;
    }

    clearScreen();
    cout << "\\n========== 发布闲置商品 ==========\\n";
    Goods g;
    g.id = goods.empty() ? 1 : goods.back().id + 1;
    g.userId = currentUserId;
    g.status = "pending";

    cout << "商品名称: ";
    cin.ignore();
    getline(cin, g.name);
    cout << "商品描述: ";
    getline(cin, g.description);
    cout << "原价: ";
    cin >> g.price_old;
    cout << "转让价: ";
    cin >> g.price_new;

    cout << "选择分类:\\n";
    for (auto& c : categories) {
        cout << c.id << ". " << c.name << "\\n";
    }
    cout << "请输入分类编号: ";
    cin >> g.categoryId;

    cout << "图片路径(可选): ";
    cin >> g.imagePath;

    time(&g.createTime);
    goods.push_back(g);
    saveGoods();
    cout << "发布成功！等待管理员审核...\\n";
    pauseScreen();
}

// ===== 浏览商品 =====
void browseGoods() {
    clearScreen();
    cout << "\\n========== 商品浏览 ==========\\n";
    cout << "1. 全部商品\\n";
    cout << "2. 按分类筛选\\n";
    cout << "3. 关键词搜索\\n";
    cout << "4. 查看收藏\\n";
    cout << "0. 返回\\n";
    cout << "请选择: ";

    int choice; cin >> choice;
    vector<Goods> result;

    switch (choice) {
    case 1: {
        for (auto& g : goods) {
            if (g.status == "on_sale") result.push_back(g);
        }
        break;
    }
    case 2: {
        cout << "选择分类:\\n";
        for (auto& c : categories) {
            cout << c.id << ". " << c.name << "\\n";
        }
        int cid;
        cout << "请输入: ";
        cin >> cid;
        for (auto& g : goods) {
            if (g.categoryId == cid && g.status == "on_sale") {
                result.push_back(g);
            }
        }
        break;
    }
    case 3: {
        string keyword;
        cout << "请输入关键词: ";
        cin >> keyword;
        for (auto& g : goods) {
            if (g.status == "on_sale" &&
                (g.name.find(keyword) != string::npos ||
                    g.description.find(keyword) != string::npos)) {
                result.push_back(g);
            }
        }
        break;
    }
    case 4: {
        for (auto& gid : favorites) {
            for (auto& g : goods) {
                if (g.id == gid && g.status == "on_sale") {
                    result.push_back(g);
                }
            }
        }
        break;
    }
    case 0: return;
    default: cout << "无效选项\\n"; pauseScreen(); return;
    }

    if (result.empty()) {
        cout << "没有找到商品\\n";
        pauseScreen();
        return;
    }

    cout << "\\n找到 " << result.size() << " 件商品:\\n";
    cout << "========================================\\n";
    for (auto& g : result) {
        cout << "ID:" << g.id << " | " << g.name << " | ¥" << g.price_new
            << " | 浏览:" << g.viewCount << " | 收藏:" << g.collectCount << "\\n";
    }

    cout << "\\n输入商品ID查看详情，输入0返回: ";
    int gid; cin >> gid;
    if (gid > 0) viewGoodsDetail(gid);
}

// ===== 查看商品详情 =====
void viewGoodsDetail(int goodsId) {
    for (auto& g : goods) {
        if (g.id == goodsId) {
            g.viewCount++;
            saveGoods();

            clearScreen();
            cout << "\\n========== 商品详情 ==========\\n";
            cout << "名称: " << g.name << "\\n";
            cout << "原价: ¥" << g.price_old << "\\n";
            cout << "转让价: ¥" << g.price_new << "\\n";
            cout << "描述: " << g.description << "\\n";
            cout << "状态: " << g.status << "\\n";
            cout << "浏览量: " << g.viewCount << "\\n";
            cout << "收藏量: " << g.collectCount << "\\n";

            // 查找分类名
            for (auto& c : categories) {
                if (c.id == g.categoryId) {
                    cout << "分类: " << c.name << "\\n";
                    break;
                }
            }

            // 查找发布者
            for (auto& u : users) {
                if (u.id == g.userId) {
                    cout << "发布者: " << u.nickname << " (" << u.phone << ")\\n";
                    break;
                }
            }

            cout << "\\n1. 收藏/取消收藏\\n";
            cout << "2. 立即购买\\n";
            cout << "3. 发表留言\\n";
            cout << "0. 返回\\n";
            cout << "请选择: ";

            int choice; cin >> choice;
            switch (choice) {
            case 1: {
                auto it = find(favorites.begin(), favorites.end(), g.id);
                if (it != favorites.end()) {
                    favorites.erase(it);
                    cout << "已取消收藏\\n";
                }
                else {
                    favorites.push_back(g.id);
                    g.collectCount++;
                    cout << "收藏成功！\\n";
                }
                saveFavorites();
                saveGoods();
                pauseScreen();
                break;
            }
            case 2:
                cout << "请到订单模块完成购买\\n";
                pauseScreen();
                break;
            case 3:
                // 跳转到留言模块
                cout << "请到留言模块发表\\n";
                pauseScreen();
                break;
            case 0: return;
            }
            return;
        }
    }
    cout << "商品不存在！\\n";
    pauseScreen();
}

// ===== 我的发布 =====
void myPublishedGoods() {
    if (currentUserId == -1) {
        cout << "请先登录！\\n";
        pauseScreen();
        return;
    }

    clearScreen();
    cout << "\\n========== 我的发布 ==========\\n";
    bool hasGoods = false;
    for (auto& g : goods) {
        if (g.userId == currentUserId) {
            hasGoods = true;
            cout << "ID:" << g.id << " | " << g.name << " | ¥" << g.price_new
                << " | 状态:" << g.status << "\\n";
        }
    }

    if (!hasGoods) {
        cout << "暂无发布\\n";
        pauseScreen();
        return;
    }

    cout << "\\n1. 编辑商品\\n";
    cout << "2. 下架商品\\n";
    cout << "0. 返回\\n";
    cout << "请选择: ";

    int choice; cin >> choice;
    if (choice == 1 || choice == 2) {
        int gid;
        cout << "请输入商品ID: ";
        cin >> gid;
        for (auto& g : goods) {
            if (g.id == gid && g.userId == currentUserId) {
                if (choice == 1) {
                    cout << "新名称(留空不变): ";
                    string tmp;
                    cin.ignore();
                    getline(cin, tmp);
                    if (!tmp.empty()) g.name = tmp;
                    cout << "新价格: ";
                    cin >> g.price_new;
                    saveGoods();
                    cout << "修改成功！\\n";
                }
                else {
                    g.status = "off_sale";
                    saveGoods();
                    cout << "已下架！\\n";
                }
                pauseScreen();
                return;
            }
        }
        cout << "商品不存在或无权限！\\n";
        pauseScreen();
    }
}