#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <cppconn/driver.h>
#include <cppconn/connection.h>
#include <cppconn/statement.h>
#include <cppconn/resultset.h>

using namespace std;
using namespace sql;

class ConsoleApp {
private:
    Driver* driver;
    Connection* conn;

public:
    ConsoleApp() : driver(nullptr), conn(nullptr) {}

    ~ConsoleApp() {
        if (conn) delete conn;
    }

    bool connect() {
        try {
            driver = get_driver_instance();
            conn = driver->connect("tcp://127.0.0.1:3306", "root", "password");
            conn->setSchema("campus_trade");
            cout << "✓ 数据库连接成功！" << endl;
            return true;
        } catch (SQLException& e) {
            cout << "✗ 数据库连接失败: " << e.what() << endl;
            return false;
        }
    }

    void showMainMenu() {
        cout << "\n=========================================" << endl;
        cout << "        校园二手交易系统 - 管理员控制台" << endl;
        cout << "=========================================" << endl;
        cout << "1. 用户管理" << endl;
        cout << "2. 商品管理" << endl;
        cout << "3. 订单管理" << endl;
        cout << "4. 分类管理" << endl;
        cout << "5. 统计报表" << endl;
        cout << "6. 退出系统" << endl;
        cout << "=========================================" << endl;
        cout << "请选择操作 (1-6): ";
    }

    void userManagement() {
        int choice;
        do {
            cout << "\n---------- 用户管理 ----------" << endl;
            cout << "1. 查看所有用户" << endl;
            cout << "2. 添加用户" << endl;
            cout << "3. 修改用户信息" << endl;
            cout << "4. 删除用户" << endl;
            cout << "5. 返回主菜单" << endl;
            cout << "请选择操作 (1-5): ";
            cin >> choice;

            switch (choice) {
                case 1: viewUsers(); break;
                case 2: addUser(); break;
                case 3: updateUser(); break;
                case 4: deleteUser(); break;
                case 5: return;
                default: cout << "无效选项，请重新选择！" << endl;
            }
        } while (choice != 5);
    }

    void viewUsers() {
        try {
            Statement* stmt = conn->createStatement();
            ResultSet* rs = stmt->executeQuery("SELECT id, student_id, name, phone, dormitory, created_at FROM users ORDER BY created_at DESC");
            
            cout << "\n" << left << setw(8) << "ID" 
                 << setw(15) << "学号" 
                 << setw(12) << "姓名" 
                 << setw(15) << "手机号" 
                 << setw(20) << "宿舍地址" 
                 << setw(20) << "注册时间" << endl;
            cout << "-----------------------------------------------------------------------------------" << endl;

            while (rs->next()) {
                cout << left << setw(8) << rs->getInt("id")
                     << setw(15) << rs->getString("student_id")
                     << setw(12) << rs->getString("name")
                     << setw(15) << rs->getString("phone")
                     << setw(20) << rs->getString("dormitory")
                     << setw(20) << rs->getString("created_at") << endl;
            }

            delete rs;
            delete stmt;
        } catch (SQLException& e) {
            cout << "查询失败: " << e.what() << endl;
        }
    }

    void addUser() {
        try {
            string studentId, password, name, phone, dormitory;
            
            cout << "请输入学号: ";
            cin >> studentId;
            cout << "请输入密码: ";
            cin >> password;
            cout << "请输入姓名: ";
            cin >> name;
            cout << "请输入手机号 (可选): ";
            cin >> phone;
            cout << "请输入宿舍地址 (可选): ";
            cin >> dormitory;

            PreparedStatement* pstmt = conn->prepareStatement(
                "INSERT INTO users (student_id, password, name, phone, dormitory) VALUES (?, ?, ?, ?, ?)");
            pstmt->setString(1, studentId);
            pstmt->setString(2, password); // 实际项目中应加密
            pstmt->setString(3, name);
            pstmt->setString(4, phone);
            pstmt->setString(5, dormitory);
            pstmt->executeUpdate();

            cout << "✓ 用户添加成功！" << endl;
            delete pstmt;
        } catch (SQLException& e) {
            cout << "添加失败: " << e.what() << endl;
        }
    }

