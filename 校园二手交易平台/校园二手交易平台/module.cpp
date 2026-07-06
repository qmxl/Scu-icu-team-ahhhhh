#include "module_user.h"

vector<User> users;
int currentUserId = -1;

// ===== 构造函数 =====
User::User(int i, string un, string pw, string nn, string ph, string em, string r)
    : id(i), username(un), password(pw), nickname(nn), phone(ph), email(em), role(r), isActive(true) {
}

// ===== 加载用户数据 =====
void loadUsers() {
    ifstream file("data/users.txt");
    if (!file) return;
    int id; string un, pw, nn, ph, em, role; bool active;
    while (file >> id >> un >> pw >> nn >> ph >> em >> role >> active) {
        User u(id, un, pw, nn, ph, em, role);
        u.isActive = active;
        users.push_back(u);
    }
    file.close();
}

// ===== 保存用户数据 =====
void saveUsers() {
    ofstream file("data/users.txt");
    for (auto& u : users) {
        file << u.id << " " << u.username << " " << u.password << " "
            << u.nickname << " " << u.phone << " " << u.email << " "
            << u.role << " " << u.isActive << "\\n";
    }
    file.close();
}

// ===== 注册 =====
bool registerUser() {
    string un, pw, nn, ph, em;
    clearScreen();
    cout << "\\n========== 用户注册 ==========\\n";
    cout << "用户名: "; cin >> un;

    for (auto& u : users) {
        if (u.username == un) {
            cout << "用户名已存在！\\n";
            pauseScreen();
            return false;
        }
    }

    cout << "密码: "; cin >> pw;
    cout << "昵称: "; cin >> nn;
    cout << "手机号: "; cin >> ph;
    cout << "邮箱: "; cin >> em;

    int newId = users.empty() ? 1 : users.back().id + 1;
    users.push_back(User(newId, un, pw, nn, ph, em, "student"));
    saveUsers();
    cout << "注册成功！\\n";
    pauseScreen();
    return true;
}

// ===== 登录 =====
bool loginUser() {
    string un, pw;
    clearScreen();
    cout << "\\n========== 用户登录 ==========\\n";
    cout << "用户名: "; cin >> un;
    cout << "密码: "; cin >> pw;

    for (auto& u : users) {
        if (u.username == un && u.password == pw && u.isActive) {
            currentUserId = u.id;
            cout << "欢迎回来，" << u.nickname << "！\\n";
            pauseScreen();
            return true;
        }
    }
    cout << "用户名或密码错误，或账号已冻结！\\n";
    pauseScreen();
    return false;
}

// ===== 退出登录 =====
void logoutUser() {
    currentUserId = -1;
    cout << "已退出登录\\n";
}

// ===== 修改个人信息 =====
void modifyProfile() {
    if (currentUserId == -1) { cout << "请先登录！\\n"; return; }

    User* cur = nullptr;
    for (auto& u : users) {
        if (u.id == currentUserId) { cur = &u; break; }
    }
    if (!cur) return;

    clearScreen();
    cout << "\\n========== 修改个人信息 ==========\\n";
    cout << "1. 修改昵称 (当前: " << cur->nickname << ")\\n";
    cout << "2. 修改手机号 (当前: " << cur->phone << ")\\n";
    cout << "3. 修改邮箱 (当前: " << cur->email << ")\\n";
    cout << "4. 修改密码\\n";
    cout << "0. 返回\\n";
    cout << "请选择: ";

    int choice; cin >> choice;
    switch (choice) {
    case 1:
        cout << "新昵称: ";
        cin >> cur->nickname;
        break;
    case 2:
        cout << "新手机号: ";
        cin >> cur->phone;
        break;
    case 3:
        cout << "新邮箱: ";
        cin >> cur->email;
        break;
    case 4: {
        string old, new1, new2;
        cout << "原密码: "; cin >> old;
        if (old != cur->password) {
            cout << "密码错误！\\n";
            pauseScreen();
            return;
        }
        cout << "新密码: "; cin >> new1;
        cout << "确认密码: "; cin >> new2;
        if (new1 != new2) {
            cout << "两次密码不一致！\\n";
            pauseScreen();
            return;
        }
        cur->password = new1;
        break;
    }
    case 0: return;
    default: cout << "无效选项\\n";
    }
    saveUsers();
    cout << "修改成功！\\n";
    pauseScreen();
}

// ===== 密码找回 =====
bool findPassword() {
    string un, email, phone;
    clearScreen();
    cout << "\\n========== 密码找回 ==========\\n";
    cout << "用户名: "; cin >> un;
    cout << "注册邮箱: "; cin >> email;
    cout << "注册手机: "; cin >> phone;

    for (auto& u : users) {
        if (u.username == un && u.email == email && u.phone == phone) {
            cout << "您的密码是: " << u.password << "\\n";
            cout << "请牢记并尽快修改！\\n";
            pauseScreen();
            return true;
        }
    }
    cout << "信息验证失败！\\n";
    pauseScreen();
    return false;
}

// ===== 获取当前用户 =====
User* getCurrentUser() {
    for (auto& u : users) {
        if (u.id == currentUserId) return &u;
    }
    return nullptr;
}

// ===== 检查是否是管理员 =====
bool isAdmin() {
    User* u = getCurrentUser();
    return u && u->role == "admin";
}