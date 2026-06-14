// ============================================================
// FrameWork.h — 核心框架头文件
// 功能：全局变量声明、结构体定义、函数声明
// ============================================================

#ifndef FARMEWORK_H
#define FARMEWORK_H

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
#include <condition_variable>
#include <mutex>
#include <memory>
#include <ctime>

#ifdef _WIN32
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
#include "Token.h"
#include "Json.h"

// ===== extern 全局变量声明（定义在 FrameWork.cpp） =====
extern Http::http_server* g_server;
extern std::atomic<bool> g_running;

// ===== 任务系统枚举 =====
enum class TaskStatus {
    PENDING,
    PROCESSING,
    COMPLETED,
    FAILED
};

enum class UserTaskType {
    PING,
    PROCESS_DATA,
    SEND_NOTIFICATION,
    SYNC_DATABASE,
    CUSTOM_EVENT,
    SHUTDOWN
};

// ===== 任务定义 =====
struct UserTask {
    std::string taskId;
    UserTaskType type;
    std::time_t createTime;
    std::string source;
    std::string input;
    TaskStatus status;
    std::string output;
    std::string errorMessage;
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

// ===== 全局容器（extern 声明，定义在 FrameWork.cpp） =====
extern std::map<int, std::unique_ptr<UserThreadInfo>> g_userThreads;
extern std::mutex g_userThreadsMutex;

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

extern std::unordered_map<std::string, TaskResult> g_taskResults;
extern std::mutex g_taskResultsMutex;

// ===== 函数声明 =====
std::string GenerateTaskId();
std::string PostUserTask(int userId, UserTaskType, const std::string&, const std::string& source = "");
void UpdateTaskResult(const std::string& taskId, int userId, TaskStatus status, const std::string& output, const std::string& errorMessage = "");
TaskResult* GetTaskResult(const std::string& taskId);
void UserWorkerRoutine(UserThreadInfo* info);
void CreateUserThread(int userId, const std::string& name, int level);
void HttpServerRoutine(int Port);
void Initiave_Http(int Port);
MySQL::mysql* Initiave_MySQL();

#endif //!FAEMRWORK_H