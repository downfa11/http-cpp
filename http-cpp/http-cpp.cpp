// http-cpp.cpp : 정적 라이브러리를 위한 함수를 정의합니다.
//

#include "pch.h"
#include "framework.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "http-cpp.h"


SOCKET httpcpp::WebServerConnect(std::string address) {

    size_t colonPos = address.find(":");
    if (colonPos == std::string::npos) {
        std::cerr << "Invalid address format." << std::endl;
        return INVALID_SOCKET;
    }

    std::string ip = address.substr(0, colonPos);
    std::string portStr = address.substr(colonPos + 1);

    int port;
    try {
        port = std::stoi(portStr);
    }
    catch (const std::exception& e) {
        std::cerr << "Invalid port number." << std::endl;
        return INVALID_SOCKET;
    }

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create server socket." << std::endl;
        return NULL;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(ip.c_str());
    serverAddr.sin_port = htons(port);

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Failed to bind server socket." << std::endl;
        closesocket(serverSocket);
        return NULL;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Failed to listen on server socket." << std::endl;
        closesocket(serverSocket);
        return NULL;
    }

    return serverSocket;
}

void httpcpp::Init() {
    serverSocket = WebServerConnect(web_address);

    if (serverSocket == INVALID_SOCKET)
        return;

    sockaddr_in clientAddr;
    int clientAddrSize = sizeof(clientAddr);
    clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientAddrSize);

    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Failed to accept client connection." << std::endl;
        closesocket(serverSocket);
        return;
    }

    HandleClient(clientSocket);
}

void httpcpp::RegisterHandler(const std::string& path, const std::string& httpMethod, RequestHandler handler) {
    handlerMap[{path, httpMethod}] = handler;
}

std::string httpcpp::BuildHttpResponse(const std::string& body) {
    std::ostringstream response;
    response << "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n" << body;
    return response.str();
}

void httpcpp::HandleClient(SOCKET clientSocket) {
    char buffer[1024];
    int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

    if (bytesRead <= 0) {
        std::cerr << "Failed to read client request." << std::endl;
        closesocket(clientSocket);
        return;
    }

    else {
        buffer[bytesRead] = '\0';
        std::string request(buffer);

        std::string httpMethod;
        if (request.find("GET ") == 0)
            httpMethod = "GET";

        else if (request.find("POST ") == 0)
            httpMethod = "POST";
        else {
            std::cerr << "Unknown HTTP method" << std::endl;
            closesocket(clientSocket);
            return;
        }

        size_t pathStart = 0;
        if (httpMethod == "GET")
            pathStart = request.find("GET ") + 4;
        else if (httpMethod == "POST")
            pathStart = request.find("POST ") + 5;

        size_t pathEnd = request.find(" HTTP/1.1");
        std::string path = request.substr(pathStart, pathEnd - pathStart);

        size_t paramsStart = path.find("?");
        std::string params = "";
        if (paramsStart != std::string::npos) {
            params = path.substr(paramsStart + 1);
            path = path.substr(0, paramsStart);
        }

        std::string response;

        auto handler = handlerMap.find({ path, httpMethod });
        if (handler != handlerMap.end()) {
            response = BuildHttpResponse(handler->second(params));
        }
        else {
            response = BuildHttpResponse("Not Found");
        }

        send(clientSocket, response.c_str(), response.size(), 0);
    }
    closesocket(clientSocket);
}

int httpcpp::SendPOST(std::string path, std::string request, size_t length) {

    std::string postRequest = "POST " + path +
        " HTTP/1.1\r\nHost: " + web_address + "\r\nContent-Type: application/json\r\nContent-Length: "
        + std::to_string(length) + "\r\n\r\n" + request;

    if (send(clientSocket, postRequest.c_str(), postRequest.length(), 0) == SOCKET_ERROR)
        return -1;
    else return 0;

    return -1;
}

int httpcpp::SendGET(std::string path, std::string request, size_t length) {

    std::string getRequest = "GET " + path +
        " HTTP/1.1\r\nHost: " + web_address + "\r\nContent-Type: application/json\r\nContent-Length: "
        + std::to_string(length) + "\r\n\r\n" + request;

    if (send(clientSocket, getRequest.c_str(), getRequest.length(), 0) == SOCKET_ERROR)
        return -1;

    else return 0;

    return -1;
}