    void updateUser() {
        try {
            int id;
            string name, phone, dormitory;
            
            cout << "请输入要修改的用户ID: ";
            cin >> id;
            cout << "请输入新姓名: ";
            cin >> name;
            cout << "请输入新手机号: ";
            cin >> phone;
            cout << "请输入新宿舍地址: ";
            cin >> dormitory;

            PreparedStatement* pstmt = conn->prepareStatement(
                "UPDATE users SET name=?, phone=?, dormitory=? WHERE id=?");
            pstmt->setString(1, name);
            pstmt->setString(2, phone);
            pstmt->setString(3, dormitory);
            pstmt->setInt(4, id);
            int rows = pstmt->executeUpdate();

            if (rows > 0) {
                cout << "✓ 用户信息修改成功！" << endl;
            } else {
                cout << "✗ 未找到该用户！" << endl;
            }

            delete pstmt;
        } catch (SQLException& e) {
            cout << "修改失败: " << e.what() << endl;
        }
    }

    void deleteUser() {
        try {
            int id;
            char confirm;
            
            cout << "请输入要删除的用户ID: ";
            cin >> id;
            cout << "确认删除该用户? (y/n): ";
            cin >> confirm;

            if (confirm == 'y' || confirm == 'Y') {
                PreparedStatement* pstmt = conn->prepareStatement("DELETE FROM users WHERE id=?");
                pstmt->setInt(1, id);
                int rows = pstmt->executeUpdate();

                if (rows > 0) {
                    cout << "✓ 用户删除成功！" << endl;
                } else {
                    cout << "✗ 未找到该用户！" << endl;
                }
                delete pstmt;
            } else {
                cout << "操作已取消" << endl;
            }
        } catch (SQLException& e) {
            cout << "删除失败: " << e.what() << endl;
        }
    }

    void productManagement() {
        int choice;
        do {
            cout << "\n---------- 商品管理 ----------" << endl;
            cout << "1. 查看所有商品" << endl;
            cout << "2. 上架商品" << endl;
            cout << "3. 修改商品信息" << endl;
            cout << "4. 下架商品" << endl;
            cout << "5. 搜索商品" << endl;
            cout << "6. 返回主菜单" << endl;
            cout << "请选择操作 (1-6): ";
            cin >> choice;

            switch (choice) {
                case 1: viewProducts(); break;
                case 2: addProduct(); break;
                case 3: updateProduct(); break;
                case 4: deleteProduct(); break;
                case 5: searchProduct(); break;
                case 6: return;
                default: cout << "无效选项，请重新选择！" << endl;
            }
        } while (choice != 6);
    }

    void viewProducts() {
        try {
            Statement* stmt = conn->createStatement();
            ResultSet* rs = stmt->executeQuery(
                "SELECT p.id, p.name, p.price, p.status, c.name as category_name, u.name as seller_name, p.created_at "
                "FROM products p "
                "JOIN categories c ON p.category_id = c.id "
                "JOIN users u ON p.seller_id = u.id "
                "ORDER BY p.created_at DESC");
            
            cout << "\n" << left << setw(8) << "ID" 
                 << setw(25) << "商品名称" 
                 << setw(10) << "价格" 
                 << setw(10) << "状态" 
                 << setw(15) << "分类" 
                 << setw(12) << "卖家" 
                 << setw(20) << "发布时间" << endl;
            cout << "---------------------------------------------------------------------------------------------" << endl;

            while (rs->next()) {
                string status = rs->getInt("status") == 1 ? "在售" : "已售出";
                cout << left << setw(8) << rs->getInt("id")
                     << setw(25) << rs->getString("name")
                     << setw(10) << rs->getDouble("price")
                     << setw(10) << status
                     << setw(15) << rs->getString("category_name")
                     << setw(12) << rs->getString("seller_name")
                     << setw(20) << rs->getString("created_at") << endl;
            }

            delete rs;
            delete stmt;
        } catch (SQLException& e) {
            cout << "查询失败: " << e.what() << endl;
        }
    }

    void addProduct() {
        try {
            string name, description, imageUrl, condition;
            double price;
            int categoryId, sellerId;

            cout << "请输入商品名称: ";
            cin.ignore();
            getline(cin, name);
            cout << "请输入商品描述: ";
            getline(cin, description);
            cout << "请输入分类ID: ";
            cin >> categoryId;
            cout << "请输入卖家ID: ";
            cin >> sellerId;
            cout << "请输入价格: ";
            cin >> price;
            cout << "请输入图片链接: ";
            cin >> imageUrl;
            cout << "请输入成色: ";
            cin >> condition;

            PreparedStatement* pstmt = conn->prepareStatement(
                "INSERT INTO products (name, description, category_id, seller_id, price, image_url, condition) "
                "VALUES (?, ?, ?, ?, ?, ?, ?)");
            pstmt->setString(1, name);
            pstmt->setString(2, description);
            pstmt->setInt(3, categoryId);
            pstmt->setInt(4, sellerId);
            pstmt->setDouble(5, price);
            pstmt->setString(6, imageUrl);
            pstmt->setString(7, condition);
            pstmt->executeUpdate();

            cout << "✓ 商品上架成功！" << endl;
            delete pstmt;
        } catch (SQLException& e) {
            cout << "上架失败: " << e.what() << endl;
        }
    }

