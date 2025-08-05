#include "ProcessHttpRequestUseCase.hpp"
#include <sstream>

ProcessHttpRequestUseCase::ProcessHttpRequestUseCase(IFileSystemAdapter* fileSystemAdapter)
    : _fileSystemAdapter(fileSystemAdapter) {}

ProcessHttpRequestUseCase::~ProcessHttpRequestUseCase() {}

HttpResponse ProcessHttpRequestUseCase::processRequest(const HttpRequest& request, const ServerConfig& serverConfig) {
    // Validar a requisição
    if (!request.isValid()) {
        return generateErrorResponse(HttpStatusCode::BAD_REQUEST, serverConfig);
    }

    // Encontrar a configuração de localização correspondente
    const LocationConfig* location = serverConfig.findMatchingLocation(request.getUri());
    
    // Verificar se o método é permitido
    if (location && !location->isMethodAllowed(request.getMethod())) {
        return generateErrorResponse(HttpStatusCode::METHOD_NOT_ALLOWED, serverConfig);
    }

    // Processar baseado no método HTTP
    if (request.getMethod() == "GET") {
        return processGetRequest(request, serverConfig, location);
    } else if (request.getMethod() == "POST") {
        return processPostRequest(request, serverConfig, location);
    } else if (request.getMethod() == "DELETE") {
        return processDeleteRequest(request, serverConfig, location);
    } else {
        return generateErrorResponse(HttpStatusCode::METHOD_NOT_ALLOWED, serverConfig);
    }
}

HttpResponse ProcessHttpRequestUseCase::processGetRequest(const HttpRequest& request, 
                                                        const ServerConfig& serverConfig, 
                                                        const LocationConfig* /* location */) {
    std::string filePath = resolveFilePath(request.getUri(), serverConfig);
    
    // Se o URI termina com '/', adicionar o arquivo index
    if (request.getUri().back() == '/') {
        filePath += serverConfig.getIndexPage();
    }
    
    // Verificar se o arquivo existe
    if (!_fileSystemAdapter->fileExists(filePath)) {
        return generateErrorResponse(HttpStatusCode::NOT_FOUND, serverConfig);
    }
    
    return serveStaticFile(filePath);
}

HttpResponse ProcessHttpRequestUseCase::processPostRequest(const HttpRequest& request, 
                                                         const ServerConfig& serverConfig, 
                                                         const LocationConfig* location) {
    // Verificar tamanho máximo do corpo
    if (request.getBody().size() > serverConfig.getClientMaxBodySize()) {
        return generateErrorResponse(HttpStatusCode::BAD_REQUEST, serverConfig);
    }
    
    // Se há configuração de upload
    if (location && !location->uploadPath.empty()) {
        // Implementar lógica de upload
        std::string uploadPath = serverConfig.getRoot() + location->uploadPath + "/uploaded_file";
        
        if (_fileSystemAdapter->writeFile(uploadPath, request.getBody())) {
            HttpResponse response(HttpStatusCode::OK);
            response.setBody("File uploaded successfully");
            response.setContentLength();
            return response;
        } else {
            return generateErrorResponse(HttpStatusCode::INTERNAL_SERVER_ERROR, serverConfig);
        }
    }
    
    // Resposta padrão para POST sem upload
    HttpResponse response(HttpStatusCode::OK);
    response.setBody("POST request processed");
    response.setContentLength();
    return response;
}

HttpResponse ProcessHttpRequestUseCase::processDeleteRequest(const HttpRequest& request, 
                                                           const ServerConfig& serverConfig, 
                                                           const LocationConfig* /* location */) {
    std::string filePath = resolveFilePath(request.getUri(), serverConfig);
    
    // Verificar se o arquivo existe
    if (!_fileSystemAdapter->fileExists(filePath)) {
        return generateErrorResponse(HttpStatusCode::NOT_FOUND, serverConfig);
    }
    
    // Tentar remover o arquivo
    if (_fileSystemAdapter->deleteFile(filePath)) {
        HttpResponse response(HttpStatusCode::OK);
        response.setBody("File deleted successfully");
        response.setContentLength();
        return response;
    } else {
        return generateErrorResponse(HttpStatusCode::INTERNAL_SERVER_ERROR, serverConfig);
    }
}

HttpResponse ProcessHttpRequestUseCase::serveStaticFile(const std::string& filePath) {
    std::string content = _fileSystemAdapter->readFile(filePath);
    
    if (content.empty()) {
        HttpResponse response(HttpStatusCode::INTERNAL_SERVER_ERROR);
        response.setBody("Error reading file");
        response.setContentLength();
        return response;
    }
    
    HttpResponse response(HttpStatusCode::OK);
    response.setBody(content);
    response.setContentLength();
    
    // Determinar Content-Type baseado na extensão do arquivo
    if (filePath.find(".html") != std::string::npos) {
        response.addHeader("Content-Type", "text/html");
    } else if (filePath.find(".css") != std::string::npos) {
        response.addHeader("Content-Type", "text/css");
    } else if (filePath.find(".js") != std::string::npos) {
        response.addHeader("Content-Type", "application/javascript");
    } else if (filePath.find(".png") != std::string::npos) {
        response.addHeader("Content-Type", "image/png");
    } else if (filePath.find(".jpg") != std::string::npos || filePath.find(".jpeg") != std::string::npos) {
        response.addHeader("Content-Type", "image/jpeg");
    } else {
        response.addHeader("Content-Type", "text/plain");
    }
    
    return response;
}

HttpResponse ProcessHttpRequestUseCase::generateErrorResponse(HttpStatusCode statusCode, const ServerConfig& serverConfig) {
    HttpResponse response(statusCode);
    
    // Tentar carregar página de erro customizada
    if (!serverConfig.getErrorPage().empty()) {
        std::string errorPagePath = serverConfig.getRoot() + "/" + serverConfig.getErrorPage();
        std::string errorPageContent = _fileSystemAdapter->readFile(errorPagePath);
        
        if (!errorPageContent.empty()) {
            response.setBody(errorPageContent);
            response.addHeader("Content-Type", "text/html");
        } else {
            // Página de erro padrão
            response.setBody(generateDefaultErrorPage(statusCode));
            response.addHeader("Content-Type", "text/html");
        }
    } else {
        // Página de erro padrão
        response.setBody(generateDefaultErrorPage(statusCode));
        response.addHeader("Content-Type", "text/html");
    }
    
    response.setContentLength();
    return response;
}

std::string ProcessHttpRequestUseCase::resolveFilePath(const std::string& uri, const ServerConfig& serverConfig) {
    return serverConfig.getRoot() + uri;
}

std::string ProcessHttpRequestUseCase::generateDefaultErrorPage(HttpStatusCode statusCode) {
    std::ostringstream oss;
    oss << "<!DOCTYPE html><html><head><title>Error " << static_cast<int>(statusCode) 
        << "</title></head><body><h1>Error " << static_cast<int>(statusCode) 
        << "</h1><p>An error occurred while processing your request.</p></body></html>";
    return oss.str();
}
