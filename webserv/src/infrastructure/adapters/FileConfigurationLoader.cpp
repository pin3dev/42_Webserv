#include "FileConfigurationLoader.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>

FileConfigurationLoader::FileConfigurationLoader() {}

FileConfigurationLoader::~FileConfigurationLoader() {
    if (_fileStream.is_open()) {
        _fileStream.close();
    }
}

std::vector<ServerConfig> FileConfigurationLoader::loadConfiguration(const std::string& configPath) {
    std::vector<ServerConfig> configs;
    
    _fileStream.open(configPath);
    if (!_fileStream.is_open()) {
        throw std::runtime_error("Cannot open configuration file: " + configPath);
    }
    
    // Ler todo o conteúdo do arquivo
    std::string content((std::istreambuf_iterator<char>(_fileStream)),
                        std::istreambuf_iterator<char>());
    _fileStream.close();
    
    // Processar blocos de servidor
    size_t pos = 0;
    while (pos < content.length()) {
        // Pular espaços em branco e comentários
        while (pos < content.length() && (std::isspace(content[pos]) || content[pos] == '#')) {
            if (content[pos] == '#') {
                // Pular até o fim da linha
                while (pos < content.length() && content[pos] != '\n') {
                    pos++;
                }
            }
            pos++;
        }
        
        // Procurar por "server {"
        if (pos < content.length() && content.substr(pos, 6) == "server") {
            ServerConfig config = parseServerBlock(content, pos);
            if (config.isValid()) {
                configs.push_back(config);
            }
        } else {
            pos++;
        }
    }
    
    return configs;
}

bool FileConfigurationLoader::validateConfiguration(const std::string& configPath) {
    try {
        std::vector<ServerConfig> configs = loadConfiguration(configPath);
        return !configs.empty();
    } catch (const std::exception&) {
        return false;
    }
}

ServerConfig FileConfigurationLoader::parseServerBlock(const std::string& content, size_t& pos) {
    std::map<std::string, std::string> settings;
    std::map<std::string, LocationConfig> locations;
    
    // Pular "server"
    pos += 6;
    
    // Encontrar '{'
    while (pos < content.length() && content[pos] != '{') {
        pos++;
    }
    pos++; // Pular '{'
    
    int braceLevel = 1;
    std::string currentLine;
    
    while (pos < content.length() && braceLevel > 0) {
        char c = content[pos];
        
        if (c == '{') {
            braceLevel++;
        } else if (c == '}') {
            braceLevel--;
            if (braceLevel == 0) {
                break;
            }
        }
        
        if (c == '\n' || c == ';') {
            if (!currentLine.empty() && !isComment(currentLine)) {
                currentLine = trim(currentLine);
                
                // Verificar se é um bloco de localização
                if (currentLine.find("location") == 0) {
                    // Voltar para processar o bloco de localização
                    size_t locationStart = pos - currentLine.length();
                    auto locationPair = parseLocationBlock(content, locationStart);
                    locations[locationPair.first] = locationPair.second;
                    pos = locationStart;
                } else {
                    // Parsear linha de configuração normal
                    auto configPair = parseConfigLine(currentLine);
                    if (!configPair.first.empty()) {
                        settings[configPair.first] = configPair.second;
                    }
                }
            }
            currentLine.clear();
        } else {
            currentLine += c;
        }
        
        pos++;
    }
    
    return createServerConfig(settings, locations);
}

