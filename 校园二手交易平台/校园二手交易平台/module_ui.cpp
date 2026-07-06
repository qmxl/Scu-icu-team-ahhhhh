#include "module_ui.h"

// ===== 清屏 =====
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// ===== 暂停 =====
void pauseScreen() {
    cout << "\\n按任意键继续...";
#ifdef _WIN32
    getch();
#else
    getchar();
#endif
    cout << "\\n";
}

// ===== 获取当前时间 =====
string getCurrentTime() {
    time_t now = time(nullptr);
    char buf[64];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now));
    return string(buf);
}

// ===== 收藏菜单 =====
void favoriteMenu() {
    clearScreen();
    cout << "\\n========== 收藏管理 ==========\\n";
    if (favorites.empty()) {
        cout << "暂无收藏\\n";
        pauseScreen();
        return;
    }

    cout << "我的收藏:\\n";
    for (auto& gid : favorites) {
        for (auto& g : goods) {
            if (g.id == gid) {
                cout << "ID:" << g.id << " | " << g.name << " | ¥" << g.price_new << "\\n";
                break;
            }
        }
    }
    pauseScreen();
}

// ===== 主菜单（定义在这里） =====
void mainMenu() {
    int choice;
    while (true) {
        clearScreen();
        cout << "\\n========== 校园二手交易平台 ==========\\n";
        cout << "1. 用户登录\\n";
        cout << "2. 用户注册\\n";
        cout << "3. 管理员登录\\n";
        cout << "4. 密码找回\\n";
        cout << "0. 退出程序\\n";
        cout << "请选择: ";
        cin >> choice;

        switch (choice) {
        case 1:
            if (loginUser()) {
                if (isAdmin()) {
                    adminMenu();
                }
                else {
                    studentMenu();
                }
            }
            break;
        case 2:
            registerUser();
            break;
        case 3:
            if (adminLogin()) {
                adminMenu();
            }
            break;
        case 4:
            findPassword();
            break;
        case 0:
            cout << "感谢使用，再见！\\n";
            exit(0);
        default:
            cout << "输入有误，请重新选择\\n";
            pauseScreen();
        }
    }
}

// ===== 学生主菜单 =====
void studentMenu() {
    int choice;
    while (true) {
        clearScreen();
        User* cur = getCurrentUser();
        cout << "\\n========== 学生主菜单 ==========\\n";
        cout << "欢迎，" << (cur ? cur->nickname : "用户") << "！\\n\\n";
        cout << "1. 浏览商品\\n";
        cout << "2. 发布闲置\\n";
        cout << "3. 我的订单\\n";
        cout << "4. 个人中心\\n";
        cout << "5. 留言互动\\n";
        cout << "6. 地址管理\\n";
        cout << "7. 意见反馈\\n";
        cout << "0. 退出登录\\n";
        cout << "请选择: ";
        cin >> choice;

        switch (choice) {
        case 1: browseGoods(); break;
        case 2: publishGoods(); break;
        case 3: myOrders(); break;
        case 4: profileMenu(); break;
        case 5: commentMenu(); break;
        case 6: manageAddress(); break;
        case 7: submitFeedback(); break;
        case 0:
            logoutUser();
            return;
        default:
            cout << "无效选项\\n";
            pauseScreen();
        }
    }
}

// ===== 个人中心菜单 =====
void profileMenu() {
    int choice;
    while (true) {
        clearScreen();
        cout << "\\n========== 个人中心 ==========\\n";
        cout << "1. 修改个人信息\\n";
        cout << "2. 我的发布\\n";
        cout << "3. 收藏管理\\n";
        cout << "4. 密码找回\\n";
        cout << "0. 返回\\n";
        cout << "请选择: ";
        cin >> choice;

        switch (choice) {
        case 1: modifyProfile(); break;
        case 2: myPublishedGoods(); break;
        case 3: favoriteMenu(); break;
        case 4: findPassword(); break;
        case 0: return;
        default:
            cout << "无效选项\\n";
            pauseScreen();
        }
    }
}

// ===== 留言互动菜单 =====
void commentMenu() {
    int choice;
    while (true) {
        clearScreen();
        cout << "\\n========== 留言互动 ==========\\n";
        cout << "1. 查看商品留言\\n";
        cout << "2. 发表留言\\n";
        cout << "3. 回复留言\\n";
        cout << "4. 举报留言\\n";
        cout << "0. 返回\\n";
        cout << "请选择: ";
        cin >> choice;

        switch (choice) {
        case 1: viewComments(); break;
        case 2: postComment(); break;
        case 3: replyComment(); break;
        case 4: reportComment(); break;
        case 0: return;
        default:
            cout << "无效选项\\n";
            pauseScreen();
        }
    }
}