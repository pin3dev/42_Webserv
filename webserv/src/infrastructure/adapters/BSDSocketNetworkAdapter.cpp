#include "BSDSocketNetworkAdapter.hpp"
#include <iostream>
#include <cstring>
#include <errno.h>

BSDSocketNetworkAdapter::BSDSocketNetworkAdapter() {}

BSDSocketNetworkAdapter::~BSDSocketNetworkAdapter() {
    // Fechar todos os sockets abertos
    for (const auto& pollfd : _pollFds) {
        if (pollfd.fd >= 0) {
            close(pollfd.fd);
        }
    }
}

int BSDSocketNetworkAdapter::initializeServer(const std::string& port, const std::string& host) {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Error creating socket: " << strerror(errno) << std::endl;
        return -1;
    }

    // Configurar opções do socket
    if (!setSocketOptions(serverSocket)) {
        close(serverSocket);
        return -1;
    }

    // Configurar endereço do servidor
    struct sockaddr_in serverAddr;
    std::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(parsePort(port));
    serverAddr.sin_addr.s_addr = parseHost(host);

    // Bind
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "Error binding socket: " << strerror(errno) << std::endl;
        close(serverSocket);
        return -1;
    }

    // Listen
    if (listen(serverSocket, 128) == -1) {
        std::cerr << "Error listening on socket: " << strerror(errno) << std::endl;
        close(serverSocket);
        return -1;
    }

    // Configurar como não-bloqueante
    if (!setNonBlocking(serverSocket)) {
        close(serverSocket);
        return -1;
    }

    return serverSocket;
}

int BSDSocketNetworkAdapter::acceptConnection(int serverSocket) {
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    
    int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
    
    if (clientSocket == -1) {
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            std::cerr << "Error accepting connection: " << strerror(errno) << std::endl;
        }
        return -1;
    }

    // Configurar cliente como não-bloqueante
    if (!setNonBlocking(clientSocket)) {
        close(clientSocket);
        return -1;
    }

    return clientSocket;
}

ssize_t BSDSocketNetworkAdapter::readFromSocket(int socket, char* buffer, size_t size) {
    ssize_t bytesRead = recv(socket, buffer, size, 0);
    
    if (bytesRead == -1) {
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            std::cerr << "Error reading from socket: " << strerror(errno) << std::endl;
        }
        return -1;
    }
    
    return bytesRead;
}

ssize_t BSDSocketNetworkAdapter::writeToSocket(int socket, const std::string& data) {
    ssize_t bytesWritten = send(socket, data.c_str(), data.length(), 0);
    
    if (bytesWritten == -1) {
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            std::cerr << "Error writing to socket: " << strerror(errno) << std::endl;
        }
        return -1;
    }
    
    return bytesWritten;
}

void BSDSocketNetworkAdapter::closeSocket(int socket) {
    if (socket >= 0) {
        close(socket);
    }
}

int BSDSocketNetworkAdapter::pollSockets(std::vector<int>& sockets, int timeout) {
    _pollFds.clear();
    
    // Preparar estruturas pollfd
    for (int socket : sockets) {
        pollfd pfd;
        pfd.fd = socket;
        pfd.events = POLLIN;
        pfd.revents = 0;
        _pollFds.push_back(pfd);
    }
    
    int ready = poll(_pollFds.data(), _pollFds.size(), timeout);
    
    if (ready == -1) {
        std::cerr << "Error in poll: " << strerror(errno) << std::endl;
        return -1;
    }
    
    // Atualizar vetor de sockets com apenas os que estão prontos
    sockets.clear();
    for (const auto& pfd : _pollFds) {
        if (pfd.revents & POLLIN) {
            sockets.push_back(pfd.fd);
        }
    }
    
    return ready;
}

bool BSDSocketNetworkAdapter::setNonBlocking(int socket) {
    int flags = fcntl(socket, F_GETFL, 0);
    if (flags == -1) {
        std::cerr << "Error getting socket flags: " << strerror(errno) << std::endl;
        return false;
    }
    
    if (fcntl(socket, F_SETFL, flags | O_NONBLOCK) == -1) {
        std::cerr << "Error setting socket non-blocking: " << strerror(errno) << std::endl;
        return false;
    }
    
    return true;
}

bool BSDSocketNetworkAdapter::setSocketOptions(int socket) {
    int opt = 1;
    
    // SO_REUSEADDR para permitir reutilização do endereço
    if (setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        std::cerr << "Error setting SO_REUSEADDR: " << strerror(errno) << std::endl;
        return false;
    }
    
    return true;
}

int BSDSocketNetworkAdapter::parsePort(const std::string& portStr) {
    try {
        int port = std::stoi(portStr);
        if (port < 1 || port > 65535) {
            std::cerr << "Invalid port number: " << port << std::endl;
            return -1;
        }
        return port;
    } catch (const std::exception& e) {
        std::cerr << "Error parsing port: " << e.what() << std::endl;
        return -1;
    }
}

in_addr_t BSDSocketNetworkAdapter::parseHost(const std::string& hostStr) {
    if (hostStr == "localhost" || hostStr == "127.0.0.1") {
        return htonl(INADDR_LOOPBACK);
    } else if (hostStr == "0.0.0.0" || hostStr.empty()) {
        return htonl(INADDR_ANY);
    } else {
        // Tentar converter como endereço IP
        in_addr_t addr = inet_addr(hostStr.c_str());
        if (addr == INADDR_NONE) {
            std::cerr << "Invalid host address: " << hostStr << std::endl;
            return htonl(INADDR_ANY);
        }
        return addr;
    }
}