std::pair<std::string, LocationConfig> FileConfigurationLoader::parseLocationBlock(const std::string& content, size_t& pos) {
    LocationConfig location;
    std::string locationPath;
    
    // Encontrar o caminho da localização
    while (pos < content.length() && content.substr(pos, 8) != "location") {
        pos++;
    }
    pos += 8; // Pular "location"
    
    // Extrair o caminho
    while (pos < content.length() && std::isspace(content[pos])) {
        pos++;
    }
    
    while (pos < content.length() && !std::isspace(content[pos]) && content[pos] != '{') {
        locationPath += content[pos];
        pos++;
    }
    
    // Encontrar '{'
    while (pos < content.length() && content[pos] != '{') {
        pos++;
    }
    pos++; // Pular '{'
    
    int braceLevel = 1;
    std::string currentLine;
    
    while (pos < content.length() && braceLevel > 0) {
        char c = content[pos];
        
        if (c == '{') {
            braceLevel++;
        } else if (c == '}') {
            braceLevel--;
            if (braceLevel == 0) {
                break;
            }
        }
        
        if (c == '\n' || c == ';') {
            if (!currentLine.empty() && !isComment(currentLine)) {
                currentLine = trim(currentLine);
                auto configPair = parseConfigLine(currentLine);
                
                if (configPair.first == "allow_methods") {
                    location.allowedMethods = split(configPair.second, ' ');
                } else if (configPair.first == "cgi_path") {
                    location.cgiPath = configPair.second;
                } else if (configPair.first == "cgi_ext") {
                    location.cgiExtension = configPair.second;
                } else if (configPair.first == "upload_to") {
                    location.uploadPath = configPair.second;
                } else if (configPair.first == "try_file") {
                    location.tryFile = configPair.second;
                }
            }
            currentLine.clear();
        } else {
            currentLine += c;
        }
        
        pos++;
    }
    
    location.path = locationPath;
    return std::make_pair(locationPath, location);
}

std::pair<std::string, std::string> FileConfigurationLoader::parseConfigLine(const std::string& line) {
    size_t spacePos = line.find(' ');
    if (spacePos == std::string::npos || spacePos == 0) {
        return std::make_pair("", "");
    }
    
    std::string key = trim(line.substr(0, spacePos));
    std::string value = trim(line.substr(spacePos + 1));
    
    return std::make_pair(key, value);
}

std::string FileConfigurationLoader::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) {
        return "";
    }
    
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

bool FileConfigurationLoader::isComment(const std::string& line) {
    std::string trimmed = trim(line);
    return !trimmed.empty() && trimmed[0] == '#';
}

ServerConfig FileConfigurationLoader::createServerConfig(const std::map<std::string, std::string>& settings,
                                                       const std::map<std::string, LocationConfig>& locations) {
    ServerConfig config;
    
    for (const auto& setting : settings) {
        if (setting.first == "server_name") {
            config.setServerName(setting.second);
        } else if (setting.first == "listen") {
            config.setPort(setting.second);
        } else if (setting.first == "host") {
            config.setHost(setting.second);
        } else if (setting.first == "root") {
            config.setRoot(setting.second);
        } else if (setting.first == "index") {
            config.setIndexPage(setting.second);
        } else if (setting.first == "error_page") {
            config.setErrorPage(setting.second);
        } else if (setting.first == "client_max_body_size") {
            config.setClientMaxBodySize(parseSizeWithSuffix(setting.second));
        }
    }
    
    // Adicionar localizações
    for (const auto& location : locations) {
        config.addLocation(location.first, location.second);
    }
    
    // Definir como padrão se não houver outros servidores padrão
    config.setIsDefault(true);
    
    return config;
}

size_t FileConfigurationLoader::parseSizeWithSuffix(const std::string& sizeStr) {
    if (sizeStr.empty()) {
        return 1048576; // 1MB padrão
    }
    
    size_t multiplier = 1;
    std::string numStr = sizeStr;
    
    char lastChar = std::tolower(sizeStr.back());
    if (lastChar == 'k') {
        multiplier = 1024;
        numStr = sizeStr.substr(0, sizeStr.length() - 1);
    } else if (lastChar == 'm') {
        multiplier = 1024 * 1024;
        numStr = sizeStr.substr(0, sizeStr.length() - 1);
    } else if (lastChar == 'g') {
        multiplier = 1024 * 1024 * 1024;
        numStr = sizeStr.substr(0, sizeStr.length() - 1);
    }
    
    try {
        return std::stoull(numStr) * multiplier;
    } catch (const std::exception&) {
        return 1048576; // 1MB padrão em caso de erro
    }
}

std::vector<std::string> FileConfigurationLoader::split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    
    while (std::getline(ss, token, delimiter)) {
        token = trim(token);
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    
    return tokens;
}
