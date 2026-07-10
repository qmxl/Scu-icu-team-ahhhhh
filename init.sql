CREATE DATABASE IF NOT EXISTS campus_trade DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;

USE campus_trade;

CREATE TABLE IF NOT EXISTS users (
    id INT(11) PRIMARY KEY AUTO_INCREMENT,
    username VARCHAR(50) NOT NULL UNIQUE,
    password VARCHAR(255) NOT NULL,
    phone VARCHAR(20),
    qq VARCHAR(20),
    role TINYINT(1) DEFAULT 0,
    status TINYINT(1) DEFAULT 1,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    updated_at DATETIME DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    INDEX idx_username (username),
    INDEX idx_role (role),
    INDEX idx_status (status)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

CREATE TABLE IF NOT EXISTS goods (
    id INT(11) PRIMARY KEY AUTO_INCREMENT,
    name VARCHAR(200) NOT NULL,
    description TEXT,
    price DECIMAL(10,2) NOT NULL,
    images TEXT,
    category VARCHAR(50),
    seller_id INT(11) NOT NULL,
    is_locked TINYINT(1) DEFAULT 0,
    status TINYINT(1) DEFAULT 1,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    updated_at DATETIME DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    INDEX idx_seller_id (seller_id),
    INDEX idx_status (status),
    INDEX idx_category (category),
    INDEX idx_is_locked (is_locked),
    FOREIGN KEY (seller_id) REFERENCES users(id) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

CREATE TABLE IF NOT EXISTS transactions (
    id VARCHAR(32) PRIMARY KEY,
    goods_id INT(11) NOT NULL,
    buyer_id INT(11) NOT NULL,
    seller_id INT(11) NOT NULL,
    status TINYINT(1) DEFAULT 0,
    start_time DATETIME,
    confirm_time DATETIME,
    cancel_reason VARCHAR(200),
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    updated_at DATETIME DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    INDEX idx_goods_id (goods_id),
    INDEX idx_buyer_id (buyer_id),
    INDEX idx_seller_id (seller_id),
    INDEX idx_status (status),
    INDEX idx_start_time (start_time),
    FOREIGN KEY (goods_id) REFERENCES goods(id) ON DELETE CASCADE,
    FOREIGN KEY (buyer_id) REFERENCES users(id) ON DELETE CASCADE,
    FOREIGN KEY (seller_id) REFERENCES users(id) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

CREATE TABLE IF NOT EXISTS configs (
    id INT(11) PRIMARY KEY AUTO_INCREMENT,
    `key` VARCHAR(100) NOT NULL UNIQUE,
    value TEXT,
    description VARCHAR(200),
    INDEX idx_key (`key`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

DROP TABLE IF EXISTS statistics;

DELIMITER //

CREATE PROCEDURE IF NOT EXISTS create_statistics_table(IN table_name VARCHAR(50))
BEGIN
    SET @sql = CONCAT('CREATE TABLE IF NOT EXISTS ', table_name, ' (
        id INT(11) PRIMARY KEY AUTO_INCREMENT,
        type VARCHAR(50) NOT NULL,
        data TEXT,
        date DATE NOT NULL,
        INDEX idx_type (type),
        INDEX idx_date (date)
    ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4');
    PREPARE stmt FROM @sql;
    EXECUTE stmt;
    DEALLOCATE PREPARE stmt;
END //

DELIMITER ;

CALL create_statistics_table('statistics_202601');
CALL create_statistics_table('statistics_202602');
CALL create_statistics_table('statistics_202603');
CALL create_statistics_table('statistics_202604');
CALL create_statistics_table('statistics_202605');
CALL create_statistics_table('statistics_202606');
CALL create_statistics_table('statistics_202607');
CALL create_statistics_table('statistics_202608');
CALL create_statistics_table('statistics_202609');
CALL create_statistics_table('statistics_202610');
CALL create_statistics_table('statistics_202611');
CALL create_statistics_table('statistics_202612');
CALL create_statistics_table('statistics_202701');
CALL create_statistics_table('statistics_202702');
CALL create_statistics_table('statistics_202703');
CALL create_statistics_table('statistics_202704');
CALL create_statistics_table('statistics_202705');
CALL create_statistics_table('statistics_202706');
CALL create_statistics_table('statistics_202707');
CALL create_statistics_table('statistics_202708');
CALL create_statistics_table('statistics_202709');
CALL create_statistics_table('statistics_202710');
CALL create_statistics_table('statistics_202711');
CALL create_statistics_table('statistics_202712');

INSERT IGNORE INTO configs (`key`, value, description) VALUES 
('site_name', '校园二手交易平台', '网站名称'),
('auto_confirm_days', '7', '自动确认天数'),
('max_image_count', '5', '最大图片数量'),
('max_file_size', '5242880', '最大文件大小(字节)');

INSERT IGNORE INTO users (username, password, phone, role, status) VALUES 
('admin', 'e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855', '13800138000', 1, 1);