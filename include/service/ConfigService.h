#ifndef CONFIG_SERVICE_H
#define CONFIG_SERVICE_H

#include <string>
#include <map>

namespace service {

class ConfigService {
public:
    static ConfigService& getInstance();
    
    std::string getConfig(const std::string& key);
    bool setConfig(const std::string& key, const std::string& value, const std::string& description = "");
    std::map<std::string, std::string> getAllConfigs();
    
private:
    ConfigService() = default;
    ConfigService(const ConfigService&) = delete;
    ConfigService& operator=(const ConfigService&) = delete;
};

}

#endif