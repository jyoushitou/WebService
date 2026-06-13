// ============================================================
// https_api.cpp — HTTP 服务器核心实现
// 功能：Socket 生命周期管理、HTTP 协议解析、路由分发、
//       静态文件服务、JSON 工具函数
// ============================================================

#include "../header/https_api.h"

namespace Http {

    // ==================== 构造/析构 ====================

    http_server::http_server(int port, int maxConnections)
        : m_port(port)
        , m_max_connections(maxConnections)
        , m_server_socket(INVALID_SOCKET)
        , m_running(false)
    {
        m_static_dir = Get_Static_Dir();  // 自动查找静态文件目录
    }

    http_server::~http_server() {
        Stop();  // 析构时自动停止服务器
    }

    void http_server::Set_Static_Dir(const std::string& dir) {
        m_static_dir = dir;
    }

    // ============================================================
    // JSON 工具函数
    // 手工解析 JSON（不依赖第三方库），仅支持简单场景
    // ============================================================

    // 从 JSON 字符串中提取指定 key 的值
    // 支持字符串、数字、布尔、对象、数组
    std::string http_server::Get_Json_Value(const std::string& json, const std::string& key) {
        std::string searchKey = "\"" + key + "\"";         // 构造 "key":
        size_t keyPos = json.find(searchKey);
        if (keyPos == std::string::npos) return "";

        size_t colonPos = json.find(":", keyPos + searchKey.length());
        if (colonPos == std::string::npos) return "";

        // 跳过冒号后的空白字符
        size_t valueStart = json.find_first_not_of(" \t\n\r", colonPos + 1);
        if (valueStart == std::string::npos) return "";

        // 判断值类型并分别处理
        if (json[valueStart] == '"') {
            // 字符串值：找下个引号
            size_t valueEnd = json.find("\"", valueStart + 1);
            if (valueEnd == std::string::npos) return "";
            return json.substr(valueStart + 1, valueEnd - valueStart - 1);
        } else if (json[valueStart] == '{' || json[valueStart] == '[') {
            // 对象或数组：用栈计数花括号/方括号
            char closeChar = (json[valueStart] == '{') ? '}' : ']';
            int depth = 1;
            size_t pos = valueStart + 1;
            while (depth > 0 && pos < json.length()) {
                if (json[pos] == json[valueStart]) depth++;
                if (json[pos] == closeChar) depth--;
                pos++;
            }
            return json.substr(valueStart, pos - valueStart);
        } else {
            // 数字或布尔值：遇到逗号或括号结束
            size_t valueEnd = json.find_first_of(",}\n\r", valueStart);
            if (valueEnd == std::string::npos) return json.substr(valueStart);
            return json.substr(valueStart, valueEnd - valueStart);
        }
    }

    // 构建标准 JSON 响应
    // 格式：{"code": 200, "message": "...", "data": ...}
    std::string http_server::Build_Json_Response(int code, const std::string& message, const std::string& data) {
        std::string json = "{";
        json += "\"code\": " + std::to_string(code) + ", ";
        json += "\"message\": \"" + message + "\"";
        if (!data.empty()) {
            json += ", \"data\": " + data;
        }
        json += "}";
        return json;
    }

    // ============================================================
    // 路由注册方法
    // 将路径和处理函数（std::function）存入对应的路由表
    // ============================================================

    void http_server::Get(const std::string& path, std::function<HttpResponse(const HttpRequest&)> handler) {
        m_get_routes[path] = handler;         // 存入 GET 路由表
    }

    void http_server::Post(const std::string& path, std::function<HttpResponse(const HttpRequest&)> handler) {
        m_post_routes[path] = handler;        // 存入 POST 路由表
    }

    void http_server::Put(const std::string& path, std::function<HttpResponse(const HttpRequest&)> handler) {
        m_put_routes[path] = handler;         // 存入 PUT 路由表
    }

    void http_server::Delete(const std::string& path, std::function<HttpResponse(const HttpRequest&)> handler) {
        m_delete_routes[path] = handler;      // 存入 DELETE 路由表
    }

    // ============================================================
    // 内置路由处理器（这些被 FrameWork.cpp 中的 lambda 替代，未使用）
    // ============================================================

    HttpResponse http_server::Handle_Hello(const HttpRequest& req) {
        HttpResponse res;
        res.status_code = 200;
        res.status_text = "OK";
        res.headers["Content-Type"] = "application/json";
        res.headers["Access-Control-Allow-Origin"] = "*";

        std::string path = req.path;
        std::cout << "GET request path: " << path << std::endl;

        auto userAgent = req.headers.find("User-Agent");
        if (userAgent != req.headers.end()) {
            std::cout << "User-Agent: " << userAgent->second << std::endl;
        }

        res.body = "{";
        res.body += "\"message\": \"Hello World\", ";
        res.body += "\"path\": \"" + path + "\"";
        res.body += "}";
        return res;
    }

