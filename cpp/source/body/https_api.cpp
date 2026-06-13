#include "../header/https_api.h"

namespace Http {

        http_server::http_server(int port, int maxConnections)
        : m_port(port)
        , m_max_connections(maxConnections)
        , m_server_socket(INVALID_SOCKET)
        , m_running(false)
    {
        m_static_dir = Get_Static_Dir();
    }

        http_server::~http_server() {
        Stop();
    }

    void http_server::Set_Static_Dir(const std::string& dir) {
        m_static_dir = dir;
    }

    // ============================================================
    // JSON 工具函数
    // ============================================================
    std::string http_server::Get_Json_Value(const std::string& json, const std::string& key) {
        std::string searchKey = "\"" + key + "\"";
        size_t keyPos = json.find(searchKey);
        if (keyPos == std::string::npos) return "";
        
        size_t colonPos = json.find(":", keyPos + searchKey.length());
        if (colonPos == std::string::npos) return "";
        
        // 跳过空格
        size_t valueStart = json.find_first_not_of(" \t\n\r", colonPos + 1);
        if (valueStart == std::string::npos) return "";
        
        // 判断值类型
        if (json[valueStart] == '"') {
            // 字符串值
            size_t valueEnd = json.find("\"", valueStart + 1);
            if (valueEnd == std::string::npos) return "";
            return json.substr(valueStart + 1, valueEnd - valueStart - 1);
        } else if (json[valueStart] == '{' || json[valueStart] == '[') {
            // 对象或数组 - 简单处理
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
            // 数字或布尔值
            size_t valueEnd = json.find_first_of(",}\n\r", valueStart);
            if (valueEnd == std::string::npos) return json.substr(valueStart);
            return json.substr(valueStart, valueEnd - valueStart);
        }
    }
    
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
    // ============================================================
    void http_server::Get(const std::string& path, std::function<HttpResponse(const HttpRequest&)> handler) {
        m_get_routes[path] = handler;
    }

    void http_server::Post(const std::string& path, std::function<HttpResponse(const HttpRequest&)> handler) {
        m_post_routes[path] = handler;
    }

    void http_server::Put(const std::string& path, std::function<HttpResponse(const HttpRequest&)> handler) {
        m_put_routes[path] = handler;
    }

    void http_server::Delete(const std::string& path, std::function<HttpResponse(const HttpRequest&)> handler) {
        m_delete_routes[path] = handler;
    }

