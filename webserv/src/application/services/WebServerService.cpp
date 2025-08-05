#include "WebServerService.hpp"
#include <iostream>
#include <algorithm>

WebServerService::WebServerService(IConfigurationLoader* configLoader, 
                                 INetworkAdapter* networkAdapter,
                                 ProcessHttpRequestUseCase* requestProcessor)
    : _configLoader(configLoader), _networkAdapter(networkAdapter), 
      _requestProcessor(requestProcessor), _isRunning(false) {}

WebServerService::~WebServerService() {
    stop();
    cleanup();
}

bool WebServerService::initialize(const std::string& configPath) {
    try {
        // Carregar configurações
        _serverConfigs = _configLoader->loadConfiguration(configPath);
        
        if (_serverConfigs.empty()) {
            std::cerr << "No valid server configurations found" << std::endl;
            return false;
        }
        
        // Validar configurações
        for (const auto& config : _serverConfigs) {
            if (!config.isValid()) {
                std::cerr << "Invalid server configuration found" << std::endl;
                return false;
            }
        }
        
        // Inicializar sockets dos servidores
        if (!initializeServerSockets()) {
            std::cerr << "Failed to initialize server sockets" << std::endl;
            return false;
        }
        
        std::cout << "WebServer initialized with " << _serverConfigs.size() << " server(s)" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error initializing server: " << e.what() << std::endl;
        return false;
    }
}

void WebServerService::run() {
    if (_serverSockets.empty()) {
        std::cerr << "No server sockets available. Call initialize() first." << std::endl;
        return;
    }
    
    _isRunning = true;
    std::cout << "WebServer started. Press Ctrl+C to stop." << std::endl;
    
    while (_isRunning) {
        processConnections();
    }
    
    std::cout << "WebServer stopped." << std::endl;
}

void WebServerService::stop() {
    _isRunning = false;
}

bool WebServerService::isRunning() const {
    return _isRunning;
}

bool WebServerService::initializeServerSockets() {
    _serverSockets.clear();
    
    for (const auto& config : _serverConfigs) {
        int serverSocket = _networkAdapter->initializeServer(config.getPort(), config.getHost());
        
        if (serverSocket == -1) {
            std::cerr << "Failed to initialize server on " << config.getHost() 
                     << ":" << config.getPort() << std::endl;
            // Limpar sockets já criados
            for (int socket : _serverSockets) {
                _networkAdapter->closeSocket(socket);
            }
            _serverSockets.clear();
            return false;
        }
        
        _serverSockets.push_back(serverSocket);
        std::cout << "Server listening on " << config.getHost() 
                 << ":" << config.getPort() << std::endl;
    }
    
    return true;
}

void WebServerService::processConnections() {
    // Timeout de 1 segundo
    int ready = _networkAdapter->pollSockets(_serverSockets, 1000);
    
    if (ready > 0) {
        for (int serverSocket : _serverSockets) {
            handleNewConnection(serverSocket);
        }
    }
}

void WebServerService::handleNewConnection(int serverSocket) {
    int clientSocket = _networkAdapter->acceptConnection(serverSocket);
    
    if (clientSocket != -1) {
        handleClientData(clientSocket);
        _networkAdapter->closeSocket(clientSocket);
    }
}

void WebServerService::handleClientData(int clientSocket) {
    const size_t bufferSize = 4096;
    char buffer[bufferSize];
    std::string requestData;
    
    // Ler dados do cliente
    ssize_t bytesRead = _networkAdapter->readFromSocket(clientSocket, buffer, bufferSize - 1);
    
    if (bytesRead > 0) {
        buffer[bytesRead] = '\0';
        requestData = std::string(buffer);
        
        // Encontrar a configuração do servidor apropriada
        // Para simplificar, usar a primeira configuração por agora
        const ServerConfig* serverConfig = &_serverConfigs[0];
        
        // Processar a requisição
        // Primeiro, precisamos parseá-la (isso seria feito por um parser HTTP)
        // Por simplicidade, vamos criar uma requisição básica
        HttpRequest request;
        // Aqui deveria haver parsing real do HTTP
        
        HttpResponse response = _requestProcessor->processRequest(request, *serverConfig);
        
        // Enviar resposta
        std::string responseStr = response.toString();
        _networkAdapter->writeToSocket(clientSocket, responseStr);
    }
}

const ServerConfig* WebServerService::findServerConfig(int serverSocket) {
    // Encontrar a configuração correspondente ao socket
    auto it = std::find(_serverSockets.begin(), _serverSockets.end(), serverSocket);
    if (it != _serverSockets.end()) {
        size_t index = std::distance(_serverSockets.begin(), it);
        if (index < _serverConfigs.size()) {
            return &_serverConfigs[index];
        }
    }
    return nullptr;
}

void WebServerService::cleanup() {
    for (int socket : _serverSockets) {
        _networkAdapter->closeSocket(socket);
    }
    _serverSockets.clear();
}
