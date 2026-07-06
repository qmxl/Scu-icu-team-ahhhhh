#include "module_ui.h"      // ← 包含这个就能用 mainMenu()
#include "module_user.h"
#include "module_goods.h"
#include "module_order.h"
#include "module_comment.h"
#include <sys/stat.h>

// ===== 创建目录（跨平台） =====
void createDataDir() {
#ifdef _WIN32
    system("mkdir data 2>nul");
#else
    mkdir("data", 0777);
#endif
}

// ===== 初始化数据 =====
void initData() {
    createDataDir();
    loadUsers();
    loadGoods();
    loadOrders();
    loadComments();
    loadFeedbacks();
    loadAddresses();
    loadFavorites();
    initCategories();
}

// ===== 主函数 =====
int main() {
#ifdef _WIN32
    system("chcp 65001 > nul");  // 中文编码
#endif

    initData();
    mainMenu();  // ← 现在能找到这个函数了

    return 0;
}