#pragma once

#include "../../domain/entities/ServerConfig.hpp"
#include "../../domain/ports/IConfigurationLoader.hpp"
#include "../../domain/ports/INetworkAdapter.hpp"
#include "../usecases/ProcessHttpRequestUseCase.hpp"
#include <vector>
#include <memory>

/**
 * Serviço principal do WebServer
 * Orquestra a inicialização e operação do servidor
 */
class WebServerService {
private:
    std::vector<ServerConfig> _serverConfigs;
    IConfigurationLoader* _configLoader;
    INetworkAdapter* _networkAdapter;
    ProcessHttpRequestUseCase* _requestProcessor;
    std::vector<int> _serverSockets;
    bool _isRunning;

public:
    WebServerService(IConfigurationLoader* configLoader, 
                    INetworkAdapter* networkAdapter,
                    ProcessHttpRequestUseCase* requestProcessor);
    ~WebServerService();

    /**
     * Inicializa o servidor com as configurações do arquivo
     * @param configPath - caminho para o arquivo de configuração
     * @return true se sucesso, false caso contrário
     */
    bool initialize(const std::string& configPath);

    /**
     * Inicia o loop principal do servidor
     */
    void run();

    /**
     * Para o servidor
     */
    void stop();

    /**
     * Verifica se o servidor está executando
     * @return true se executando, false caso contrário
     */
    bool isRunning() const;

private:
    /**
     * Inicializa os sockets dos servidores
     * @return true se sucesso, false caso contrário
     */
    bool initializeServerSockets();

    /**
     * Processa conexões de entrada
     */
    void processConnections();

    /**
     * Lida com uma nova conexão
     * @param serverSocket - socket do servidor que recebeu a conexão
     */
    void handleNewConnection(int serverSocket);

    /**
     * Processa dados de um cliente conectado
     * @param clientSocket - socket do cliente
     */
    void handleClientData(int clientSocket);

    /**
     * Encontra a configuração do servidor para um socket específico
     * @param serverSocket - socket do servidor
     * @return ponteiro para a configuração ou nullptr
     */
    const ServerConfig* findServerConfig(int serverSocket);

    /**
     * Limpa recursos
     */
    void cleanup();
};