    void updateProduct() {
        try {
            int id, categoryId;
            string name, description, imageUrl, condition;
            double price;

            cout << "请输入要修改的商品ID: ";
            cin >> id;
            cout << "请输入新名称: ";
            cin.ignore();
            getline(cin, name);
            cout << "请输入新描述: ";
            getline(cin, description);
            cout << "请输入新分类ID: ";
            cin >> categoryId;
            cout << "请输入新价格: ";
            cin >> price;
            cout << "请输入新图片链接: ";
            cin >> imageUrl;
            cout << "请输入新成色: ";
            cin >> condition;

            PreparedStatement* pstmt = conn->prepareStatement(
                "UPDATE products SET name=?, description=?, category_id=?, price=?, image_url=?, condition=? WHERE id=?");
            pstmt->setString(1, name);
            pstmt->setString(2, description);
            pstmt->setInt(3, categoryId);
            pstmt->setDouble(4, price);
            pstmt->setString(5, imageUrl);
            pstmt->setString(6, condition);
            pstmt->setInt(7, id);
            int rows = pstmt->executeUpdate();

            if (rows > 0) {
                cout << "✓ 商品信息修改成功！" << endl;
            } else {
                cout << "✗ 未找到该商品！" << endl;
            }

            delete pstmt;
        } catch (SQLException& e) {
            cout << "修改失败: " << e.what() << endl;
        }
    }

    void deleteProduct() {
        try {
            int id;
            char confirm;

            cout << "请输入要下架的商品ID: ";
            cin >> id;
            cout << "确认下架该商品? (y/n): ";
            cin >> confirm;

            if (confirm == 'y' || confirm == 'Y') {
                PreparedStatement* pstmt = conn->prepareStatement("UPDATE products SET status=0 WHERE id=?");
                pstmt->setInt(1, id);
                int rows = pstmt->executeUpdate();

                if (rows > 0) {
                    cout << "✓ 商品已下架！" << endl;
                } else {
                    cout << "✗ 未找到该商品！" << endl;
                }
                delete pstmt;
            } else {
                cout << "操作已取消" << endl;
            }
        } catch (SQLException& e) {
            cout << "下架失败: " << e.what() << endl;
        }
    }

    void searchProduct() {
        try {
            string keyword;
            cout << "请输入搜索关键词: ";
            cin.ignore();
            getline(cin, keyword);

            PreparedStatement* pstmt = conn->prepareStatement(
                "SELECT id, name, price, description FROM products WHERE name LIKE ? OR description LIKE ?");
            pstmt->setString(1, "%" + keyword + "%");
            pstmt->setString(2, "%" + keyword + "%");
            ResultSet* rs = pstmt->executeQuery();

            cout << "\n搜索结果:" << endl;
            cout << left << setw(8) << "ID" << setw(25) << "商品名称" << setw(10) << "价格" << "描述" << endl;
            cout << "-------------------------------------------------------------" << endl;

            while (rs->next()) {
                cout << left << setw(8) << rs->getInt("id")
                     << setw(25) << rs->getString("name")
                     << setw(10) << rs->getDouble("price")
                     << rs->getString("description") << endl;
            }

            delete rs;
            delete pstmt;
        } catch (SQLException& e) {
            cout << "搜索失败: " << e.what() << endl;
        }
    }

    void orderManagement() {
        int choice;
        do {
            cout << "\n---------- 订单管理 ----------" << endl;
            cout << "1. 查看所有订单" << endl;
            cout << "2. 查看待处理订单" << endl;
            cout << "3. 修改订单状态" << endl;
            cout << "4. 查看订单详情" << endl;
            cout << "5. 返回主菜单" << endl;
            cout << "请选择操作 (1-5): ";
            cin >> choice;

            switch (choice) {
                case 1: viewOrders(); break;
                case 2: viewPendingOrders(); break;
                case 3: updateOrderStatus(); break;
                case 4: viewOrderDetail(); break;
                case 5: return;
                default: cout << "无效选项，请重新选择！" << endl;
            }
        } while (choice != 5);
    }

