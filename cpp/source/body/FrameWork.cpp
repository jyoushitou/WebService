// ============================================================
// FrameWork.cpp — 核心框架文件
// 功能：全局变量定义、子进程/子线程创建HTTP服务器、
//       HTTP服务器在子进程/子线程中运行、MySQL数据库初始化、
//       Token 鉴权系统
// ============================================================

#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <unordered_map>
#include <functional>
#include <thread>
#include <chrono>
#include <vector>
#include <cstdlib>
#include <atomic>
#include <algorithm>
#include <mutex>
#include <condition_variable>
#include <random>
#include <iomanip>
#include <ctime>

#ifdef _WIN32
// g++ 14+ 已内置 NOMINMAX，无需重复定义
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <winsock2.h>
#include <windows.h>
#include <process.h>
#else
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#endif

#include "MyMySQL.h"
#include "https_api.h"
#include "Tools.h"

// ==================== 全局变量 ====================

Http::http_server* g_server = nullptr;
std::atomic<bool> g_running(true);

// ==================== Token 鉴权系统（支持多设备） ====================

struct DeviceInfo {
    std::string deviceName;   // 自定义设备名（前端传入）
    std::string userAgent;    // User-Agent 头
    std::string clientIp;     // 客户端 IP
    std::time_t loginTime;    // 登录时间
};

struct SessionInfo {
    int userId;
    int level;
    std::string name;
    DeviceInfo device;
};

std::unordered_map<std::string, SessionInfo> g_tokenStore;
std::mutex g_tokenMutex;

std::string GenerateToken() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);
    std::stringstream ss;
    for (int i = 0; i < 32; i++) {
        ss << std::hex << dis(gen);
    }
    return ss.str();
}

std::string GetTokenFromRequest(const Http::HttpRequest& req) {
    auto authIt = req.headers.find("Authorization");
    if (authIt == req.headers.end()) {
        authIt = req.headers.find("authorization");
    }
    if (authIt == req.headers.end()) return "";

    std::string auth = authIt->second;
    size_t bearerPos = auth.find("Bearer ");
    if (bearerPos == std::string::npos) {
        bearerPos = auth.find("bearer ");
    }
    if (bearerPos == std::string::npos) return auth;

    return auth.substr(bearerPos + 7);
}

SessionInfo* ValidateToken(const std::string& token) {
    if (token.empty()) return nullptr;
    std::lock_guard<std::mutex> lock(g_tokenMutex);
    auto it = g_tokenStore.find(token);
    if (it == g_tokenStore.end()) return nullptr;
    return &it->second;
}

// ===================================================================
// 异步任务系统（每个用户线程 = 独立业务处理器，带完整入参/出参/状态）
// ===================================================================

// 任务状态周期：PENDING → PROCESSING → COMPLETED / FAILED
enum class TaskStatus {
    PENDING,
    PROCESSING,
    COMPLETED,
    FAILED
};

// 用户线程可执行的任务类型
enum class UserTaskType {
    PING,               // 心跳/保活
    PROCESS_DATA,       // 处理用户提交的数据
    SEND_NOTIFICATION,  // 向用户推送通知
    SYNC_DATABASE,      // 同步用户数据到数据库
    CUSTOM_EVENT,       // 自定义事件
    SHUTDOWN            // 关闭线程
};

// ===== 单个任务定义（入参 + 出参 + 状态，完整！） =====
struct UserTask {
    // --- 元信息 ---
    std::string taskId;          // 唯一 ID，可返回给前端查询
    UserTaskType type;
    std::time_t createTime;
    std::string source;          // 来源描述（如 "POST /api/data"）

    // --- 入参（路由 / 前端传入） ---
    std::string input;           // 输入 JSON

    // --- 出参（线程处理后写入） ---
    TaskStatus status;
    std::string output;          // 输出 JSON
    std::string errorMessage;    // 错误信息
    std::time_t completeTime;
};

