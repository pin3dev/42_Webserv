#pragma once

#include "../../domain/ports/IConfigurationLoader.hpp"
#include <string>
#include <vector>
#include <fstream>
#include <map>

/**
 * Adaptador concreto para carregamento de configurações
 * Implementa a interface IConfigurationLoader usando arquivos do sistema
 */
class FileConfigurationLoader : public IConfigurationLoader {
private:
    std::ifstream _fileStream;
    std::map<std::string, std::string> _currentSettings;
    std::map<std::string, LocationConfig> _currentLocations;

public:
    FileConfigurationLoader();
    ~FileConfigurationLoader();

    // Implementação da interface IConfigurationLoader
    std::vector<ServerConfig> loadConfiguration(const std::string& configPath) override;
    bool validateConfiguration(const std::string& configPath) override;

private:
    /**
     * Parseia um bloco de servidor do arquivo de configuração
     * @param content - conteúdo do arquivo
     * @param pos - posição atual no conteúdo (será atualizada)
     * @return configuração do servidor parseada
     */
    ServerConfig parseServerBlock(const std::string& content, size_t& pos);

    /**
     * Parseia um bloco de localização
     * @param content - conteúdo do arquivo
     * @param pos - posição atual no conteúdo (será atualizada)
     * @return par com o caminho e configuração da localização
     */
    std::pair<std::string, LocationConfig> parseLocationBlock(const std::string& content, size_t& pos);

    /**
     * Extrai uma linha de configuração (chave valor)
     * @param line - linha do arquivo de configuração
     * @return par com chave e valor
     */
    std::pair<std::string, std::string> parseConfigLine(const std::string& line);

    /**
     * Remove espaços em branco do início e fim de uma string
     * @param str - string para limpar
     * @return string limpa
     */
    std::string trim(const std::string& str);

    /**
     * Verifica se uma linha é um comentário
     * @param line - linha para verificar
     * @return true se é comentário, false caso contrário
     */
    bool isComment(const std::string& line);

    /**
     * Converte configurações parseadas em objeto ServerConfig
     * @param settings - mapa de configurações
     * @param locations - mapa de localizações
     * @return objeto ServerConfig
     */
    ServerConfig createServerConfig(const std::map<std::string, std::string>& settings,
                                  const std::map<std::string, LocationConfig>& locations);

    /**
     * Parseia o tamanho com sufixos (K, M, G)
     * @param sizeStr - string com o tamanho
     * @return tamanho em bytes
     */
    size_t parseSizeWithSuffix(const std::string& sizeStr);

    /**
     * Separa uma string por delimitador
     * @param str - string para separar
     * @param delimiter - delimitador
     * @return vetor de strings
     */
    std::vector<std::string> split(const std::string& str, char delimiter);
};
