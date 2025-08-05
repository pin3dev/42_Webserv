#pragma once

#include "../../domain/ports/INetworkAdapter.hpp"
#include <vector>
#include <poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

/**
 * Adaptador concreto para operações de rede
 * Implementa a interface INetworkAdapter usando sockets BSD
 */
class BSDSocketNetworkAdapter : public INetworkAdapter {
private:
    std::vector<pollfd> _pollFds;

public:
    BSDSocketNetworkAdapter();
    ~BSDSocketNetworkAdapter();

    // Implementação da interface INetworkAdapter
    int initializeServer(const std::string& port, const std::string& host) override;
    int acceptConnection(int serverSocket) override;
    ssize_t readFromSocket(int socket, char* buffer, size_t size) override;
    ssize_t writeToSocket(int socket, const std::string& data) override;
    void closeSocket(int socket) override;
    int pollSockets(std::vector<int>& sockets, int timeout) override;

private:
    /**
     * Configura um socket como não-bloqueante
     * @param socket - socket para configurar
     * @return true se sucesso, false caso contrário
     */
    bool setNonBlocking(int socket);

    /**
     * Configura opções do socket (SO_REUSEADDR, etc.)
     * @param socket - socket para configurar
     * @return true se sucesso, false caso contrário
     */
    bool setSocketOptions(int socket);

    /**
     * Converte string de porta para número
     * @param portStr - string da porta
     * @return número da porta ou -1 se inválida
     */
    int parsePort(const std::string& portStr);

    /**
     * Converte string de host para endereço IP
     * @param hostStr - string do host
     * @return endereço IP em formato in_addr_t
     */
    in_addr_t parseHost(const std::string& hostStr);
};
