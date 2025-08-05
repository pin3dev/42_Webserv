#pragma once

#include "../entities/HttpResponse.hpp"
#include <vector>

/**
 * Port para operações de rede
 * Define o contrato para comunicação de rede (sockets, polling, etc.)
 */
class INetworkAdapter {
public:
    virtual ~INetworkAdapter() = default;
    
    /**
     * Inicializa o servidor na porta especificada
     * @param port - porta para escuta
     * @param host - host para bind
     * @return socket descriptor ou -1 em caso de erro
     */
    virtual int initializeServer(const std::string& port, const std::string& host) = 0;
    
    /**
     * Aceita uma nova conexão
     * @param serverSocket - socket do servidor
     * @return socket descriptor da conexão ou -1 em caso de erro
     */
    virtual int acceptConnection(int serverSocket) = 0;
    
    /**
     * Lê dados de um socket
     * @param socket - socket para leitura
     * @param buffer - buffer para armazenar os dados
     * @param size - tamanho máximo a ler
     * @return número de bytes lidos ou -1 em caso de erro
     */
    virtual ssize_t readFromSocket(int socket, char* buffer, size_t size) = 0;
    
    /**
     * Escreve dados para um socket
     * @param socket - socket para escrita
     * @param data - dados para escrever
     * @return número de bytes escritos ou -1 em caso de erro
     */
    virtual ssize_t writeToSocket(int socket, const std::string& data) = 0;
    
    /**
     * Fecha um socket
     * @param socket - socket para fechar
     */
    virtual void closeSocket(int socket) = 0;
    
    /**
     * Executa polling nos sockets
     * @param sockets - lista de sockets para monitorar
     * @param timeout - timeout em milissegundos
     * @return número de sockets prontos ou -1 em caso de erro
     */
    virtual int pollSockets(std::vector<int>& sockets, int timeout) = 0;
};
