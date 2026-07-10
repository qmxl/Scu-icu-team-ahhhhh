#include "FileUtil.h"
#include <drogon/drogon.h>
#include <fstream>
#include <random>
#include <ctime>

std::string FileUtil::saveUploadedFile(const drogon::UploadFile& file) {
    std::string fileName = generateUniqueFileName(file.getFileName());
    std::string filePath = getUploadPath() + "/" + fileName;

    std::ofstream ofs(filePath, std::ios::binary);
    if (ofs) {
        ofs.write(file.getFileContent().c_str(), file.getFileContent().size());
        ofs.close();
        return "/uploads/" + fileName;
    }
    return "";
}

std::vector<std::string> FileUtil::saveUploadedFiles(const std::vector<drogon::UploadFile>& files) {
    std::vector<std::string> paths;
    for (const auto& file : files) {
        std::string path = saveUploadedFile(file);
        if (!path.empty()) {
            paths.push_back(path);
        }
    }
    return paths;
}

std::string FileUtil::getFileExtension(const std::string& fileName) {
    size_t dotPos = fileName.find_last_of('.');
    if (dotPos != std::string::npos) {
        return fileName.substr(dotPos + 1);
    }
    return "";
}

std::string FileUtil::generateUniqueFileName(const std::string& originalName) {
    std::string ext = getFileExtension(originalName);
    std::string timestamp = std::to_string(std::time(nullptr));
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1000, 9999);
    
    return timestamp + "_" + std::to_string(dis(gen)) + "." + ext;
}

bool FileUtil::isAllowedExtension(const std::string& fileName) {
    std::string ext = getFileExtension(fileName);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    std::vector<std::string> allowedExts = {"jpg", "jpeg", "png", "gif"};
    return std::find(allowedExts.begin(), allowedExts.end(), ext) != allowedExts.end();
}

std::string FileUtil::getUploadPath() {
    auto config = drogon::app().getCustomConfig();
    return config["upload"]["uploadPath"].asString();
}