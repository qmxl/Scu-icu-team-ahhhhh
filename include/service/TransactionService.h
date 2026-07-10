#ifndef TRANSACTION_SERVICE_H
#define TRANSACTION_SERVICE_H

#include "model/Transaction.h"
#include <string>
#include <vector>

namespace service {

class TransactionService {
public:
    static TransactionService& getInstance();
    
    bool createTransaction(int goodsId, int buyerId, int sellerId);
    bool confirmTransaction(const std::string& transactionId);
    bool completeTransaction(const std::string& transactionId);
    bool cancelTransaction(const std::string& transactionId, const std::string& reason);
    model::Transaction getTransactionById(const std::string& transactionId);
    std::vector<model::Transaction> getTransactionsByBuyer(int buyerId, int page, int pageSize);
    std::vector<model::Transaction> getTransactionsBySeller(int sellerId, int page, int pageSize);
    int getTransactionCount();
    int getTransactionCountByStatus(int status);
    void checkAutoConfirm();
    
private:
    TransactionService() = default;
    TransactionService(const TransactionService&) = delete;
    TransactionService& operator=(const TransactionService&) = delete;
    
    std::string generateTransactionId();
};

}

#endif