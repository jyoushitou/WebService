#include "FarmeWork.h"

// ---------- 工具函数 ----------

std::string GenerateTaskId() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);
    std::stringstream ss;
    std::time_t now = std::time(nullptr);
    ss << std::hex << now;
    for (int i = 0; i < 16; i++) ss << std::hex << dis(gen);
    return ss.str();
}

// 投递任务 → 返回 taskId（前端用这个查结果）
std::string PostUserTask(int userId, UserTaskType type,
                         const std::string& input,
                         const std::string& source) {
    std::lock_guard<std::mutex> lock(g_userThreadsMutex);
    auto it = g_userThreads.find(userId);
    if (it == g_userThreads.end()) return "";

    std::string taskId = GenerateTaskId();

    UserTask task;
    task.taskId = taskId;
    task.type = type;
    task.input = input;
    task.source = source;
    task.createTime = std::time(nullptr);
    task.status = TaskStatus::PENDING;

    {   // 入队
        std::lock_guard<std::mutex> qLock(it->second->taskMutex);
        it->second->taskQueue.push_back(std::move(task));
    }

    // 预留在全局结果表
    {
        std::lock_guard<std::mutex> rLock(g_taskResultsMutex);
        g_taskResults[taskId] = {taskId, userId, TaskStatus::PENDING,
                                 input, "", "", std::time(nullptr), 0};
    }

    it->second->taskCV.notify_one();
    return taskId;
}

// 更新任务结果（由用户线程在处理完后调用）
void UpdateTaskResult(const std::string& taskId, int userId,
                      TaskStatus status,
                      const std::string& output,
                      const std::string& errorMessage) {
    std::lock_guard<std::mutex> lock(g_taskResultsMutex);
    auto it = g_taskResults.find(taskId);
    if (it == g_taskResults.end()) return;
    it->second.status = status;
    it->second.output = output;
    it->second.errorMessage = errorMessage;
    it->second.completeTime = std::time(nullptr);
}

// 按 taskId 查询结果
TaskResult* GetTaskResult(const std::string& taskId) {
    std::lock_guard<std::mutex> lock(g_taskResultsMutex);
    auto it = g_taskResults.find(taskId);
    if (it == g_taskResults.end()) return nullptr;
    return &it->second;
}

// ---------- 用户线程主循环（每个用户一个独立的 "main"） ----------

