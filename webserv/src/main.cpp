#include "src/application/services/WebServerService.hpp"
#include "src/application/usecases/ProcessHttpRequestUseCase.hpp"
#include "src/infrastructure/adapters/FileConfigurationLoader.hpp"
#include "src/infrastructure/adapters/BSDSocketNetworkAdapter.hpp"
#include "src/infrastructure/adapters/StandardFileSystemAdapter.hpp"
#include "src/infrastructure/adapters/HttpRequestParser.hpp"

#include <iostream>
#include <memory>
#include <csignal>

// Variável global para controle de shutdown
static WebServerService* g_serverService = nullptr;

void signalHandler(int signum) {
    if (signum == SIGINT && g_serverService) {
        std::cout << "\nReceived SIGINT, shutting down server..." << std::endl;
        g_serverService->stop();
    }
}

int main(int argc, char* argv[]) {
    try {
        // Verificar argumentos
        if (argc != 2) {
            std::cerr << "Usage: " << argv[0] << " <config_file>" << std::endl;
            return 1;
        }

        std::string configPath = argv[1];

        // Criar adaptadores (camada de infraestrutura)
        std::unique_ptr<IConfigurationLoader> configLoader = 
            std::make_unique<FileConfigurationLoader>();
        
        std::unique_ptr<INetworkAdapter> networkAdapter = 
            std::make_unique<BSDSocketNetworkAdapter>();
        
        std::unique_ptr<IFileSystemAdapter> fileSystemAdapter = 
            std::make_unique<StandardFileSystemAdapter>();
        
        std::unique_ptr<IHttpRequestParser> requestParser = 
            std::make_unique<HttpRequestParser>();

        // Criar use cases (camada de aplicação)
        std::unique_ptr<ProcessHttpRequestUseCase> requestProcessor = 
            std::make_unique<ProcessHttpRequestUseCase>(fileSystemAdapter.get());

        // Criar serviço principal (camada de aplicação)
        std::unique_ptr<WebServerService> serverService = 
            std::make_unique<WebServerService>(
                configLoader.get(),
                networkAdapter.get(),
                requestProcessor.get()
            );

        // Configurar handler de sinal
        g_serverService = serverService.get();
        std::signal(SIGINT, signalHandler);
        std::signal(SIGPIPE, SIG_IGN); // Ignorar SIGPIPE

        // Inicializar servidor
        if (!serverService->initialize(configPath)) {
            std::cerr << "Failed to initialize server" << std::endl;
            return 1;
        }

        // Executar servidor
        serverService->run();

        std::cout << "Server shutdown complete." << std::endl;
        return 0;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred" << std::endl;
        return 1;
    }
}
