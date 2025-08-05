#pragma once

#include "../entities/HttpRequest.hpp"

/**
 * Port para parsing de requisições HTTP
 * Define o contrato para converter dados brutos em objetos HttpRequest
 */
class IHttpRequestParser {
public:
    virtual ~IHttpRequestParser() = default;
    
    /**
     * Converte dados brutos de uma requisição HTTP em um objeto HttpRequest
     * @param rawData - dados brutos da requisição
     * @return objeto HttpRequest parseado
     */
    virtual HttpRequest parse(const std::string& rawData) = 0;
    
    /**
     * Verifica se os dados fornecidos representam uma requisição HTTP válida
     * @param rawData - dados brutos para verificação
     * @return true se válido, false caso contrário
     */
    virtual bool isValidRequest(const std::string& rawData) = 0;
};
