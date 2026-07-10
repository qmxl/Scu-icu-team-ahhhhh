#ifndef HTTPLIB_H
#define HTTPLIB_H

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <functional>
#include <memory>
#include <cstring>
#include <sstream>
#include <algorithm>
#include <mutex>
#include <fstream>
#include <thread>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
typedef int ssize_t;
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#endif

namespace httplib {

struct Request {
    std::string method;
    std::string path;
    std::string body;
    std::map<std::string, std::string> headers;
    std::map<std::string, std::string> params;
    
    std::string get_param_value(const std::string& key) const {
        auto it = params.find(key);
        return it != params.end() ? it->second : "";
    }
};

struct Response {
    int status = 200;
    std::string body;
    std::map<std::string, std::string> headers;
    
    void set_content(const std::string& content, const std::string& type) {
        body = content;
        headers["Content-Type"] = type;
        headers["Content-Length"] = std::to_string(content.size());
    }
    
    void set_header(const std::string& key, const std::string& value) {
        headers[key] = value;
    }
};

using Handler = std::function<void(const Request&, Response&)>;

class Server {
public:
    Server() : running_(false), port_(8080) {}
    
    void Get(const std::string& path, Handler handler) {
        routes_["GET " + path] = handler;
    }
    
    void Post(const std::string& path, Handler handler) {
        routes_["POST " + path] = handler;
    }
    
    void Options(const std::string& path, Handler handler) {
        routes_["OPTIONS " + path] = handler;
    }
    
    void set_base_dir(const std::string& dir) {
        base_dir_ = dir;
    }
    
    bool listen(const std::string& host, int port) {
        port_ = port;
        
#ifdef _WIN32
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            return false;
        }
#endif
        
        server_socket_ = socket(AF_INET, SOCK_STREAM, 0);
        if (server_socket_ < 0) {
            return false;
        }
        
        int opt = 1;
#ifdef _WIN32
        setsockopt(server_socket_, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
#else
        setsockopt(server_socket_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
#endif
        
        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = host == "0.0.0.0" ? INADDR_ANY : inet_addr(host.c_str());
        addr.sin_port = htons(port);
        
        if (bind(server_socket_, (sockaddr*)&addr, sizeof(addr)) < 0) {
            return false;
        }
        
        if (::listen(server_socket_, 10) < 0) {
            return false;
        }
        
        running_ = true;
        
        while (running_) {
            sockaddr_in client_addr{};
            socklen_t client_len = sizeof(client_addr);
            
            int client_socket = accept(server_socket_, (sockaddr*)&client_addr, &client_len);
            if (client_socket < 0) continue;
            
            std::thread([this, client_socket]() {
                handle_client(client_socket);
            }).detach();
        }
        
        return true;
    }
    
    void stop() {
        running_ = false;
#ifdef _WIN32
        closesocket(server_socket_);
        WSACleanup();
#else
        close(server_socket_);
#endif
    }
    
private:
    Request parse_request(const std::string& request_str) {
        Request req;
        std::istringstream iss(request_str);
        std::string line;
        
        std::getline(iss, line);
        size_t pos1 = line.find(' ');
        size_t pos2 = line.find(' ', pos1 + 1);
        if (pos1 != std::string::npos && pos2 != std::string::npos) {
            req.method = line.substr(0, pos1);
            std::string full_path = line.substr(pos1 + 1, pos2 - pos1 - 1);
            
            size_t q_pos = full_path.find('?');
            if (q_pos != std::string::npos) {
                req.path = full_path.substr(0, q_pos);
                std::string query = full_path.substr(q_pos + 1);
                parse_query(query, req.params);
            } else {
                req.path = full_path;
            }
        }
        
        while (std::getline(iss, line)) {
            if (line.empty() || line == "\r") break;
            size_t colon = line.find(':');
            if (colon != std::string::npos) {
                std::string key = line.substr(0, colon);
                std::string value = line.substr(colon + 2);
                if (!value.empty() && value.back() == '\r') {
                    value.pop_back();
                }
                req.headers[key] = value;
            }
        }
        
        int content_length = 0;
        if (req.headers.count("Content-Length")) {
            content_length = std::stoi(req.headers["Content-Length"]);
        }
        
        if (content_length > 0) {
            size_t body_pos = request_str.find("\r\n\r\n");
            if (body_pos != std::string::npos) {
                body_pos += 4;
                req.body = request_str.substr(body_pos, content_length);
            }
        }
        
        return req;
    }
    
    void parse_query(const std::string& query, std::map<std::string, std::string>& params) {
        std::string key, value;
        bool in_key = true;
        for (char c : query) {
            if (c == '=') {
                in_key = false;
            } else if (c == '&') {
                params[key] = value;
                key.clear();
                value.clear();
                in_key = true;
            } else {
                if (in_key) key += c;
                else value += c;
            }
        }
        if (!key.empty()) {
            params[key] = value;
        }
    }
    
    void handle_client(int client_socket) {
        char buffer[8192] = {0};
        ssize_t bytes_read = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytes_read <= 0) {
#ifdef _WIN32
            closesocket(client_socket);
#else
            close(client_socket);
#endif
            return;
        }
        
        std::string request_str(buffer, bytes_read);
        
        std::istringstream iss(request_str);
        std::string line;
        std::getline(iss, line);
        
        bool has_expect_continue = false;
        while (std::getline(iss, line)) {
            if (line.empty() || line == "\r") break;
            size_t colon = line.find(':');
            if (colon != std::string::npos) {
                std::string key = line.substr(0, colon);
                std::string value = line.substr(colon + 2);
                if (key == "Expect" && value.find("100-continue") != std::string::npos) {
                    has_expect_continue = true;
                }
            }
        }
        
        if (has_expect_continue) {
            std::string continue_response = "HTTP/1.1 100 Continue\r\n\r\n";
            send(client_socket, continue_response.c_str(), continue_response.size(), 0);
            
            char body_buffer[8192] = {0};
            ssize_t body_bytes = recv(client_socket, body_buffer, sizeof(body_buffer) - 1, 0);
            if (body_bytes > 0) {
                request_str += std::string(body_buffer, body_bytes);
            }
        }
        
        Request req = parse_request(request_str);
        Response res;
        
        std::string route_key = req.method + " " + req.path;
        
        if (routes_.count(route_key)) {
            routes_[route_key](req, res);
        } else if (req.method == "OPTIONS") {
            res.set_header("Access-Control-Allow-Origin", "*");
            res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
            res.set_header("Access-Control-Allow-Headers", "Content-Type");
            res.status = 200;
        } else if (req.method == "GET") {
            std::string file_path = base_dir_;
            if (!req.path.empty() && req.path != "/") {
                if (!file_path.empty() && file_path.back() != '/' && file_path.back() != '\\') {
                    file_path += "/";
                }
                file_path += req.path.substr(1);
            } else {
                if (!file_path.empty() && file_path.back() != '/' && file_path.back() != '\\') {
                    file_path += "/";
                }
                file_path += "index.html";
            }
            serve_file(client_socket, file_path);
            return;
        } else {
            res.status = 404;
            res.set_content("{\"code\":404,\"message\":\"Not Found\"}", "application/json");
        }
        
        send_response(client_socket, res);
        
#ifdef _WIN32
        closesocket(client_socket);
#else
        close(client_socket);
#endif
    }
    
