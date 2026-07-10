# 校园二手交易系统

基于 Drogon + MySQL 的校园二手交易系统后端服务，提供完整的二手物品交易功能。

## 技术栈

- **框架**: Drogon 1.8+
- **语言**: C++17
- **数据库**: MySQL 8.0+
- **ORM**: Drogon ORM
- **前端**: HTML5 + CSS3 + JavaScript
- **控制台**: C++ 控制台应用

## 功能特性

### 用户模块
- 用户注册/登录
- 个人资料管理
- 用户信息查询与修改

### 商品模块
- 商品发布/编辑/删除
- 商品搜索与筛选
- 商品分类管理
- 商品图片上传

### 订单模块
- 创建订单
- 订单状态管理
- 订单查询

### 收藏模块
- 商品收藏
- 收藏列表管理

### 控制台管理
- 用户管理
- 商品管理
- 订单管理
- 分类管理
- 统计报表

## 项目结构

```
二手/
├── src/
│   ├── controller/          # 控制器层
│   │   ├── UserController.cpp/h
│   │   ├── ProductController.cpp/h
│   │   ├── CategoryController.cpp/h
│   │   ├── OrderController.cpp/h
│   │   └── FavoriteController.cpp/h
│   ├── dao/                  # 数据访问层
│   │   ├── UserDAO.cpp/h
│   │   ├── ProductDAO.cpp/h
│   │   ├── CategoryDAO.cpp/h
│   │   ├── OrderDAO.cpp/h
│   │   └── FavoriteDAO.cpp/h
│   ├── model/                # 数据模型
│   │   ├── User.h
│   │   ├── Product.h
│   │   ├── Category.h
│   │   ├── Order.h
│   │   └── Favorite.h
│   ├── console/              # 控制台管理程序
│   │   └── ConsoleApp.cpp
│   └── main.cpp              # 主入口
├── frontend/                 # 前端静态页面
│   ├── index.html
│   ├── css/
│   │   └── style.css
│   └── js/
│       ├── api.js
│       └── app.js
├── database/                 # 数据库脚本
│   └── campus_trade.sql
├── build/                    # 编译输出目录
├── config/                   # 配置文件
│   └── config.json
└── README.md
```

## 环境要求

### 依赖安装

**Windows 环境:**

1. 安装 MySQL 8.0+
2. 安装 Drogon 框架
3. 安装 MySQL Connector/C++

```powershell
# 安装 vcpkg (如果尚未安装)
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat

# 安装依赖
vcpkg install drogon:x64-windows
vcpkg install mysql-connector-cpp:x64-windows
```

## 数据库配置

1. 创建数据库:
```sql
CREATE DATABASE campus_trade CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;
```

2. 导入数据库脚本:
```bash
mysql -u root -p campus_trade < database/campus_trade.sql
```

3. 修改配置文件 `config/config.json` 中的数据库连接信息:
```json
{
    "database": {
        "host": "127.0.0.1",
        "port": 3306,
        "dbname": "campus_trade",
        "user": "root",
        "password": "password"
    }
}
```

## 编译运行

### 编译后端服务

```powershell
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=[vcpkg_path]/scripts/buildsystems/vcpkg.cmake
cmake --build . --config Release
```

### 运行后端服务

```powershell
cd build/Release
.\campus_trade.exe
```

服务启动后访问: http://localhost:8080

### 编译控制台程序

```powershell
cd src/console
g++ ConsoleApp.cpp -o ConsoleApp.exe -lmysqlcppconn
```

### 运行控制台程序

```powershell
.\ConsoleApp.exe
```

## API 接口

### 用户接口
| 方法 | 路径 | 描述 |
|------|------|------|
| POST | /api/user/register | 用户注册 |
| POST | /api/user/login | 用户登录 |
| GET | /api/user/profile | 获取用户资料 |
| PUT | /api/user/profile | 更新用户资料 |
| DELETE | /api/user | 删除用户 |

### 商品接口
| 方法 | 路径 | 描述 |
|------|------|------|
| POST | /api/products | 创建商品 |
| GET | /api/products | 获取商品列表 |
| GET | /api/products/:id | 获取商品详情 |
| PUT | /api/products/:id | 更新商品 |
| DELETE | /api/products/:id | 删除商品 |
| GET | /api/products/search | 搜索商品 |

### 订单接口
| 方法 | 路径 | 描述 |
|------|------|------|
| POST | /api/orders | 创建订单 |
| GET | /api/orders | 获取订单列表 |
| GET | /api/orders/:id | 获取订单详情 |
| PUT | /api/orders/:id/status | 更新订单状态 |

### 分类接口
| 方法 | 路径 | 描述 |
|------|------|------|
| GET | /api/categories | 获取所有分类 |
| GET | /api/categories/:id | 获取分类详情 |
| GET | /api/categories/top | 获取顶级分类 |

### 收藏接口
| 方法 | 路径 | 描述 |
|------|------|------|
| POST | /api/favorites | 添加收藏 |
| GET | /api/favorites | 获取收藏列表 |
| DELETE | /api/favorites/:id | 删除收藏 |
| GET | /api/favorites/check | 检查是否收藏 |

## 前端页面

前端页面位于 `frontend/index.html`，包含以下功能:
- 首页展示
- 商品列表
- 商品详情
- 发布商品
- 订单管理
- 收藏管理
- 个人中心

## 配置说明

### config.json 配置项

```json
{
    "app": {
        "address": "0.0.0.0",
        "port": 8080,
        "document_root": "./frontend",
        "upload_path": "./uploads"
    },
    "database": {
        "host": "127.0.0.1",
        "port": 3306,
        "dbname": "campus_trade",
        "user": "root",
        "password": "password"
    },
    "log": {
        "log_path": "./logs",
        "log_level": "DEBUG"
    }
}
```

## 开发说明

### 添加新接口

1. 在 `controller/` 目录下创建控制器类
2. 在 `dao/` 目录下创建数据访问对象
3. 在 `model/` 目录下定义数据模型
4. 在主入口文件中注册控制器

### 数据库迁移

每次修改数据库结构后，更新 `database/campus_trade.sql` 文件。

## 安全注意事项

1. 密码存储使用哈希加密
2. SQL 查询使用参数化查询防止注入
3. 文件上传进行类型和大小验证
4. 接口访问进行权限验证

## License

MIT License

## 作者

校园二手交易系统开发团队