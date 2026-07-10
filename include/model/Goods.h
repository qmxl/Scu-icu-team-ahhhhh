#ifndef GOODS_H
#define GOODS_H

#include <string>

namespace model {

class Goods {
public:
    int id = 0;
    std::string name;
    std::string description;
    double price = 0;
    std::string images;
    std::string category;
    int seller_id = 0;
    std::string seller_name;
    int is_locked = 0;
    int status = 0;
    std::string created_at;
    std::string updated_at;
};

}

#endif