    bool ends_with(const std::string& s, const std::string& suffix) {
        if (s.size() < suffix.size()) return false;
        return s.compare(s.size() - suffix.size(), suffix.size(), suffix) == 0;
    }
    
    void serve_file(int client_socket, const std::string& file_path) {

        
#ifdef _WIN32
        int wlen = MultiByteToWideChar(CP_UTF8, 0, file_path.c_str(), -1, nullptr, 0);
        std::wstring wfile_path(wlen, 0);
        MultiByteToWideChar(CP_UTF8, 0, file_path.c_str(), -1, &wfile_path[0], wlen);
        
        HANDLE hFile = CreateFileW(wfile_path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (hFile == INVALID_HANDLE_VALUE) {
            std::string response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\nContent-Length: 9\r\n\r\nNot Found";
            send(client_socket, response.c_str(), response.size(), 0);
            return;
        }
        
        DWORD file_size = GetFileSize(hFile, nullptr);
        std::string content(file_size, 0);
        DWORD bytes_read = 0;
        ReadFile(hFile, &content[0], file_size, &bytes_read, nullptr);
        CloseHandle(hFile);
#else
        std::ifstream file(file_path, std::ios::binary);
        if (!file) {
            std::string response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\nContent-Length: 9\r\n\r\nNot Found";
            send(client_socket, response.c_str(), response.size(), 0);
            return;
        }
        
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
#endif
        
        std::string content_type = "text/html; charset=utf-8";
        if (ends_with(file_path, ".css")) content_type = "text/css; charset=utf-8";
        else if (ends_with(file_path, ".js")) content_type = "application/javascript; charset=utf-8";
        else if (ends_with(file_path, ".json")) content_type = "application/json";
        else if (ends_with(file_path, ".png")) content_type = "image/png";
        else if (ends_with(file_path, ".jpg") || ends_with(file_path, ".jpeg")) content_type = "image/jpeg";
        else if (ends_with(file_path, ".gif")) content_type = "image/gif";
        
        std::string response = "HTTP/1.1 200 OK\r\nContent-Type: " + content_type + "\r\nContent-Length: " + std::to_string(content.size()) + "\r\n\r\n" + content;
        send(client_socket, response.c_str(), response.size(), 0);
    }
    
    void send_response(int client_socket, const Response& res) {
        std::string response = "HTTP/1.1 " + std::to_string(res.status) + " OK\r\n";
        for (auto& header : res.headers) {
            response += header.first + ": " + header.second + "\r\n";
        }
        response += "\r\n" + res.body;
        send(client_socket, response.c_str(), response.size(), 0);
    }
    
    std::map<std::string, Handler> routes_;
    std::string base_dir_;
    bool running_;
    int port_;
#ifdef _WIN32
    SOCKET server_socket_;
#else
    int server_socket_;
#endif
};

}

#endif