void UserWorkerRoutine(UserThreadInfo* info) {
    // 用户线程启动日志
    Tools::Out_System("用户线程启动 - 用户: " + info->name +
                      " (ID: " + std::to_string(info->userId) +
                      ", 级别: " + std::to_string(info->level) + ")");

    info->startTime = std::time(nullptr);

    // 每个用户线程有自己的独立 DB 连接（像 main 一样！）
    MySQL::mysql* threadDb = nullptr;
    try {
        threadDb = new MySQL::mysql();
        Tools::Out_System("用户线程 " + info->name + " 数据库连接成功");
    } catch (const std::exception& e) {
        Tools::Out_System_Error("用户线程 " + info->name + " 数据库连接失败: " + std::string(e.what()));
    }

    // ========== 用户线程主循环 ==========
    while (info->running && g_running) {
        std::unique_lock<std::mutex> lock(info->taskMutex);

        // 等任务（最长 5 秒，超时用来做定期任务）
        if (info->taskCV.wait_for(lock, std::chrono::seconds(5), [info]() {
            return !info->taskQueue.empty() || !info->running || !g_running;
        })) {
            // ─── 处理所有待办任务 ───
            while (!info->taskQueue.empty() && info->running && g_running) {
                UserTask task = std::move(info->taskQueue.front());
                info->taskQueue.erase(info->taskQueue.begin());
                lock.unlock();

                UpdateTaskResult(task.taskId, info->userId, TaskStatus::PROCESSING, "");

                try {
                    switch (task.type) {

                        // ===== PING =====
                        case UserTaskType::PING: {
                            std::string echo = ParseJsonString(task.input, "message");
                            std::string result = "{"
                                "\"pong\":true,"
                                "\"echo\":\"" + echo + "\","
                                "\"userId\":" + std::to_string(info->userId) + ","
                                "\"userName\":\"" + info->name + "\"}";
                            UpdateTaskResult(task.taskId, info->userId,
                                             TaskStatus::COMPLETED, result);
                            break;
                        }

                        // ===== PROCESS_DATA =====
                        case UserTaskType::PROCESS_DATA: {
                            std::string content = ParseJsonString(task.input, "content");
                            std::string dataType = ParseJsonString(task.input, "type");

                            Tools::Out_System("用户线程 " + info->name +
                                              " 处理数据: " + dataType +
                                              " / " + content.substr(0, 30) + "...");

                            std::string result;
                            if (threadDb) {
                                // ★ 在这里写你的数据库业务代码 ★
                                // std::string sql = "INSERT INTO logs(user_id,type,content) VALUES("
                                //     + std::to_string(info->userId) + ",'" + dataType + "','" + content + "')";
                                // mysql_query(threadDb->conn, sql.c_str());

                                result = "{"
                                    "\"processed\":true,"
                                    "\"userId\":" + std::to_string(info->userId) + ","
                                    "\"type\":\"" + dataType + "\","
                                    "\"contentLength\":" + std::to_string(content.size()) + ","
                                    "\"savedToDb\":true}";
                            } else {
                                result = "{\"processed\":true,\"savedToDb\":false}";
                            }
                            UpdateTaskResult(task.taskId, info->userId,
                                             TaskStatus::COMPLETED, result);
                            break;
                        }

                        // ===== SEND_NOTIFICATION =====
                        case UserTaskType::SEND_NOTIFICATION: {
                            std::string title = ParseJsonString(task.input, "title");
                            std::string body  = ParseJsonString(task.input, "body");

                            Tools::Out_System("用户线程 " + info->name +
                                              " 通知: " + title + " - " + body);

                            // ★ 在这里写推送逻辑 ★
                            std::string result = "{"
                                "\"notified\":true,"
                                "\"title\":\"" + title + "\","
                                "\"body\":\"" + body + "\","
                                "\"userId\":" + std::to_string(info->userId) + "}";
                            UpdateTaskResult(task.taskId, info->userId,
                                             TaskStatus::COMPLETED, result);
                            break;
                        }

                        // ===== SYNC_DATABASE =====
                        case UserTaskType::SYNC_DATABASE: {
                            std::string table = ParseJsonString(task.input, "table");
                            std::string data  = ParseJsonValueRaw(task.input, "data");

                            Tools::Out_System("用户线程 " + info->name +
                                              " 同步数据库: " + table);

                            std::string result;
                            if (threadDb) {
                                // ★ 在这里写数据同步代码 ★
                                // std::string sql = "UPDATE " + table + " SET ... WHERE user_id="
                                //     + std::to_string(info->userId);
                                // mysql_query(threadDb->conn, sql.c_str());

                                result = "{"
                                    "\"synced\":true,"
                                    "\"table\":\"" + table + "\","
                                    "\"userId\":" + std::to_string(info->userId) + ","
                                    "\"affectedRows\":1}";
                            } else {
                                result = "{\"synced\":false,\"error\":\"数据库不可用\"}";
                            }
                            UpdateTaskResult(task.taskId, info->userId,
                                             TaskStatus::COMPLETED, result);
                            break;
                        }

                        // ===== CUSTOM_EVENT =====
                        case UserTaskType::CUSTOM_EVENT: {
                            std::string eventType = ParseJsonString(task.input, "event");
                            if (eventType.empty()) eventType = "unknown";

                            Tools::Out_System("用户线程 " + info->name +
                                              " 事件: " + eventType);

                            std::string result = "{"
                                "\"event\":\"" + eventType + "\","
                                "\"processed\":true,"
                                "\"userId\":" + std::to_string(info->userId) + ","
                                "\"userName\":\"" + info->name + "\","
                                "\"echo\":" + task.input + "}";
                            UpdateTaskResult(task.taskId, info->userId,
                                             TaskStatus::COMPLETED, result);
                            break;
                        }

                        // ===== SHUTDOWN =====
                        case UserTaskType::SHUTDOWN: {
                            std::string result = "{\"shutdown\":true,\"userId\":"
                                + std::to_string(info->userId) + "}";
                            UpdateTaskResult(task.taskId, info->userId,
                                             TaskStatus::COMPLETED, result);
                            Tools::Out_System("用户线程 " + info->name + " 收到关闭指令");
                            info->running = false;
                            break;
                        }
                    }
                } catch (const std::exception& e) {
                    UpdateTaskResult(task.taskId, info->userId,
                                     TaskStatus::FAILED, "", e.what());
                    Tools::Out_System_Error("用户线程 " + info->name +
                                           " 任务异常: " + std::string(e.what()));
                }

                info->tasksProcessed++;
                lock.lock();
            }
        } else {
            // ─── 5 秒超时 → 做定期后台任务 ───
            lock.unlock();

            static std::time_t lastCleanup = 0;
            std::time_t now = std::time(nullptr);
            if (now - lastCleanup >= 30) {
                lastCleanup = now;
                Tools::Out_System("用户线程 " + info->name +
                                  " 心跳 (已处理 " + std::to_string(info->tasksProcessed.load()) + " 个任务)");
            }
        }
    }

    if (threadDb) {
        Tools::Out_System("用户线程 " + info->name + " 关闭数据库连接");
        delete threadDb;
    }
    Tools::Out_System("用户线程 " + info->name + " 退出 (共处理 " +
                      std::to_string(info->tasksProcessed.load()) + " 个任务)");
}

