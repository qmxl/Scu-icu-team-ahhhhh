#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <string>

namespace model {

struct Transaction {
    std::string id;
    int goods_id = 0;
    int buyer_id = 0;
    int seller_id = 0;
    int status = 0;
    std::string start_time;
    std::string confirm_time;
    std::string cancel_reason;
    std::string created_at;
    std::string updated_at;
    std::string goods_name;
    std::string goods_images;
    double goods_price = 0;
    std::string buyer_name;
    std::string seller_name;
};

}

#endif