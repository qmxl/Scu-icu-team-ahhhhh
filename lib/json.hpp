#ifndef JSON_HPP
#define JSON_HPP

#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <cctype>
#include <algorithm>
#include <stdexcept>
#include <utility>

namespace nlohmann {

class json {
public:
    enum class value_t { null, object, array, string, boolean, number };
    
    json() : type_(value_t::null) {}
    json(std::nullptr_t) : type_(value_t::null) {}
    json(bool b) : type_(value_t::boolean) { bool_val_ = b; }
    json(int i) : type_(value_t::number) { num_val_ = i; }
    json(double d) : type_(value_t::number) { num_val_ = d; }
    json(const std::string& s) : type_(value_t::string) { str_val_ = new std::string(s); }
    json(const char* s) : type_(value_t::string) { str_val_ = new std::string(s); }
    
    json(std::initializer_list<std::pair<const std::string, json>> init) {
        type_ = value_t::object;
        object_val_ = new std::map<std::string, json>();
        for (const auto& p : init) {
            (*object_val_)[p.first] = p.second;
        }
    }
    
    json(const json& other) {
        copy(other);
    }
    
    json& operator=(const json& other) {
        if (this != &other) {
            clear();
            copy(other);
        }
        return *this;
    }
    
    json& operator=(std::initializer_list<std::pair<const std::string, json>> init) {
        clear();
        type_ = value_t::object;
        object_val_ = new std::map<std::string, json>();
        for (const auto& p : init) {
            (*object_val_)[p.first] = p.second;
        }
        return *this;
    }
    
    ~json() {
        clear();
    }
    
    static json array() {
        json j;
        j.type_ = value_t::array;
        j.array_val_ = new std::vector<json>();
        return j;
    }
    
    static json object() {
        json j;
        j.type_ = value_t::object;
        j.object_val_ = new std::map<std::string, json>();
        return j;
    }
    
    bool is_null() const { return type_ == value_t::null; }
    bool is_object() const { return type_ == value_t::object; }
    bool is_array() const { return type_ == value_t::array; }
    bool is_string() const { return type_ == value_t::string; }
    bool is_boolean() const { return type_ == value_t::boolean; }
    bool is_number() const { return type_ == value_t::number; }
    
    std::size_t size() const {
        if (type_ == value_t::object) return object_val_->size();
        if (type_ == value_t::array) return array_val_->size();
        return 0;
    }
    
    bool empty() const { return size() == 0; }
    
    json& operator[](const std::string& key) {
        if (type_ != value_t::object) {
            clear();
            type_ = value_t::object;
            object_val_ = new std::map<std::string, json>();
        }
        return (*object_val_)[key];
    }
    
    const json& operator[](const std::string& key) const {
        if (type_ != value_t::object) {
            throw std::out_of_range("key not found");
        }
        auto it = object_val_->find(key);
        if (it == object_val_->end()) {
            throw std::out_of_range("key not found");
        }
        return it->second;
    }
    
    json& operator[](std::size_t idx) {
        if (type_ != value_t::array) {
            clear();
            type_ = value_t::array;
            array_val_ = new std::vector<json>();
        }
        if (idx >= array_val_->size()) {
            array_val_->resize(idx + 1);
        }
        return (*array_val_)[idx];
    }
    
    void push_back(const json& val) {
        if (type_ != value_t::array) {
            clear();
            type_ = value_t::array;
            array_val_ = new std::vector<json>();
        }
        array_val_->push_back(val);
    }
    
    template<typename T>
    T get() const {
        throw std::bad_cast();
    }
    
    operator std::string() const {
        if (type_ == value_t::string) return *str_val_;
        return dump();
    }
    
    operator int() const {
        if (type_ == value_t::number) return static_cast<int>(num_val_);
        throw std::bad_cast();
    }
    
    operator double() const {
        if (type_ == value_t::number) return num_val_;
        throw std::bad_cast();
    }
    
    operator bool() const {
        if (type_ == value_t::boolean) return bool_val_;
        throw std::bad_cast();
    }
    
    bool contains(const std::string& key) const {
        if (type_ != value_t::object) return false;
        return object_val_->count(key) > 0;
    }
    
    std::string dump() const {
        std::ostringstream oss;
        serialize(oss);
        return oss.str();
    }
    
    static json parse(const std::string& s) {
        json j;
        size_t pos = 0;
        j.parse_internal(s, pos);
        return j;
    }
    
private:
    value_t type_;
    union {
        bool bool_val_;
        double num_val_;
        std::string* str_val_;
        std::map<std::string, json>* object_val_;
        std::vector<json>* array_val_;
    };
    
    void clear() {
        switch (type_) {
            case value_t::string: delete str_val_; break;
            case value_t::object: delete object_val_; break;
            case value_t::array: delete array_val_; break;
            default: break;
        }
        type_ = value_t::null;
    }
    
    void copy(const json& other) {
        type_ = other.type_;
        switch (type_) {
            case value_t::boolean: bool_val_ = other.bool_val_; break;
            case value_t::number: num_val_ = other.num_val_; break;
            case value_t::string: str_val_ = new std::string(*other.str_val_); break;
            case value_t::object: object_val_ = new std::map<std::string, json>(*other.object_val_); break;
            case value_t::array: array_val_ = new std::vector<json>(*other.array_val_); break;
            default: break;
        }
    }
    