void CreateUserThread(int userId, const std::string& name, int level) {
    std::lock_guard<std::mutex> lock(g_userThreadsMutex);
    if (g_userThreads.find(userId) != g_userThreads.end()) {
        Tools::Out_System("用户线程 " + name + " 已有线程在运行");
        return;
    }
    auto info = std::make_unique<UserThreadInfo>();
    info->userId = userId;
    info->name = name;
    info->level = level;
    info->running = true;
    info->startTime = std::time(nullptr);
    info->worker = std::thread(UserWorkerRoutine, info.get());
    g_userThreads[userId] = std::move(info);
    Tools::Out_System("为用户 " + name + " (ID: " + std::to_string(userId) +
                      ") 创建了专属业务线程");
}
// ==================== HTTP 服务器路由 ====================

void HttpServerRoutine(int Port) {
    std::cout << "HTTP Server worker started, PID: "
#ifdef _WIN32
              << _getpid()
#else
              << getpid()
#endif
              << std::endl;

    MySQL::mysql* db = nullptr;
    try {
        db = new MySQL::mysql();
        Tools::Out_System("子进程 MySQL 连接成功");
    } catch (const std::exception& e) {
        Tools::Out_System_Error("子进程 MySQL 连接失败: " + std::string(e.what()));
    }

        Http::http_server server(Port, 100);
    g_server = &server;

    // 注册所有业务路由（原分散在下的 lambda 路由已整合到 RegisterBusinessRoutes()）
    server.RegisterBusinessRoutes();

    // ===== POST /api/login（支持多设备登录，依赖 db 局部变量，暂保留此处） =====
    server.Post("/api/login", [db](const Http::HttpRequest& req) -> Http::HttpResponse {
        Http::HttpResponse res;
        res.status_code = 200;
        res.status_text = "OK";
        res.headers["Content-Type"] = "application/json";
        res.headers["Access-Control-Allow-Origin"] = "*";

        if (!db) {
            res.status_code = 500;
            res.body = "{\"status\": \"error\", \"message\": \"数据库不可用\"}";
            return res;
        }

        std::string name = ParseJsonString(req.body, "name");
        std::string password = ParseJsonString(req.body, "password");
        std::string deviceName = ParseJsonString(req.body, "device_name");

        if (name.empty() || password.empty()) {
            res.status_code = 400;
            res.body = "{\"status\": \"fail\", \"message\": \"用户名或密码不能为空\"}";
            return res;
        }

        std::string userAgent;
        auto uaIt = req.headers.find("User-Agent");
        if (uaIt == req.headers.end()) {
            uaIt = req.headers.find("user-agent");
        }
        if (uaIt != req.headers.end()) {
            userAgent = uaIt->second;
        }

        std::string clientIp = req.client_ip;
        if (clientIp.empty()) clientIp = "unknown";

        if (deviceName.empty()) {
            if (!userAgent.empty()) {
                deviceName = userAgent.substr(0, 30);
                size_t spacePos = deviceName.find(' ');
                if (spacePos != std::string::npos && spacePos > 5) {
                    deviceName = deviceName.substr(0, spacePos);
                }
            } else {
                deviceName = "未知设备";
            }
        }

        std::cout << "[登录] 尝试登录 - name: '" << name << "', device: '" << deviceName << "', IP: " << clientIp << std::endl;
        int permission = db->User(name, password);

        if (permission > 0) {
            int userId = db->Get_UserId(name);
            std::string token = GenerateToken();

            {
                std::lock_guard<std::mutex> lock(g_tokenMutex);
                DeviceInfo device;
                device.deviceName = deviceName;
                device.userAgent = userAgent;
                device.clientIp = clientIp;
                device.loginTime = std::time(nullptr);
                g_tokenStore[token] = {userId, permission, name, device};
            }

            std::cout << "[登录] 成功 - 用户: " << name
                      << ", 设备: " << deviceName
                      << ", IP: " << clientIp << std::endl;
            CreateUserThread(userId, name, permission);

            PostUserTask(userId, UserTaskType::CUSTOM_EVENT,
                         "{\"event\":\"login\",\"device\":\"" + deviceName + "\"}", "登录系统");

            std::string devicesJson = "[";
            {
                std::lock_guard<std::mutex> lock(g_tokenMutex);
                bool first = true;
                for (const auto& [t, session] : g_tokenStore) {
                    if (session.userId == userId) {
                        if (!first) devicesJson += ",";
                        first = false;
                        devicesJson += "{";
                        devicesJson += std::string("\"device_name\":\"") + session.device.deviceName + "\",";
                        devicesJson += std::string("\"client_ip\":\"") + session.device.clientIp + "\",";
                        devicesJson += std::string("\"login_time\":") + std::to_string(session.device.loginTime);
                        devicesJson += "}";
                    }
                }
            }
            devicesJson += "]";

            res.body = "{"
                "\"status\": \"success\", "
                "\"message\": \"登录成功\", "
                "\"token\": \"" + token + "\", "
                "\"user_id\": " + std::to_string(userId) + ", "
                "\"level\": " + std::to_string(permission) + ", "
                "\"device_name\": \"" + deviceName + "\", "
                "\"devices\": " + devicesJson + "}";
        } else {
            res.status_code = 401;
            res.body = "{\"status\": \"fail\", \"message\": \"用户名或密码错误\"}";
        }
        return res;
    });

    // ===== GET /api/userinfo =====
    server.Get("/api/userinfo", [](const Http::HttpRequest& req) -> Http::HttpResponse {
        Http::HttpResponse res;
        res.status_code = 200;
        res.status_text = "OK";
        res.headers["Content-Type"] = "application/json";
        res.headers["Access-Control-Allow-Origin"] = "*";

        std::string token = GetTokenFromRequest(req);
        SessionInfo* session = ValidateToken(token);
        if (!session) {
            res.status_code = 401;
            res.body = "{\"status\": \"fail\", \"message\": \"未登录或 token 已失效\"}";
            return res;
        }

        res.body = "{"
            "\"status\": \"success\", "
            "\"user_id\": " + std::to_string(session->userId) + ", "
            "\"level\": " + std::to_string(session->level) + ", "
            "\"name\": \"" + session->name + "\", "
            "\"device_name\": \"" + session->device.deviceName + "\", "
            "\"client_ip\": \"" + session->device.clientIp + "\", "
            "\"login_time\": " + std::to_string(session->device.loginTime) + "}";
        return res;
    });

    // ===== POST /api/task/result =====
    server.Post("/api/task/result", [](const Http::HttpRequest& req) -> Http::HttpResponse {
        Http::HttpResponse res;
        res.status_code = 200;
        res.status_text = "OK";
        res.headers["Content-Type"] = "application/json";
        res.headers["Access-Control-Allow-Origin"] = "*";

        std::string taskId = ParseJsonString(req.body, "task_id");
        if (taskId.empty()) {
            res.body = "{\"status\":\"fail\",\"message\":\"请提供 task_id\"}";
            return res;
        }

        std::string token = GetTokenFromRequest(req);
        SessionInfo* session = ValidateToken(token);
        if (!session) {
            res.status_code = 401;
            res.body = "{\"status\":\"fail\",\"message\":\"未登录\"}";
            return res;
        }

        TaskResult* result = GetTaskResult(taskId);
        if (!result) {
            res.status_code = 404;
            res.body = "{\"status\":\"fail\",\"message\":\"任务不存在或已过期\"}";
            return res;
        }

        if (result->userId != session->userId) {
            res.status_code = 403;
            res.body = "{\"status\":\"fail\",\"message\":\"无权查看此任务\"}";
            return res;
        }

        std::string statusStr;
        switch (result->status) {
            case TaskStatus::PENDING:    statusStr = "pending";    break;
            case TaskStatus::PROCESSING: statusStr = "processing"; break;
            case TaskStatus::COMPLETED:  statusStr = "completed";  break;
            case TaskStatus::FAILED:     statusStr = "failed";     break;
            default:                     statusStr = "unknown";    break;
        }

        res.body = "{"
            "\"status\":\"success\","
            "\"task_id\":\"" + result->taskId + "\","
            "\"task_status\":\"" + statusStr + "\","
            "\"output\":" + (result->output.empty() ? "null" : result->output) + ","
            "\"error\":\"" + result->errorMessage + "\","
            "\"create_time\":" + std::to_string(result->createTime) + ","
            "\"complete_time\":" + std::to_string(result->completeTime) + "}";
        return res;
    });

    // ===== GET /api/devices =====
    server.Get("/api/devices", [](const Http::HttpRequest& req) -> Http::HttpResponse {
        Http::HttpResponse res;
        res.status_code = 200;
        res.status_text = "OK";
        res.headers["Content-Type"] = "application/json";
        res.headers["Access-Control-Allow-Origin"] = "*";

        std::string token = GetTokenFromRequest(req);
        SessionInfo* session = ValidateToken(token);
        if (!session) {
            res.status_code = 401;
            res.body = "{\"status\": \"fail\", \"message\": \"未登录或 token 已失效\"}";
            return res;
        }

        int userId = session->userId;
        std::string devicesJson = "[";
        bool first = true;
        {
            std::lock_guard<std::mutex> lock(g_tokenMutex);
            for (const auto& [t, s] : g_tokenStore) {
                if (s.userId == userId) {
                    if (!first) devicesJson += ",";
                    first = false;
                    devicesJson += "{";
                    devicesJson += std::string("\"token_prefix\":\"") + t.substr(0, 8) + "...\",";
                    devicesJson += std::string("\"is_current\": ") + ((t == token) ? "true" : "false") + ",";
                    devicesJson += std::string("\"device_name\":\"") + s.device.deviceName + "\",";
                    devicesJson += std::string("\"client_ip\":\"") + s.device.clientIp + "\",";
                    devicesJson += std::string("\"user_agent\":\"") + s.device.userAgent + "\",";
                    devicesJson += std::string("\"login_time\":") + std::to_string(s.device.loginTime);
                    devicesJson += "}";
                }
            }
        }
        devicesJson += "]";

        res.body = "{"
            "\"status\": \"success\", "
            "\"user_id\": " + std::to_string(userId) + ", "
            "\"devices\": " + devicesJson + "}";
        return res;
    });

    // ===== POST /api/logout =====
    server.Post("/api/logout", [](const Http::HttpRequest& req) -> Http::HttpResponse {
        Http::HttpResponse res;
        res.status_code = 200;
        res.status_text = "OK";
        res.headers["Content-Type"] = "application/json";
        res.headers["Access-Control-Allow-Origin"] = "*";

        std::string token = GetTokenFromRequest(req);
        std::string deviceName = "未知";
        if (!token.empty()) {
            std::lock_guard<std::mutex> lock(g_tokenMutex);
            auto it = g_tokenStore.find(token);
            if (it != g_tokenStore.end()) {
                deviceName = it->second.device.deviceName;
                int userId = it->second.userId;

                bool otherDevicesOnline = false;
                for (const auto& [otherToken, otherSession] : g_tokenStore) {
                    if (otherToken != token && otherSession.userId == userId) {
                        otherDevicesOnline = true;
                        break;
                    }
                }

                if (!otherDevicesOnline) {
                    std::lock_guard<std::mutex> threadLock(g_userThreadsMutex);
                    auto threadIt = g_userThreads.find(userId);
                    if (threadIt != g_userThreads.end()) {
                        PostUserTask(userId, UserTaskType::SHUTDOWN, "{}", "logout");
                        threadIt->second->running = false;
                        if (threadIt->second->worker.joinable()) {
                            threadIt->second->worker.detach();
                        }
                        g_userThreads.erase(threadIt);
                    }
                                } else {
                    std::cout << "[退出] 用户 " << it->second.name
                              << " 还有其他设备在线，保留用户线程" << std::endl;
                }
                g_tokenStore.erase(it);
                std::cout << "[退出] 设备 '" << deviceName << "' 已注销登录" << std::endl;
            }
        }
        res.body = "{"
            "\"status\": \"success\", "
            "\"message\": \"设备 '";
        res.body += deviceName;
        res.body += "' 已退出登录\"}";
        return res;
    });

    // ===== POST /api/data =====
    server.Post("/api/data", [db](const Http::HttpRequest& req) -> Http::HttpResponse {
        Http::HttpResponse res;
        res.status_code = 200;
        res.status_text = "OK";
        res.headers["Content-Type"] = "application/json";
        res.headers["Access-Control-Allow-Origin"] = "*";

        std::string token = GetTokenFromRequest(req);
        SessionInfo* session = ValidateToken(token);
        if (!session) {
            res.status_code = 401;
            res.body = "{\"status\": \"fail\", \"message\": \"未登录\"}";
            return res;
        }

        std::string taskId = PostUserTask(session->userId, UserTaskType::PROCESS_DATA,
                                          req.body, "POST /api/data");
        if (!taskId.empty()) {
            res.body = "{"
                "\"status\": \"success\", "
                "\"message\": \"数据已提交到用户线程处理\", "
                "\"task_id\": \"" + taskId + "\", "
                "\"user_id\": " + std::to_string(session->userId) + "}";
        } else {
            res.status_code = 500;
            res.body = "{\"status\": \"error\", \"message\": \"用户线程不可用\"}";
        }
        return res;
    });

    // ===== PUT /api/update =====
    server.Put("/api/update", [](const Http::HttpRequest& req) -> Http::HttpResponse {
        Http::HttpResponse res;
        res.status_code = 200;
        res.status_text = "OK";
        res.headers["Content-Type"] = "application/json";
        res.headers["Access-Control-Allow-Origin"] = "*";

        std::string token = GetTokenFromRequest(req);
        SessionInfo* session = ValidateToken(token);
        if (!session) {
            res.status_code = 401;
            res.body = "{\"status\": \"fail\", \"message\": \"未登录\"}";
            return res;
        }

        std::string taskId = PostUserTask(session->userId, UserTaskType::SYNC_DATABASE,
                                          req.body, "PUT /api/update");
        if (!taskId.empty()) {
            res.body = "{"
                "\"status\": \"success\", "
                "\"message\": \"更新请求已提交到用户线程\", "
                "\"task_id\": \"" + taskId + "\", "
                "\"user_id\": " + std::to_string(session->userId) + "}";
        } else {
            res.status_code = 500;
            res.body = "{\"status\": \"error\", \"message\": \"用户线程不可用\"}";
        }
        return res;
    });

    // ===== DELETE /api/delete =====
    server.Delete("/api/delete", [](const Http::HttpRequest& req) -> Http::HttpResponse {
        Http::HttpResponse res;
        res.status_code = 200;
        res.status_text = "OK";
        res.headers["Content-Type"] = "application/json";
        res.headers["Access-Control-Allow-Origin"] = "*";

        std::string token = GetTokenFromRequest(req);
        SessionInfo* session = ValidateToken(token);
        if (!session) {
            res.status_code = 401;
            res.body = "{\"status\": \"fail\", \"message\": \"未登录\"}";
            return res;
        }

        std::string taskId = PostUserTask(session->userId, UserTaskType::CUSTOM_EVENT,
                                          "{\"action\":\"delete\"}", "DELETE /api/delete");
        if (!taskId.empty()) {
            res.body = "{"
                "\"status\": \"success\", "
                "\"message\": \"删除请求已提交到用户线程\", "
                "\"task_id\": \"" + taskId + "\", "
                "\"user_id\": " + std::to_string(session->userId) + "}";
        } else {
            res.status_code = 500;
            res.body = "{\"status\": \"error\", \"message\": \"用户线程不可用\"}";
        }
        return res;
    });

    std::cout << "Server is running on port " << Port << " in child process" << std::endl;
    server.Start(false);

    if (db) {
        delete db;
        db = nullptr;
    }
}