    void viewOrders() {
        try {
            Statement* stmt = conn->createStatement();
            ResultSet* rs = stmt->executeQuery(
                "SELECT o.id, o.order_no, p.name as product_name, o.price, o.status, o.created_at, "
                "ub.name as buyer_name, us.name as seller_name "
                "FROM orders o "
                "JOIN products p ON o.product_id = p.id "
                "JOIN users ub ON o.buyer_id = ub.id "
                "JOIN users us ON p.seller_id = us.id "
                "ORDER BY o.created_at DESC");

            cout << "\n" << left << setw(8) << "ID" 
                 << setw(20) << "订单号" 
                 << setw(20) << "商品名称" 
                 << setw(10) << "价格" 
                 << setw(12) << "状态" 
                 << setw(12) << "买家" 
                 << setw(12) << "卖家" 
                 << setw(20) << "创建时间" << endl;
            cout << "-------------------------------------------------------------------------------------------------" << endl;

            while (rs->next()) {
                string status = getOrderStatusText(rs->getString("status"));
                cout << left << setw(8) << rs->getInt("id")
                     << setw(20) << rs->getString("order_no")
                     << setw(20) << rs->getString("product_name")
                     << setw(10) << rs->getDouble("price")
                     << setw(12) << status
                     << setw(12) << rs->getString("buyer_name")
                     << setw(12) << rs->getString("seller_name")
                     << setw(20) << rs->getString("created_at") << endl;
            }

            delete rs;
            delete stmt;
        } catch (SQLException& e) {
            cout << "查询失败: " << e.what() << endl;
        }
    }

    void viewPendingOrders() {
        try {
            PreparedStatement* pstmt = conn->prepareStatement(
                "SELECT o.id, o.order_no, p.name as product_name, o.price, o.created_at, "
                "ub.name as buyer_name, us.name as seller_name "
                "FROM orders o "
                "JOIN products p ON o.product_id = p.id "
                "JOIN users ub ON o.buyer_id = ub.id "
                "JOIN users us ON p.seller_id = us.id "
                "WHERE o.status = 'pending' ORDER BY o.created_at DESC");
            ResultSet* rs = pstmt->executeQuery();

            cout << "\n待处理订单:" << endl;
            cout << left << setw(8) << "ID" << setw(20) << "订单号" << setw(20) << "商品名称" 
                 << setw(10) << "价格" << setw(12) << "买家" << setw(12) << "卖家" << "创建时间" << endl;
            cout << "-------------------------------------------------------------" << endl;

            while (rs->next()) {
                cout << left << setw(8) << rs->getInt("id")
                     << setw(20) << rs->getString("order_no")
                     << setw(20) << rs->getString("product_name")
                     << setw(10) << rs->getDouble("price")
                     << setw(12) << rs->getString("buyer_name")
                     << setw(12) << rs->getString("seller_name")
                     << rs->getString("created_at") << endl;
            }

            delete rs;
            delete pstmt;
        } catch (SQLException& e) {
            cout << "查询失败: " << e.what() << endl;
        }
    }

    void updateOrderStatus() {
        try {
            int id;
            string status;

            cout << "请输入订单ID: ";
            cin >> id;
            cout << "请输入新状态 (pending/confirmed/completed/cancelled): ";
            cin >> status;

            PreparedStatement* pstmt = conn->prepareStatement("UPDATE orders SET status=? WHERE id=?");
            pstmt->setString(1, status);
            pstmt->setInt(2, id);
            int rows = pstmt->executeUpdate();

            if (rows > 0) {
                cout << "✓ 订单状态已更新为: " << getOrderStatusText(status) << endl;
            } else {
                cout << "✗ 未找到该订单！" << endl;
            }

            delete pstmt;
        } catch (SQLException& e) {
            cout << "更新失败: " << e.what() << endl;
        }
    }

