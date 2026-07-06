#include "module_comment.h"

vector<Comment> comments;
vector<Feedback> feedbacks;

// ===== 加载留言 =====
void loadComments() {
    ifstream file("data/comments.txt");
    if (!file) return;
    int id, gid, uid; string c, r; time_t ct, rt; bool rep;
    while (file >> id >> gid >> uid >> c >> r >> ct >> rt >> rep) {
        Comment cm;
        cm.id = id; cm.goodsId = gid; cm.userId = uid;
        cm.content = c; cm.reply = r;
        cm.createTime = ct; cm.replyTime = rt;
        cm.isReported = rep;
        comments.push_back(cm);
    }
    file.close();
}

// ===== 保存留言 =====
void saveComments() {
    ofstream file("data/comments.txt");
    for (auto& cm : comments) {
        file << cm.id << " " << cm.goodsId << " " << cm.userId << " "
            << cm.content << " " << cm.reply << " "
            << cm.createTime << " " << cm.replyTime << " "
            << cm.isReported << "\\n";
    }
    file.close();
}

// ===== 加载反馈 =====
void loadFeedbacks() {
    ifstream file("data/feedbacks.txt");
    if (!file) return;
    int id, uid; string c, r; time_t ct;
    while (file >> id >> uid >> c >> r >> ct) {
        Feedback f;
        f.id = id; f.userId = uid; f.content = c; f.reply = r; f.createTime = ct;
        feedbacks.push_back(f);
    }
    file.close();
}

// ===== 保存反馈 =====
void saveFeedbacks() {
    ofstream file("data/feedbacks.txt");
    for (auto& f : feedbacks) {
        file << f.id << " " << f.userId << " " << f.content << " "
            << f.reply << " " << f.createTime << "\\n";
    }
    file.close();
}

// ===== 发表留言 =====
void postComment() {
    if (currentUserId == -1) {
        cout << "请先登录！\\n";
        pauseScreen();
        return;
    }

    clearScreen();
    cout << "\\n========== 发表留言 ==========\\n";
    int gid;
    cout << "请输入商品ID: ";
    cin >> gid;

    bool exists = false;
    for (auto& g : goods) {
        if (g.id == gid) { exists = true; break; }
    }
    if (!exists) {
        cout << "商品不存在！\\n";
        pauseScreen();
        return;
    }

    Comment cm;
    cm.id = comments.empty() ? 1 : comments.back().id + 1;
    cm.goodsId = gid;
    cm.userId = currentUserId;
    // ===== 发表留言 - 续 =====
    cout << "请输入留言内容: ";
    cin.ignore();
    getline(cin, cm.content);
    time(&cm.createTime);
    cm.replyTime = 0;
    cm.isReported = false;

    comments.push_back(cm);
    saveComments();
    cout << "留言成功！\\n";
    pauseScreen();
}

// ===== 回复留言 =====
void replyComment() {
    if (currentUserId == -1) {
        cout << "请先登录！\\n";
        pauseScreen();
        return;
    }

    clearScreen();
    cout << "\\n========== 回复留言 ==========\\n";
    int cmid;
    cout << "请输入要回复的留言ID: ";
    cin >> cmid;

    for (auto& cm : comments) {
        if (cm.id == cmid) {
            // 检查是否有权限回复（商品的发布者）
            bool isOwner = false;
            for (auto& g : goods) {
                if (g.id == cm.goodsId && g.userId == currentUserId) {
                    isOwner = true;
                    break;
                }
            }
            if (!isOwner) {
                cout << "只有商品发布者才能回复！\\n";
                pauseScreen();
                return;
            }

            cout << "原留言: " << cm.content << "\\n";
            cout << "请输入回复内容: ";
            cin.ignore();
            getline(cin, cm.reply);
            time(&cm.replyTime);
            saveComments();
            cout << "回复成功！\\n";
            pauseScreen();
            return;
        }
    }
    cout << "留言不存在！\\n";
    pauseScreen();
}

// ===== 查看商品留言 =====
void viewComments() {
    clearScreen();
    cout << "\\n========== 商品留言 ==========\\n";
    int gid;
    cout << "请输入商品ID: ";
    cin >> gid;

    bool hasComment = false;
    cout << "\\n--- 留言记录 ---\\n";
    for (auto& cm : comments) {
        if (cm.goodsId == gid && !cm.isReported) {
            hasComment = true;
            // 查找留言用户
            string userName = "未知用户";
            for (auto& u : users) {
                if (u.id == cm.userId) { userName = u.nickname; break; }
            }
            cout << "[" << userName << "] " << cm.content << "\\n";
            if (!cm.reply.empty()) {
                cout << "  └─ 卖家回复: " << cm.reply << "\\n";
            }
            cout << "  (留言ID: " << cm.id << ")\\n\\n";
        }
    }
    if (!hasComment) {
        cout << "暂无留言\\n";
    }
    pauseScreen();
}

// ===== 意见反馈 =====
void submitFeedback() {
    if (currentUserId == -1) {
        cout << "请先登录！\\n";
        pauseScreen();
        return;
    }

    clearScreen();
    cout << "\\n========== 意见反馈 ==========\\n";
    Feedback f;
    f.id = feedbacks.empty() ? 1 : feedbacks.back().id + 1;
    f.userId = currentUserId;
    cout << "请输入您的意见或建议: ";
    cin.ignore();
    getline(cin, f.content);
    time(&f.createTime);

    feedbacks.push_back(f);
    saveFeedbacks();
    cout << "感谢您的反馈！\\n";
    pauseScreen();
}

// ===== 举报留言 =====
void reportComment() {
    if (currentUserId == -1) {
        cout << "请先登录！\\n";
        pauseScreen();
        return;
    }

    clearScreen();
    cout << "\\n========== 举报留言 ==========\\n";
    int cmid;
    cout << "请输入要举报的留言ID: ";
    cin >> cmid;

    for (auto& cm : comments) {
        if (cm.id == cmid) {
            if (cm.isReported) {
                cout << "该留言已被举报！\\n";
                pauseScreen();
                return;
            }
            cm.isReported = true;
            saveComments();
            cout << "举报成功，管理员将尽快处理！\\n";
            pauseScreen();
            return;
        }
    }
    cout << "留言不存在！\\n";
    pauseScreen();
}