#pragma once
#ifndef MODULE_COMMENT_H
#define MODULE_COMMENT_H

#include "common.h"
#include "module_user.h"
#include "module_goods.h"

// ===== 留言类 =====
class Comment {
public:
    int id, goodsId, userId;
    string content, reply;
    time_t createTime, replyTime;
    bool isReported;

    Comment() : id(0), goodsId(0), userId(0), isReported(false) {}
};

// ===== 反馈类 =====
class Feedback {
public:
    int id, userId;
    string content, reply;
    time_t createTime;

    Feedback() : id(0), userId(0) {}
};

// ===== 函数声明 =====
void loadComments();
void saveComments();
void loadFeedbacks();
void saveFeedbacks();
void postComment();
void replyComment();
void viewComments();
void submitFeedback();
void reportComment();

#endif