    HttpResponse http_server::Handle_Hello(const HttpRequest& req) {
        HttpResponse res;
        res.status_code = 200;
        res.status_text = "OK";
        res.headers["Content-Type"] = "application/json";
        res.headers["Access-Control-Allow-Origin"] = "*";
        
        // 从请求路径中获取信息
        std::string path = req.path;
        std::cout << "GET request path: " << path << std::endl;
        
        // 从请求头中获取信息
        auto userAgent = req.headers.find("User-Agent");
        if (userAgent != req.headers.end()) {
            std::cout << "User-Agent: " << userAgent->second << std::endl;
        }

        // 构建 JSON 响应
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
        
        // 获取请求体中的 JSON 数据
        std::string body = req.body;
        std::cout << "Received POST body: " << body << std::endl;
        
        // 检查 Content-Type
        auto contentType = req.headers.find("Content-Type");
        if (contentType != req.headers.end()) {
            std::cout << "Content-Type: " << contentType->second << std::endl;
        }

                // 使用类中的 JSON 工具函数解析数据
        std::string name = Get_Json_Value(body, "name");
        std::string age = Get_Json_Value(body, "age");

        std::cout << "Parsed data - name: " << name << ", age: " << age << std::endl;

        // 构建响应
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
        
        // 处理更新逻辑
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
        
        // 从路径中提取 ID（例如 /api/delete/123）
        size_t lastSlash = path.find_last_of('/');
        std::string id = "";
        if (lastSlash != std::string::npos) {
            id = path.substr(lastSlash + 1);
            // 检查是否是数字
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
    // ============================================================
    std::string http_server::Get_Static_Dir() {
        std::string exeDir;

        #ifdef _WIN32
            char path[MAX_PATH];
            GetModuleFileNameA(NULL, path, MAX_PATH);
            exeDir = std::string(path);
            size_t pos = exeDir.find_last_of("\\/");
            if (pos != std::string::npos) {
                exeDir = exeDir.substr(0, pos);
            }
        #else
            char path[1024];
            ssize_t len = readlink("/proc/self/exe", path, sizeof(path) - 1);
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

        Tools::Out_System_Http("[Debug] Static directory not found, creating...");
        std::string fallbackDir = exeDir + "/frontend";
        fs::create_directories(fallbackDir);
        return fallbackDir;
    }

    // ============================================================
    // 初始化服务器
    // ============================================================
    bool http_server::Initialize() {
        #ifdef _WIN32
            WSADATA wsaData;
            if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
                std::cerr << "Failed to initialize Winsock" << std::endl;
                return false;
            }
        #endif

                std::cout << "Static files directory: " << fs::absolute(m_static_dir) << std::endl;
        if (!fs::exists(m_static_dir)) {
            fs::create_directories(m_static_dir);
            std::cout << "Created static directory: " << m_static_dir << std::endl;
        }

        m_server_socket = socket(AF_INET, SOCK_STREAM, 0);
        #ifdef _WIN32
            if (m_server_socket == INVALID_SOCKET) {
        #else
            if (m_server_socket < 0) {
        #endif
                std::cerr << "Failed to create socket" << std::endl;
                #ifdef _WIN32
                    WSACleanup();
                #endif
                return false;
            }

        #ifdef _WIN32
            char opt = 1;
        #else
            int opt = 1;
        #endif
        if (setsockopt(m_server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
            std::cerr << "Failed to set socket options" << std::endl;
            close(m_server_socket);
            #ifdef _WIN32
                WSACleanup();
            #endif
            return false;
        }

        struct sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_port = htons(m_port);

        if (bind(m_server_socket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
            std::cerr << "Failed to bind socket to port " << m_port << std::endl;
            close(m_server_socket);
            #ifdef _WIN32
                WSACleanup();
            #endif
            return false;
        }

        return true;
    }

    // ============================================================
    // 启动服务器
    // ============================================================
    void http_server::Start(bool async) {
        if (m_running) {
            std::cout << "Server is already running" << std::endl;
            return;
        }

        if (m_server_socket == INVALID_SOCKET) {
            if (!Initialize()) {
                return;
            }
        }

        if (listen(m_server_socket, m_max_connections) < 0) {
            std::cerr << "Failed to listen on socket" << std::endl;
            close(m_server_socket);
            #ifdef _WIN32
                WSACleanup();
            #endif
            return;
        }

        m_running = true;
        std::cout << "Server started at http://0.0.0.0:" << m_port << std::endl;

        if (async) {
                        std::thread([this]() {
                Run_Accept_Loop();
            }).detach();
        } else {
            Run_Accept_Loop();
        }
    }

    // ============================================================
    // 接受连接循环
    // ============================================================
    void http_server::Run_Accept_Loop() {
        while (m_running) {
            struct sockaddr_in clientAddr;
            #ifdef _WIN32
                int clientAddrLen = sizeof(clientAddr);
            #else
                socklen_t clientAddrLen = sizeof(clientAddr);
            #endif

            SOCKET clientSocket = accept(m_server_socket, (struct sockaddr*)&clientAddr, &clientAddrLen);
            #ifdef _WIN32
                if (clientSocket == INVALID_SOCKET) {
            #else
                if (clientSocket < 0) {
            #endif
                    if (m_running) {
                        std::cerr << "Failed to accept connection" << std::endl;
                    }
                    continue;
                }

            std::thread clientThread(Handle_Client, clientSocket, this);
            clientThread.detach();
        }
    }

    // ============================================================
    // 停止服务器
    // ============================================================
    void http_server::Stop() {
        if (!m_running) return;
        
        m_running = false;
        
        if (m_server_socket != INVALID_SOCKET) {
            close(m_server_socket);
            m_server_socket = INVALID_SOCKET;
        }

        #ifdef _WIN32
            WSACleanup();
        #endif

        std::cout << "Server stopped" << std::endl;
    }

        // ============================================================
    // 处理客户端连接
    // ============================================================
        void http_server::Handle_Client(SOCKET clientSocket, http_server* server) {
    // 获取客户端 IP
    struct sockaddr_in clientAddr;
    #ifdef _WIN32
        int clientAddrLen = sizeof(clientAddr);
    #else
        socklen_t clientAddrLen = sizeof(clientAddr);
    #endif
    std::string clientIp = "unknown";
    if (getpeername(clientSocket, (struct sockaddr*)&clientAddr, &clientAddrLen) == 0) {
        clientIp = inet_ntoa(clientAddr.sin_addr);
    }
        char buffer[8192];
        std::string request;

        int bytesRead;
        while ((bytesRead = read(clientSocket, buffer, static_cast<int>(sizeof(buffer) - 1))) > 0) {
            buffer[bytesRead] = '\0';
            request += buffer;

            // 查找请求头结束标志
            size_t headerEnd = request.find("\r\n\r\n");
            if (headerEnd != std::string::npos) {
                // 大小写不敏感查找 Content-Length
                std::string headerPart = request.substr(0, headerEnd);
                size_t contentLengthPos = std::string::npos;
                
                // 尝试几种常见写法
                std::vector<std::string> clKeys = {"Content-Length: ", "content-length: ", "Content-length: "};
                for (const auto& key : clKeys) {
                    contentLengthPos = headerPart.find(key);
                    if (contentLengthPos != std::string::npos) {
                        break;
                    }
                }
                
                if (contentLengthPos != std::string::npos) {
                    size_t endOfLine = request.find("\r\n", contentLengthPos);
                    if (endOfLine == std::string::npos) endOfLine = request.size();
                    
                    // 找到冒号后的数字
                    size_t colonPos = request.find(":", contentLengthPos);
                    std::string afterColon = request.substr(colonPos + 1, endOfLine - colonPos - 1);
                    // 去掉前后空格和\r
                    size_t firstDigit = afterColon.find_first_of("0123456789");
                    size_t lastDigit = afterColon.find_last_of("0123456789");
                    if (firstDigit != std::string::npos && lastDigit != std::string::npos) {
                        std::string lengthStr = afterColon.substr(firstDigit, lastDigit - firstDigit + 1);
                        size_t contentLength = std::stoul(lengthStr);
                        size_t bodyStart = headerEnd + 4;
                        if (request.size() - bodyStart >= contentLength) {
                            break;  // body 已收全
                        }
                        // body 没收全，继续读取
                    } else {
                        break;  // Content-Length 值异常
                    }
                } else {
                    break;  // 没有 Content-Length，GET/DELETE 等请求直接退出
                }
            }
        }

                if (!request.empty()) {
            HttpRequest req = server->Request_Parse(request, clientIp);
            HttpResponse res;

            // 处理 OPTIONS 请求（CORS 预检请求）
            if (req.method == "OPTIONS") {
                res.status_code = 204;
                res.status_text = "No Content";
                res.headers["Access-Control-Allow-Origin"] = "*";
                res.headers["Access-Control-Allow-Methods"] = "GET, POST, PUT, DELETE, OPTIONS";
                res.headers["Access-Control-Allow-Headers"] = "Content-Type, Authorization";
                res.headers["Access-Control-Max-Age"] = "86400";
            } else {
                // 根据请求方法查找路由
                bool routeFound = false;
                
                if (req.method == "GET") {
                                        auto it = server->m_get_routes.find(req.path);
                    if (it != server->m_get_routes.end()) {
                        res = it->second(req);
                        routeFound = true;
                    }
                } else if (req.method == "POST") {
                    auto it = server->m_post_routes.find(req.path);
                    if (it != server->m_post_routes.end()) {
                        res = it->second(req);
                        routeFound = true;
                    }
                } else if (req.method == "PUT") {
                    auto it = server->m_put_routes.find(req.path);
                    if (it != server->m_put_routes.end()) {
                        res = it->second(req);
                        routeFound = true;
                    }
                } else if (req.method == "DELETE") {
                    auto it = server->m_delete_routes.find(req.path);
                    if (it != server->m_delete_routes.end()) {
                        res = it->second(req);
                        routeFound = true;
                    }
                }
                
                // 如果没有找到路由，使用默认处理
                if (!routeFound) {
                    res = server->Default_Request_Handle(req);
                }
                
                // 添加 CORS 头
                res.headers["Access-Control-Allow-Origin"] = "*";
            }

            std::string response = server->Response_Build(res);
            write(clientSocket, response.c_str(), static_cast<int>(response.size()));
        }

        close(clientSocket);
    }

    // ============================================================
    // HTTP 解析器
    // ============================================================
        HttpRequest http_server::Request_Parse(const std::string& raw, const std::string& client_ip) {
        HttpRequest req;
        req.client_ip = client_ip;
        std::istringstream stream(raw);
        std::string line;

        std::getline(stream, line);
        std::istringstream lineStream(line);
        lineStream >> req.method >> req.path >> req.version;

        while (std::getline(stream, line) && line != "\r" && !line.empty()) {
            size_t colonPos = line.find(':');
            if (colonPos != std::string::npos) {
                std::string key = line.substr(0, colonPos);
                std::string value = line.substr(colonPos + 2);
                if (!value.empty() && value.back() == '\r') {
                    value.pop_back();
                }
                req.headers[key] = value;
            }
        }

                auto it = req.headers.find("Content-Length");
        if (it == req.headers.end()) {
            it = req.headers.find("content-length");
        }
        if (it == req.headers.end()) {
            it = req.headers.find("Content-length");
        }
        if (it != req.headers.end()) {
            size_t contentLength = std::stoul(it->second);
            std::string body(contentLength, '\0');
            stream.read(&body[0], contentLength);
            req.body = body;
        }

        return req;
    }

    // ============================================================
    // 构建 HTTP 响应
    // ============================================================
    std::string http_server::Response_Build(const HttpResponse& res) {
        std::ostringstream response;
        response << "HTTP/1.1 " << res.status_code << " " << res.status_text << "\r\n";
        for (const auto& [key, value] : res.headers) {
            response << key << ": " << value << "\r\n";
        }
        response << "Content-Length: " << res.body.size() << "\r\n";
        response << "\r\n";
        response << res.body;
        return response.str();
    }

    // ============================================================
    // 默认请求处理器
    // ============================================================
    HttpResponse http_server::Default_Request_Handle(const HttpRequest& req) {
        Tools::Out_System_Http("[Request] " + req.method + " " + req.path);
        HttpResponse res;
        res.headers["Server"] = "Native Socket Server";

        // 静态文件服务
        std::string path = m_static_dir + (req.path == "/" ? "/index.html" : req.path);

        try {
            fs::path fullPath = fs::canonical(fs::absolute(path));
            fs::path staticPath = fs::canonical(fs::absolute(m_static_dir));

            if (fullPath.string().find(staticPath.string()) != 0) {
                res.status_code = 404;
                res.status_text = "Not Found";
                res.headers["Content-Type"] = MIME_HTML;
                res.body = "<h1>404 Not Found</h1><p>The requested resource was not found.</p>";
                return res;
            }

            std::string content = Tools::Read_File(fullPath.string());
            if (content.empty()) {
                res.status_code = 404;
                res.status_text = "Not Found";
                res.headers["Content-Type"] = MIME_HTML;
                res.body = "<h1>404 Not Found</h1><p>The requested resource was not found.</p>";
                return res;
            }

            std::string ext = fullPath.extension().string();
            res.status_code = 200;
            res.status_text = "OK";
            res.headers["Content-Type"] = Tools::Get_MimeType(ext);
            res.body = std::move(content);
        } 
        catch (const std::exception&) {
            res.status_code = 404;
            res.status_text = "Not Found";
            res.headers["Content-Type"] = MIME_HTML;
            res.body = "<h1>404 Not Found</h1><p>The requested resource was not found.</p>";
        }

        return res;
    }
}