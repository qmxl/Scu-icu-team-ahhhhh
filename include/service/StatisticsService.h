#ifndef STATISTICS_SERVICE_H
#define STATISTICS_SERVICE_H

#include <string>
#include <map>

namespace service {

class StatisticsService {
public:
    static StatisticsService& getInstance();
    
    std::map<std::string, int> getStatistics();
    void updateStatistics();
    void ensureTableExists(const std::string& tableName);
    std::string getTableName(int year, int month);
    std::string getCurrentTableName();
    
private:
    StatisticsService() = default;
    StatisticsService(const StatisticsService&) = delete;
    StatisticsService& operator=(const StatisticsService&) = delete;
};

}

#endif