// ==================== 初始化函数 ====================

// 初始化并启动 HTTP 服务器（子进程/子线程中运行，不阻塞主进程）
void Initiave_Http(int Port) {
#ifdef _WIN32
    std::thread serverThread(HttpServerRoutine, Port);  // Windows 下创建线程运行服务器
    serverThread.detach();                               // 分离线程，后台运行
    Tools::Out_System("HTTP 服务器线程已启动 (端口: " + std::to_string(Port) + ")");
#else
    pid_t pid = fork();                                  // Linux 下创建子进程运行服务器
    if (pid == 0) {
        HttpServerRoutine(Port);                         // 子进程中启动 HTTP 服务器
        exit(0);                                         // 子进程退出
    } else if (pid > 0) {
        Tools::Out_System("HTTP 服务器在子进程中运行 (PID: " + std::to_string(pid) +
                          ", 端口: " + std::to_string(Port) + ")");
    } else {
        Tools::Out_System_Error("创建子进程失败");
    }
#endif
}

// 初始化 MySQL 数据库连接
MySQL::mysql* Initiave_MySQL() {
    try {
        return new MySQL::mysql();                       // 创建并返回数据库连接对象
    } catch (const std::exception& e) {
        Tools::Out_System_Error("MySQL 初始化失败: " + std::string(e.what()));
        return nullptr;                                  // 初始化失败返回空指针
    } catch (...) {
        Tools::Out_System_Error("MySQL 初始化失败: 未知错误");
        return nullptr;
    }
}

