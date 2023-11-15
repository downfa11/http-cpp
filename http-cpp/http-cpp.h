#pragma once
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <map>
#include <sstream>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

class httpcpp {

public:
    std::string web_address;

    SOCKET serverSocket;
    SOCKET clientSocket;

    httpcpp(std::string address = "127.0.0.1:80") {
        web_address = address;
        Init();
    }

    void Init();

    int SendGET(std::string path, std::string request, size_t length);
    int SendPOST(std::string path, std::string request, size_t length);

    typedef std::string(*RequestHandler)(const std::string& params);
    void RegisterHandler(const std::string& path, const std::string& httpMethod, RequestHandler handler);

    std::map<std::pair<std::string, std::string>, RequestHandler> handlerMap;
    std::string BuildHttpResponse(const std::string& body);

private:

    SOCKET WebServerConnect(std::string address);
    void HandleClient(SOCKET clientSocket);
};
