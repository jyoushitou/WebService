#ifndef HTTPS_API_H
#define HTTPS_API_H

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <windows.h>
    #pragma comment(lib, "ws2_32.lib")
    #define close closesocket
    inline int read(SOCKET fd, char* buf, int count) {
        return recv(fd, buf, count, 0);
    }
    inline int write(SOCKET fd, const char* buf, int count) {
        return send(fd, buf, count, 0);
    }
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <unistd.h>
    #include <arpa/inet.h>
    using SOCKET = int;
#endif

#include <fstream>
#include <sstream>
#include <string>
#include <thread>
#include <functional>
#include <unordered_map>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <algorithm>
#include <regex>
#include <filesystem>
#include <iostream>
#include <exception>

#include "Tools.h"

namespace fs = std::filesystem;

namespace Http {
    struct HttpRequest {
        std::string method;
        std::string path;
        std::string version;
        std::unordered_map<std::string, std::string> headers;
        std::string body;
        std::string client_ip;  // 新增：客户端 IP 地址
    };
    
    struct HttpResponse {
        int status_code;
        std::string status_text;
        std::unordered_map<std::string, std::string> headers;
        std::string body;
    };

    class http_server {
        private:
        int m_port;
        std::string m_static_dir;
        int m_max_connections;
        SOCKET m_server_socket;
        bool m_running;
        
        
        // 路由表
        std::unordered_map<std::string, std::function<HttpResponse(const HttpRequest&)>> m_get_routes;
        std::unordered_map<std::string, std::function<HttpResponse(const HttpRequest&)>> m_post_routes;
        std::unordered_map<std::string, std::function<HttpResponse(const HttpRequest&)>> m_put_routes;
        std::unordered_map<std::string, std::function<HttpResponse(const HttpRequest&)>> m_delete_routes;

        std::string Get_Static_Dir();
        HttpRequest Request_Parse(const std::string& raw, const std::string& client_ip = "");
        std::string Response_Build(const HttpResponse& res);
        HttpResponse Default_Request_Handle(const HttpRequest& req);
        void Run_Accept_Loop();
        static void Handle_Client(SOCKET clientSocket, http_server* server);

        // JSON 工具函数
        std::string Get_Json_Value(const std::string& json, const std::string& key);
        std::string Build_Json_Response(int code, const std::string& message, const std::string& data);

        public:
        http_server(int port = 8080, int maxConnections = 10);
        ~http_server();
        
        void Set_Static_Dir(const std::string& dir);
        
        // 路由注册方法
        void Get(const std::string& path, std::function<HttpResponse(const HttpRequest&)> handler);
        void Post(const std::string& path, std::function<HttpResponse(const HttpRequest&)> handler);
        void Put(const std::string& path, std::function<HttpResponse(const HttpRequest&)> handler);
        void Delete(const std::string& path, std::function<HttpResponse(const HttpRequest&)> handler);
        
        HttpResponse Handle_Hello(const HttpRequest& req);
        HttpResponse Handle_Data(const HttpRequest& req);
        HttpResponse Handle_Update(const HttpRequest& req);
        HttpResponse Handle_Delete(const HttpRequest& req);

        bool Initialize();
        void Start(bool async = false);
        void Stop();
        
        bool Is_Running() const { return m_running; }
        int Get_Port() const { return m_port; }
        
        
    };
}

#endif // HTTPS_API_H