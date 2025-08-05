#pragma once

#include "../../domain/entities/HttpRequest.hpp"
#include "../../domain/entities/HttpResponse.hpp"
#include "../../domain/entities/ServerConfig.hpp"
#include "../../domain/ports/IFileSystemAdapter.hpp"

/**
 * Use Case para processar requisições HTTP
 * Contém a lógica de negócio para processar diferentes tipos de requisições
 */
class ProcessHttpRequestUseCase {
private:
    IFileSystemAdapter* _fileSystemAdapter;

public:
    ProcessHttpRequestUseCase(IFileSystemAdapter* fileSystemAdapter);
    ~ProcessHttpRequestUseCase();

    /**
     * Processa uma requisição HTTP e gera uma resposta
     * @param request - requisição HTTP recebida
     * @param serverConfig - configuração do servidor
     * @return resposta HTTP apropriada
     */
    HttpResponse processRequest(const HttpRequest& request, const ServerConfig& serverConfig);

private:
    /**
     * Processa requisições GET
     * @param request - requisição HTTP
     * @param serverConfig - configuração do servidor
     * @param location - configuração da localização
     * @return resposta HTTP
     */
    HttpResponse processGetRequest(const HttpRequest& request, 
                                 const ServerConfig& serverConfig, 
                                 const LocationConfig* location);

    /**
     * Processa requisições POST
     * @param request - requisição HTTP
     * @param serverConfig - configuração do servidor
     * @param location - configuração da localização
     * @return resposta HTTP
     */
    HttpResponse processPostRequest(const HttpRequest& request, 
                                  const ServerConfig& serverConfig, 
                                  const LocationConfig* location);

    /**
     * Processa requisições DELETE
     * @param request - requisição HTTP
     * @param serverConfig - configuração do servidor
     * @param location - configuração da localização
     * @return resposta HTTP
     */
    HttpResponse processDeleteRequest(const HttpRequest& request, 
                                    const ServerConfig& serverConfig, 
                                    const LocationConfig* location);

    /**
     * Serve um arquivo estático
     * @param filePath - caminho para o arquivo
     * @return resposta HTTP com o conteúdo do arquivo
     */
    HttpResponse serveStaticFile(const std::string& filePath);

    /**
     * Gera uma resposta de erro
     * @param statusCode - código de status HTTP
     * @param serverConfig - configuração do servidor para página de erro customizada
     * @return resposta HTTP de erro
     */
    HttpResponse generateErrorResponse(HttpStatusCode statusCode, const ServerConfig& serverConfig);

    /**
     * Resolve o caminho completo do arquivo
     * @param uri - URI da requisição
     * @param serverConfig - configuração do servidor
     * @return caminho completo do arquivo
     */
    std::string resolveFilePath(const std::string& uri, const ServerConfig& serverConfig);

    /**
     * Gera uma página de erro padrão
     * @param statusCode - código de status HTTP
     * @return HTML da página de erro
     */
    std::string generateDefaultErrorPage(HttpStatusCode statusCode);
};
