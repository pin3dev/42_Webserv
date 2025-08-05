#pragma once

#include "../../domain/ports/IHttpRequestParser.hpp"
#include <tuple>
#include <vector>

/**
 * Parser HTTP concreto
 * Implementa o parsing de requisições HTTP seguindo RFC 7230
 */
class HttpRequestParser : public IHttpRequestParser {
public:
    HttpRequestParser();
    ~HttpRequestParser();

    // Implementação da interface IHttpRequestParser
    HttpRequest parse(const std::string& rawData) override;
    bool isValidRequest(const std::string& rawData) override;

private:
    /**
     * Parseia a linha de requisição (método, URI, versão)
     * @param requestLine - primeira linha da requisição HTTP
     * @return tupla com método, URI e versão
     */
    std::tuple<std::string, std::string, std::string> parseRequestLine(const std::string& requestLine);

    /**
     * Parseia os cabeçalhos HTTP
     * @param headerLines - linhas de cabeçalho
     * @return mapa de cabeçalhos
     */
    std::map<std::string, std::string> parseHeaders(const std::vector<std::string>& headerLines);

    /**
     * Separa os dados brutos em linhas
     * @param rawData - dados brutos da requisição
     * @return vetor de linhas
     */
    std::vector<std::string> splitIntoLines(const std::string& rawData);

    /**
     * Remove espaços em branco do início e fim de uma string
     * @param str - string para limpar
     * @return string limpa
     */
    std::string trim(const std::string& str);

    /**
     * Converte string para maiúscula
     * @param str - string para converter
     * @return string em maiúscula
     */
    std::string toUpper(const std::string& str);
};
