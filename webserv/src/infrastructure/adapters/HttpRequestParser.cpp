#include "HttpRequestParser.hpp"
#include <sstream>
#include <algorithm>
#include <tuple>

HttpRequestParser::HttpRequestParser() {}

HttpRequestParser::~HttpRequestParser() {}

HttpRequest HttpRequestParser::parse(const std::string& rawData) {
    if (!isValidRequest(rawData)) {
        return HttpRequest(); // Retorna requisição vazia/inválida
    }

    std::vector<std::string> lines = splitIntoLines(rawData);
    
    if (lines.empty()) {
        return HttpRequest();
    }

    // Parsear linha de requisição
    auto [method, uri, version] = parseRequestLine(lines[0]);
    HttpRequest request(method, uri, version);

    // Encontrar onde terminam os cabeçalhos (linha vazia)
    size_t headerEndIndex = 1;
    for (size_t i = 1; i < lines.size(); i++) {
        if (lines[i].empty()) {
            headerEndIndex = i;
            break;
        }
    }

    // Parsear cabeçalhos
    std::vector<std::string> headerLines(lines.begin() + 1, lines.begin() + headerEndIndex);
    auto headers = parseHeaders(headerLines);
    
    for (const auto& header : headers) {
        request.addHeader(header.first, header.second);
    }

    // Parsear corpo (se existir)
    if (headerEndIndex + 1 < lines.size()) {
        std::string body;
        for (size_t i = headerEndIndex + 1; i < lines.size(); i++) {
            body += lines[i];
            if (i < lines.size() - 1) {
                body += "\n";
            }
        }
        request.setBody(body);
    }

    return request;
}

bool HttpRequestParser::isValidRequest(const std::string& rawData) {
    if (rawData.empty()) {
        return false;
    }

    std::vector<std::string> lines = splitIntoLines(rawData);
    
    if (lines.empty()) {
        return false;
    }

    // Verificar linha de requisição
    std::istringstream iss(lines[0]);
    std::string method, uri, version;
    
    if (!(iss >> method >> uri >> version)) {
        return false;
    }

    // Verificar método HTTP válido
    method = toUpper(method);
    if (method != "GET" && method != "POST" && method != "DELETE" && 
        method != "PUT" && method != "HEAD" && method != "OPTIONS") {
        return false;
    }

    // Verificar URI não vazia
    if (uri.empty() || uri[0] != '/') {
        return false;
    }

    // Verificar versão HTTP
    if (version != "HTTP/1.1" && version != "HTTP/1.0") {
        return false;
    }

    return true;
}

std::tuple<std::string, std::string, std::string> HttpRequestParser::parseRequestLine(const std::string& requestLine) {
    std::istringstream iss(requestLine);
    std::string method, uri, version;
    
    iss >> method >> uri >> version;
    
    return std::make_tuple(toUpper(method), uri, version);
}

std::map<std::string, std::string> HttpRequestParser::parseHeaders(const std::vector<std::string>& headerLines) {
    std::map<std::string, std::string> headers;
    
    for (const auto& line : headerLines) {
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            std::string key = trim(line.substr(0, colonPos));
            std::string value = trim(line.substr(colonPos + 1));
            
            if (!key.empty()) {
                // Converter chave para formato padrão (primeira letra maiúscula)
                key[0] = std::toupper(key[0]);
                for (size_t i = 1; i < key.length(); i++) {
                    if (key[i-1] == '-') {
                        key[i] = std::toupper(key[i]);
                    } else {
                        key[i] = std::tolower(key[i]);
                    }
                }
                
                headers[key] = value;
            }
        }
    }
    
    return headers;
}

std::vector<std::string> HttpRequestParser::splitIntoLines(const std::string& rawData) {
    std::vector<std::string> lines;
    std::istringstream iss(rawData);
    std::string line;
    
    while (std::getline(iss, line)) {
        // Remover \r se presente (Windows line endings)
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        lines.push_back(line);
    }
    
    return lines;
}

std::string HttpRequestParser::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t");
    if (first == std::string::npos) {
        return "";
    }
    
    size_t last = str.find_last_not_of(" \t");
    return str.substr(first, (last - first + 1));
}

std::string HttpRequestParser::toUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}