// ==================== 全局变量定义 ====================

Http::http_server* g_server = nullptr;
std::atomic<bool> g_running(true);
std::map<int, std::unique_ptr<UserThreadInfo>> g_userThreads;
std::mutex g_userThreadsMutex;
std::unordered_map<std::string, TaskResult> g_taskResults;
std::mutex g_taskResultsMutex;

// ==================== JSON 解析工具 ====================

std::string ParseJsonString(const std::string& body, const std::string& key) {
    std::string searchKey = "\"" + key + "\"";
    size_t keyPos = body.find(searchKey);
    if (keyPos == std::string::npos) return "";
    size_t colonPos = body.find(":", keyPos);
    if (colonPos == std::string::npos) return "";
    size_t quoteStart = body.find("\"", colonPos);
    if (quoteStart == std::string::npos) return "";
    size_t quoteEnd = body.find("\"", quoteStart + 1);
    if (quoteEnd == std::string::npos) return "";
    return body.substr(quoteStart + 1, quoteEnd - quoteStart - 1);
}

std::string ParseJsonValueRaw(const std::string& body, const std::string& key) {
    std::string searchKey = "\"" + key + "\"";
    size_t keyPos = body.find(searchKey);
    if (keyPos == std::string::npos) return "";
    size_t colonPos = body.find(":", keyPos + searchKey.length());
    if (colonPos == std::string::npos) return "";
    size_t valueStart = body.find_first_not_of(" \t\n\r", colonPos + 1);
    if (valueStart == std::string::npos) return "";
    if (body[valueStart] == '"') {
        size_t valueEnd = body.find("\"", valueStart + 1);
        if (valueEnd == std::string::npos) return "";
        return body.substr(valueStart + 1, valueEnd - valueStart - 1);
    } else {
        size_t valueEnd = body.find_first_of(",}\n\r", valueStart);
        if (valueEnd == std::string::npos) valueEnd = body.length();
        std::string raw = body.substr(valueStart, valueEnd - valueStart);
        while (!raw.empty() && (raw.back() == ' ' || raw.back() == '\t')) {
            raw.pop_back();
        }
        return raw;
    }
}