// ===== 用户线程信息 =====
struct UserThreadInfo {
    int userId;
    int level;
    std::string name;
    std::thread worker;
    std::atomic<bool> running{true};

    std::vector<UserTask> taskQueue;
    std::mutex taskMutex;
    std::condition_variable taskCV;

    std::atomic<int> tasksProcessed{0};
    std::time_t startTime;
};

std::map<int, std::unique_ptr<UserThreadInfo>> g_userThreads;
std::mutex g_userThreadsMutex;

// ===== 全局任务结果存储 =====
// taskId → 结果。前端 / 路由可根据 taskId 随时查询
struct TaskResult {
    std::string taskId;
    int userId;
    TaskStatus status;
    std::string input;
    std::string output;
    std::string errorMessage;
    std::time_t createTime;
    std::time_t completeTime;
};

std::unordered_map<std::string, TaskResult> g_taskResults;
std::mutex g_taskResultsMutex;

// ---------- 前向声明（JSON 解析函数定义在后面，先声明给用户线程用） ----------
std::string ParseJsonString(const std::string& body, const std::string& key);
std::string ParseJsonValueRaw(const std::string& body, const std::string& key);

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
                         const std::string& source = "") {
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
                      const std::string& errorMessage = "") {
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

// ==================== JSON 解析工具 ====================

// 取 JSON 中字符串类型的值（带引号）
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

// 取 JSON 中任意类型的值（字符串/数字/布尔），返回原始文本
std::string ParseJsonValueRaw(const std::string& body, const std::string& key) {
    std::string searchKey = "\"" + key + "\"";
    size_t keyPos = body.find(searchKey);
    if (keyPos == std::string::npos) return "";

    size_t colonPos = body.find(":", keyPos + searchKey.length());
    if (colonPos == std::string::npos) return "";

    // 跳过空格
    size_t valueStart = body.find_first_not_of(" \t\n\r", colonPos + 1);
    if (valueStart == std::string::npos) return "";

    // 判断值类型
    if (body[valueStart] == '"') {
        // 字符串值
        size_t valueEnd = body.find("\"", valueStart + 1);
        if (valueEnd == std::string::npos) return "";
        return body.substr(valueStart + 1, valueEnd - valueStart - 1);
    } else {
        // 数字或布尔值 — 找到逗号、花括号闭合或换行
        size_t valueEnd = body.find_first_of(",}\n\r", valueStart);
        if (valueEnd == std::string::npos) valueEnd = body.length();
        std::string raw = body.substr(valueStart, valueEnd - valueStart);
        // 去掉尾部空格
        while (!raw.empty() && (raw.back() == ' ' || raw.back() == '\t')) {
            raw.pop_back();
        }
        return raw;
    }
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

    // ===== GET /api/hello =====
    server.Get("/api/hello", [](const Http::HttpRequest& req) -> Http::HttpResponse {
        Http::HttpResponse res;
        res.status_code = 200;
        res.status_text = "OK";
        res.headers["Content-Type"] = "application/json";
        res.headers["Access-Control-Allow-Origin"] = "*";
        res.body = "{\"message\": \"Hello from server\", \"pid\": " +
                   std::to_string(
#ifdef _WIN32
                       _getpid()
#else
                       getpid()
#endif
                   ) + "}";
        return res;
    });

    // ===== POST /api/login（支持多设备登录） =====
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
        // 可选的设备名，由前端传入
        std::string deviceName = ParseJsonString(req.body, "device_name");

        if (name.empty() || password.empty()) {
            res.status_code = 400;
            res.body = "{\"status\": \"fail\", \"message\": \"用户名或密码不能为空\"}";
            return res;
        }

        // 从请求头获取 User-Agent
        std::string userAgent;
        auto uaIt = req.headers.find("User-Agent");
        if (uaIt == req.headers.end()) {
            uaIt = req.headers.find("user-agent");
        }
        if (uaIt != req.headers.end()) {
            userAgent = uaIt->second;
        }

        // 从请求中获取客户端 IP
        std::string clientIp = req.client_ip;
        if (clientIp.empty()) clientIp = "unknown";

        if (deviceName.empty()) {
            // 如果没有传入设备名，用 User-Agent 的前 30 个字符作为默认设备名
            if (!userAgent.empty()) {
                deviceName = userAgent.substr(0, 30);
                // 去掉可能截断在中间的乱码
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

            // 不再删除旧 token — 允许多设备同时在线
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

            // 发送欢迎任务给用户线程（返回 taskId 但登录接口不需要等结果）
            PostUserTask(userId, UserTaskType::CUSTOM_EVENT,
                         "{\"event\":\"login\",\"device\":\"" + deviceName + "\"}", "登录系统");

            // 获取当前用户所有设备列表（用于返回）
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

    // ===== GET /api/userinfo（返回用户信息及当前设备信息） =====
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

    // ===== POST /api/task/result（查询异步任务结果） =====
    server.Post("/api/task/result", [](const Http::HttpRequest& req) -> Http::HttpResponse {
        Http::HttpResponse res;
        res.status_code = 200;
        res.status_text = "OK";
        res.headers["Content-Type"] = "application/json";
        res.headers["Access-Control-Allow-Origin"] = "*";

        // 从 body 中获取 task_id
        std::string taskId = ParseJsonString(req.body, "task_id");
        if (taskId.empty()) {
            res.body = "{\"status\":\"fail\",\"message\":\"请提供 task_id\"}";
            return res;
        }

        // 鉴权
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

        // 只能查自己的任务
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

    // ===== GET /api/devices（查看当前用户的所有登录设备） =====
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

    // ===== POST /api/logout（只登出当前设备） =====
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

                // 检查该用户是否还有其他设备在线
                bool otherDevicesOnline = false;
                for (const auto& [otherToken, otherSession] : g_tokenStore) {
                    if (otherToken != token && otherSession.userId == userId) {
                        otherDevicesOnline = true;
                        break;
                    }
                }

                // 只有所有设备都登出了，才销毁用户线程
                if (!otherDevicesOnline) {
                    std::lock_guard<std::mutex> threadLock(g_userThreadsMutex);
                    auto threadIt = g_userThreads.find(userId);
                    if (threadIt != g_userThreads.end()) {
                        // 发送关闭任务，让线程优雅退出
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
            "\"message\": \"设备 ";
        res.body += deviceName;
        res.body += "' 已退出登录\"}";
        return res;
    });

    // ===== POST /api/data =====
    // 用户提交数据 → 投递到用户专属线程异步处理
    server.Post("/api/data", [db](const Http::HttpRequest& req) -> Http::HttpResponse {
        Http::HttpResponse res;
        res.status_code = 200;
        res.status_text = "OK";
        res.headers["Content-Type"] = "application/json";
        res.headers["Access-Control-Allow-Origin"] = "*";

        // 鉴权
        std::string token = GetTokenFromRequest(req);
        SessionInfo* session = ValidateToken(token);
        if (!session) {
            res.status_code = 401;
            res.body = "{\"status\": \"fail\", \"message\": \"未登录\"}";
            return res;
        }

        // 将任务投递到该用户的专属线程去处理
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
    // 用户更新数据 → 投递到用户专属线程处理
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

    // ===== GET /api/contents（返回首页板块/标签/图片列表） =====
    server.Get("/api/contents", [db](const Http::HttpRequest& req) -> Http::HttpResponse {
        Http::HttpResponse res;
        res.status_code = 200;
        res.status_text = "OK";
        res.headers["Content-Type"] = "application/json";
        res.headers["Access-Control-Allow-Origin"] = "*";

        std::string json = "{";
        json += "\"code\": 200, ";
        json += "\"message\": \"success\", ";
        json += "\"data\": [";

        // 4 个板块：文章、图片、视频、博客
        std::vector<std::string> sectionTitles = {"文章", "图片", "视频", "博客"};
        std::vector<std::vector<std::pair<std::string, std::vector<std::string>>>> sectionData;

        // 每个板块 3 个标签，每个标签若干图片
        for (int s = 0; s < 4; s++) {
            std::vector<std::pair<std::string, std::vector<std::string>>> rows;
            for (int r = 0; r < 3; r++) {
                std::string label = "标签" + std::to_string(r + 1);
                std::vector<std::string> imgs;
                int imgCount = 6 - r; // 6, 5, 4
                for (int i = 0; i < imgCount; i++) {
                    // 图片路径：/image/{section_index}_{row_index}_{img_index}.jpg
                    imgs.push_back("/image/" + std::to_string(s) + "_" + std::to_string(r) + "_" + std::to_string(i) + ".jpg");
                }
                rows.push_back({label, imgs});
            }
            sectionData.push_back(rows);
        }

        for (int s = 0; s < (int)sectionData.size(); s++) {
            if (s > 0) json += ",";
            json += "{";
            json += "\"title\":\"" + sectionTitles[s] + "\",";
            json += "\"rows\":[";
            for (int r = 0; r < (int)sectionData[s].size(); r++) {
                if (r > 0) json += ",";
                json += "{";
                json += "\"label\":\"" + sectionData[s][r].first + "\",";
                json += "\"imgs\":[";
                for (int i = 0; i < (int)sectionData[s][r].second.size(); i++) {
                    if (i > 0) json += ",";
                    json += "\"" + sectionData[s][r].second[i] + "\"";
                }
                json += "]";
                json += "}";
            }
            json += "]";
            json += "}";
        }

        json += "]}";
        res.body = json;
        return res;
    });

    // ===== GET /api/article（返回文章数据） =====
    server.Get("/api/article", [](const Http::HttpRequest& req) -> Http::HttpResponse {
        Http::HttpResponse res;
        res.status_code = 200;
        res.status_text = "OK";
        res.headers["Content-Type"] = "application/json";
        res.headers["Access-Control-Allow-Origin"] = "*";

        std::string json = "{";
        json += "\"code\": 200, ";
        json += "\"message\": \"success\", ";
        json += "\"data\": {";
        json += "\"title\": \"文章名\",";
        json += "\"chapters\": [";
        json += "{\"name\":\"第一章 开篇\",\"content\":\"清晨的微光透过窗棂洒落，轻轻拂过桌面，为平凡的一日拉开序幕。生活总在日复一日的前行中书写着不同的故事，有人奔赴远方追寻理想，有人留守身旁守护温暖，每一种选择都有着独属于自己的意义。\"},";
        json += "{\"name\":\"第二章 内容一\",\"content\":\"行走在世间，我们会遇见形形色色的人，经历大大小小的事，有欢声笑语相伴，也难免有失意迷茫相随。就像四季更迭，春有百花秋有月，夏有凉风冬有雪，不同的光景拼凑出完整的人生画卷。\"},";
        json += "{\"name\":\"第三章 内容二\",\"content\":\"我们在成长中学会接纳，在挫折中变得坚强，在陪伴中懂得珍惜。每一次迈步，都是对未来的期许；每一次停留，都是对当下的感悟。不必焦虑前路漫漫，也不必纠结过往遗憾，用心感受眼前的点滴美好，认真对待每一分每一秒，便是对生活最好的回应。\"},";
        json += "{\"name\":\"第四章 结尾\",\"content\":\"时光缓缓流淌，带走稚嫩，沉淀阅历，让原本懵懂的内心慢慢变得丰盈而笃定。无论是喧嚣的闹市，还是静谧的郊野，只要心怀热爱，处处皆是风景。待人以真诚，处事以坦然，在烟火日常里守住本心，在风雨来袭时挺直脊梁。人生本就是一场不断探索、不断修行的旅程，沿途的坎坷与惊喜，都是命运赠予的礼物。学会和自己和解，和世界温柔相处，不慌不忙，慢慢前行，终会在属于自己的天地里，收获独有的光芒。\"}";
        json += "]";
        json += "}}";
        res.body = json;
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