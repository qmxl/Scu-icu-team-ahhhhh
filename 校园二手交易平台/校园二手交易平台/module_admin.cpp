#ifndef MODULE_ADMIN_H
#define MODULE_ADMIN_H

#include "common.h"
#include "module_user.h"
#include "module_goods.h"
#include "module_comment.h"

// ===== º¯ÊýÉùÃ÷ =====
bool adminLogin();
void adminMenu();
void auditGoods();
void manageCategories();
void viewAllUsers();
void viewAllFeedbacks();
void handleReports();

#endif