    void serialize(std::ostringstream& oss) const {
        switch (type_) {
            case value_t::null: oss << "null"; break;
            case value_t::boolean: oss << (bool_val_ ? "true" : "false"); break;
            case value_t::number: oss << num_val_; break;
            case value_t::string: oss << "\"" << escape(*str_val_) << "\""; break;
            case value_t::object: 
                oss << "{";
                for (auto it = object_val_->begin(); it != object_val_->end();) {
                    oss << "\"" << escape(it->first) << "\":";
                    it->second.serialize(oss);
                    if (++it != object_val_->end()) oss << ",";
                }
                oss << "}";
                break;
            case value_t::array:
                oss << "[";
                for (auto it = array_val_->begin(); it != array_val_->end();) {
                    it->serialize(oss);
                    if (++it != array_val_->end()) oss << ",";
                }
                oss << "]";
                break;
        }
    }
    
    std::string escape(const std::string& s) const {
        std::string res;
        for (char c : s) {
            switch (c) {
                case '\\': res += "\\\\"; break;
                case '"': res += "\\\""; break;
                case '\n': res += "\\n"; break;
                case '\r': res += "\\r"; break;
                case '\t': res += "\\t"; break;
                default: res += c; break;
            }
        }
        return res;
    }
    
    void parse_internal(const std::string& s, size_t& pos) {
        skip_ws(s, pos);
        if (pos >= s.size()) return;
        
        char c = s[pos];
        if (c == '{') {
            pos++;
            type_ = value_t::object;
            object_val_ = new std::map<std::string, json>();
            skip_ws(s, pos);
            if (s[pos] != '}') {
                while (true) {
                    skip_ws(s, pos);
                    std::string key = parse_string(s, pos);
                    skip_ws(s, pos);
                    expect(s, pos, ':');
                    json val;
                    val.parse_internal(s, pos);
                    (*object_val_)[key] = val;
                    skip_ws(s, pos);
                    if (s[pos] == ',') {
                        pos++;
                    } else {
                        break;
                    }
                }
            }
            expect(s, pos, '}');
        } else if (c == '[') {
            pos++;
            type_ = value_t::array;
            array_val_ = new std::vector<json>();
            skip_ws(s, pos);
            if (s[pos] != ']') {
                while (true) {
                    json val;
                    val.parse_internal(s, pos);
                    array_val_->push_back(val);
                    skip_ws(s, pos);
                    if (s[pos] == ',') {
                        pos++;
                    } else {
                        break;
                    }
                }
            }
            expect(s, pos, ']');
        } else if (c == '"') {
            type_ = value_t::string;
            str_val_ = new std::string(parse_string(s, pos));
        } else if (c == 't' && s.substr(pos, 4) == "true") {
            type_ = value_t::boolean;
            bool_val_ = true;
            pos += 4;
        } else if (c == 'f' && s.substr(pos, 5) == "false") {
            type_ = value_t::boolean;
            bool_val_ = false;
            pos += 5;
        } else if (c == 'n' && s.substr(pos, 4) == "null") {
            type_ = value_t::null;
            pos += 4;
        } else if (isdigit(c) || c == '-') {
            type_ = value_t::number;
            num_val_ = parse_number(s, pos);
        }
    }
    
    void skip_ws(const std::string& s, size_t& pos) {
        while (pos < s.size() && isspace(s[pos])) pos++;
    }
    
    void expect(const std::string& s, size_t& pos, char expected) {
        skip_ws(s, pos);
        if (s[pos] != expected) {
            throw std::runtime_error("Unexpected character");
        }
        pos++;
    }
    
    std::string parse_string(const std::string& s, size_t& pos) {
        pos++;
        std::string res;
        while (pos < s.size() && s[pos] != '"') {
            if (s[pos] == '\\') {
                pos++;
                switch (s[pos]) {
                    case '\\': res += '\\'; break;
                    case '"': res += '"'; break;
                    case 'n': res += '\n'; break;
                    case 'r': res += '\r'; break;
                    case 't': res += '\t'; break;
                    default: res += s[pos]; break;
                }
            } else {
                res += s[pos];
            }
            pos++;
        }
        pos++;
        return res;
    }
    
    double parse_number(const std::string& s, size_t& pos) {
        size_t start = pos;
        while (pos < s.size() && (isdigit(s[pos]) || s[pos] == '.' || s[pos] == '-' || s[pos] == 'e' || s[pos] == 'E')) {
            pos++;
        }
        return std::stod(s.substr(start, pos - start));
    }
};

template<> inline std::string json::get<std::string>() const {
    if (type_ == value_t::string) return *str_val_;
    throw std::bad_cast();
}

template<> inline int json::get<int>() const {
    if (type_ == value_t::number) return static_cast<int>(num_val_);
    throw std::bad_cast();
}

template<> inline double json::get<double>() const {
    if (type_ == value_t::number) return num_val_;
    throw std::bad_cast();
}

template<> inline bool json::get<bool>() const {
    if (type_ == value_t::boolean) return bool_val_;
    throw std::bad_cast();
}

}

#endif