    void viewOrderDetail() {
        try {
            int id;
            cout << "请输入订单ID: ";
            cin >> id;

            PreparedStatement* pstmt = conn->prepareStatement(
                "SELECT o.*, p.name as product_name, p.description, p.image_url, "
                "ub.name as buyer_name, ub.phone as buyer_phone, ub.dormitory as buyer_dormitory, "
                "us.name as seller_name, us.phone as seller_phone, us.dormitory as seller_dormitory "
                "FROM orders o "
                "JOIN products p ON o.product_id = p.id "
                "JOIN users ub ON o.buyer_id = ub.id "
                "JOIN users us ON p.seller_id = us.id "
                "WHERE o.id = ?");
            pstmt->setInt(1, id);
            ResultSet* rs = pstmt->executeQuery();

            if (rs->next()) {
                cout << "\n========== 订单详情 ==========" << endl;
                cout << "订单ID: " << rs->getInt("id") << endl;
                cout << "订单号: " << rs->getString("order_no") << endl;
                cout << "商品名称: " << rs->getString("product_name") << endl;
                cout << "商品描述: " << rs->getString("description") << endl;
                cout << "价格: ¥" << rs->getDouble("price") << endl;
                cout << "状态: " << getOrderStatusText(rs->getString("status")) << endl;
                cout << "\n买家信息:" << endl;
                cout << "  姓名: " << rs->getString("buyer_name") << endl;
                cout << "  电话: " << rs->getString("buyer_phone") << endl;
                cout << "  宿舍: " << rs->getString("buyer_dormitory") << endl;
                cout << "\n卖家信息:" << endl;
                cout << "  姓名: " << rs->getString("seller_name") << endl;
                cout << "  电话: " << rs->getString("seller_phone") << endl;
                cout << "  宿舍: " << rs->getString("seller_dormitory") << endl;
                cout << "\n创建时间: " << rs->getString("created_at") << endl;
            } else {
                cout << "✗ 未找到该订单！" << endl;
            }

            delete rs;
            delete pstmt;
        } catch (SQLException& e) {
            cout << "查询失败: " << e.what() << endl;
        }
    }

    string getOrderStatusText(string status) {
        if (status == "pending") return "待确认";
        if (status == "confirmed") return "已确认";
        if (status == "completed") return "已完成";
        if (status == "cancelled") return "已取消";
        return status;
    }

    void categoryManagement() {
        int choice;
        do {
            cout << "\n---------- 分类管理 ----------" << endl;
            cout << "1. 查看所有分类" << endl;
            cout << "2. 添加分类" << endl;
            cout << "3. 修改分类" << endl;
            cout << "4. 删除分类" << endl;
            cout << "5. 返回主菜单" << endl;
            cout << "请选择操作 (1-5): ";
            cin >> choice;

            switch (choice) {
                case 1: viewCategories(); break;
                case 2: addCategory(); break;
                case 3: updateCategory(); break;
                case 4: deleteCategory(); break;
                case 5: return;
                default: cout << "无效选项，请重新选择！" << endl;
            }
        } while (choice != 5);
    }

    void viewCategories() {
        try {
            Statement* stmt = conn->createStatement();
            ResultSet* rs = stmt->executeQuery("SELECT * FROM categories ORDER BY parent_id, sort_order");

            cout << "\n" << left << setw(8) << "ID" 
                 << setw(20) << "分类名称" 
                 << setw(10) << "父分类ID" 
                 << setw(15) << "图标" 
                 << setw(10) << "排序" << endl;
            cout << "-------------------------------------------" << endl;

            while (rs->next()) {
                cout << left << setw(8) << rs->getInt("id")
                     << setw(20) << rs->getString("name")
                     << setw(10) << rs->getInt("parent_id")
                     << setw(15) << rs->getString("icon")
                     << setw(10) << rs->getInt("sort_order") << endl;
            }

            delete rs;
            delete stmt;
        } catch (SQLException& e) {
            cout << "查询失败: " << e.what() << endl;
        }
    }

    void addCategory() {
        try {
            string name, icon;
            int parentId, sortOrder;

            cout << "请输入分类名称: ";
            cin >> name;
            cout << "请输入父分类ID (顶级分类填0): ";
            cin >> parentId;
            cout << "请输入图标: ";
            cin >> icon;
            cout << "请输入排序号: ";
            cin >> sortOrder;

            PreparedStatement* pstmt = conn->prepareStatement(
                "INSERT INTO categories (name, parent_id, icon, sort_order) VALUES (?, ?, ?, ?)");
            pstmt->setString(1, name);
            pstmt->setInt(2, parentId);
            pstmt->setString(3, icon);
            pstmt->setInt(4, sortOrder);
            pstmt->executeUpdate();

            cout << "✓ 分类添加成功！" << endl;
            delete pstmt;
        } catch (SQLException& e) {
            cout << "添加失败: " << e.what() << endl;
        }
    }

