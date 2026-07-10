#pragma once

#include <string>
#include <vector>
#include <drogon/HttpRequest.h>

class FileUtil {
public:
    // 保存上传的文件
    static std::string saveUploadedFile(const drogon::UploadFile& file);

    // 保存多个上传的文件
    static std::vector<std::string> saveUploadedFiles(const std::vector<drogon::UploadFile>& files);

    // 获取文件扩展名
    static std::string getFileExtension(const std::string& fileName);

    // 生成唯一文件名
    static std::string generateUniqueFileName(const std::string& originalName);

    // 检查文件扩展名是否允许
    static bool isAllowedExtension(const std::string& fileName);

    // 获取上传目录
    static std::string getUploadPath();
};