    HttpResponse http_server::Handle_Data(const HttpRequest& req) {
        HttpResponse res;
        res.status_code = 200;
        res.status_text = "OK";
        res.headers["Content-Type"] = "application/json";
        res.headers["Access-Control-Allow-Origin"] = "*";

        std::string body = req.body;
        std::cout << "Received POST body: " << body << std::endl;

        auto contentType = req.headers.find("Content-Type");
        if (contentType != req.headers.end()) {
            std::cout << "Content-Type: " << contentType->second << std::endl;
        }

        std::string name = Get_Json_Value(body, "name");
        std::string age = Get_Json_Value(body, "age");
        std::cout << "Parsed data - name: " << name << ", age: " << age << std::endl;

        res.body = "{";
        res.body += "\"status\": \"success\", ";
        res.body += "\"message\": \"Data received\", ";
        res.body += "\"received_name\": \"" + name + "\", ";
        res.body += "\"received_age\": " + age;
        res.body += "}";
        return res;
    }

    HttpResponse http_server::Handle_Update(const HttpRequest& req) {
        HttpResponse res;
        res.status_code = 200;
        res.status_text = "OK";
        res.headers["Content-Type"] = "application/json";
        res.headers["Access-Control-Allow-Origin"] = "*";

        std::string body = req.body;
        std::cout << "Received PUT body: " << body << std::endl;

        res.body = "{";
        res.body += "\"status\": \"success\", ";
        res.body += "\"message\": \"Update received\", ";
        res.body += "\"received_data\": " + body;
        res.body += "}";
        return res;
    }

    HttpResponse http_server::Handle_Delete(const HttpRequest& req) {
        HttpResponse res;
        res.status_code = 200;
        res.status_text = "OK";
        res.headers["Content-Type"] = "application/json";
        res.headers["Access-Control-Allow-Origin"] = "*";

        std::string body = req.body;
        std::string path = req.path;

        std::cout << "DELETE request - path: " << path << std::endl;
        std::cout << "DELETE request - body: " << body << std::endl;

        // 从路径提取 ID（如 /api/delete/123 → 123）
        size_t lastSlash = path.find_last_of('/');
        std::string id = "";
        if (lastSlash != std::string::npos) {
            id = path.substr(lastSlash + 1);
            if (!id.empty() && std::all_of(id.begin(), id.end(), ::isdigit)) {
                std::cout << "Deleting record with ID: " << id << std::endl;
            }
        }

        res.body = "{";
        res.body += "\"status\": \"success\", ";
        res.body += "\"message\": \"Delete request processed\"";
        if (!id.empty()) {
            res.body += ", \"deleted_id\": " + id;
        }
        res.body += "}";
        return res;
    }

    // ============================================================
    // 获取静态文件目录
    // 自动查找前端构建输出的位置（支持多种目录结构）
    // ============================================================

    std::string http_server::Get_Static_Dir() {
        std::string exeDir;

        // 获取可执行文件所在目录
        #ifdef _WIN32
            char path[MAX_PATH];
            GetModuleFileNameA(NULL, path, MAX_PATH);  // Windows API
            exeDir = std::string(path);
            size_t pos = exeDir.find_last_of("\\/");
            if (pos != std::string::npos) {
                exeDir = exeDir.substr(0, pos);
            }
        #else
            char path[1024];
            ssize_t len = readlink("/proc/self/exe", path, sizeof(path) - 1);  // Linux 读取符号链接
            if (len != -1) {
                path[len] = '\0';
                exeDir = std::string(path);
                size_t pos = exeDir.find_last_of("\\/");
                if (pos != std::string::npos) {
                    exeDir = exeDir.substr(0, pos);
                }
            }
        #endif

        Tools::Out_System_Http("Executable directory: " + exeDir);

        // 尝试多个可能的静态文件位置
        std::vector<std::string> candidates = {
            exeDir + "/../../frontend",
            exeDir + "/../../../frontend",
            exeDir + "/../frontend",
            exeDir + "/frontend",
            "../frontend",
            "../../frontend",
            "../../../frontend",
            "frontend"
        };

        for (const auto& dir : candidates) {
            std::string indexPath = dir + "/index.html";
            Tools::Out_System_Http(std::string("Checking: ") + fs::absolute(indexPath).string());
            if (fs::exists(indexPath)) {
                Tools::Out_System_Http(std::string("Found static directory: ") + fs::absolute(dir).string());
                return dir;
            }
        }

        // 没找到则创建默认目录
        Tools::Out_System_Http("[Debug] Static directory not found, creating...");
        std::string fallbackDir = exeDir + "/frontend";
        fs::create_directories(fallbackDir);
        return fallbackDir;
    }

    // ============================================================
    // 初始化服务器
    //
    // TCP Socket 创建流程：
    // 1. socket() — 创建 Socket 文件描述符
    // 2. setsockopt() — 设置 SO_REUSEADDR 允许端口重用
    // 3. bind() — 绑定到指定 IP 和端口