    void updateCategory() {
        try {
            int id, parentId, sortOrder;
            string name, icon;

            cout << "请输入要修改的分类ID: ";
            cin >> id;
            cout << "请输入新名称: ";
            cin >> name;
            cout << "请输入新父分类ID: ";
            cin >> parentId;
            cout << "请输入新图标: ";
            cin >> icon;
            cout << "请输入新排序号: ";
            cin >> sortOrder;

            PreparedStatement* pstmt = conn->prepareStatement(
                "UPDATE categories SET name=?, parent_id=?, icon=?, sort_order=? WHERE id=?");
            pstmt->setString(1, name);
            pstmt->setInt(2, parentId);
            pstmt->setString(3, icon);
            pstmt->setInt(4, sortOrder);
            pstmt->setInt(5, id);
            int rows = pstmt->executeUpdate();

            if (rows > 0) {
                cout << "✓ 分类信息修改成功！" << endl;
            } else {
                cout << "✗ 未找到该分类！" << endl;
            }

            delete pstmt;
        } catch (SQLException& e) {
            cout << "修改失败: " << e.what() << endl;
        }
    }

    void deleteCategory() {
        try {
            int id;
            char confirm;

            cout << "请输入要删除的分类ID: ";
            cin >> id;
            cout << "确认删除该分类? (y/n): ";
            cin >> confirm;

            if (confirm == 'y' || confirm == 'Y') {
                PreparedStatement* pstmt = conn->prepareStatement("DELETE FROM categories WHERE id=?");
                pstmt->setInt(1, id);
                int rows = pstmt->executeUpdate();

                if (rows > 0) {
                    cout << "✓ 分类删除成功！" << endl;
                } else {
                    cout << "✗ 未找到该分类！" << endl;
                }
                delete pstmt;
            } else {
                cout << "操作已取消" << endl;
            }
        } catch (SQLException& e) {
            cout << "删除失败: " << e.what() << endl;
        }
    }

    void showStatistics() {
        try {
            cout << "\n========== 统计报表 ==========" << endl;

            // 用户统计
            Statement* stmt = conn->createStatement();
            ResultSet* rs = stmt->executeQuery("SELECT COUNT(*) as count FROM users");
            rs->next();
            cout << "总用户数: " << rs->getInt("count") << endl;

            // 商品统计
            rs = stmt->executeQuery("SELECT COUNT(*) as count FROM products WHERE status=1");
            rs->next();
            cout << "在售商品数: " << rs->getInt("count") << endl;

            rs = stmt->executeQuery("SELECT COUNT(*) as count FROM products");
            rs->next();
            cout << "总商品数: " << rs->getInt("count") << endl;

            // 订单统计
            rs = stmt->executeQuery("SELECT COUNT(*) as count FROM orders");
            rs->next();
            cout << "总订单数: " << rs->getInt("count") << endl;

            rs = stmt->executeQuery("SELECT COUNT(*) as count FROM orders WHERE status='completed'");
            rs->next();
            cout << "已完成订单: " << rs->getInt("count") << endl;

            // 交易额统计
            rs = stmt->executeQuery("SELECT SUM(price) as total FROM orders WHERE status='completed'");
            rs->next();
            cout << "成交总额: ¥" << fixed << setprecision(2) << rs->getDouble("total") << endl;

            // 分类统计
            cout << "\n各分类商品数量:" << endl;
            rs = stmt->executeQuery(
                "SELECT c.name, COUNT(p.id) as count FROM categories c "
                "LEFT JOIN products p ON c.id = p.category_id GROUP BY c.id, c.name");
            while (rs->next()) {
                cout << "  " << rs->getString("name") << ": " << rs->getInt("count") << " 件" << endl;
            }

            delete rs;
            delete stmt;
        } catch (SQLException& e) {
            cout << "统计失败: " << e.what() << endl;
        }
    }

    void run() {
        cout << "=========================================" << endl;
        cout << "    校园二手交易系统 - 管理员控制台" << endl;
        cout << "=========================================" << endl;

        if (!connect()) {
            return;
        }

        int choice;
        do {
            showMainMenu();
            cin >> choice;

            switch (choice) {
                case 1: userManagement(); break;
                case 2: productManagement(); break;
                case 3: orderManagement(); break;
                case 4: categoryManagement(); break;
                case 5: showStatistics(); break;
                case 6: cout << "感谢使用校园二手交易系统，再见！" << endl; break;
                default: cout << "无效选项，请重新选择！" << endl;
            }
        } while (choice != 6);
    }
};

int main() {
    ConsoleApp app;
    app.run();
    return 0;
}