#pragma once

#include "../entities/ServerConfig.hpp"
#include <vector>

/**
 * Port para carregamento de configurações
 * Define o contrato para obter configurações do servidor
 */
class IConfigurationLoader {
public:
    virtual ~IConfigurationLoader() = default;
    
    /**
     * Carrega configurações de servidores a partir de um arquivo
     * @param configPath - caminho para o arquivo de configuração
     * @return vetor de configurações de servidor
     */
    virtual std::vector<ServerConfig> loadConfiguration(const std::string& configPath) = 0;
    
    /**
     * Valida se um arquivo de configuração é válido
     * @param configPath - caminho para o arquivo de configuração
     * @return true se válido, false caso contrário
     */
    virtual bool validateConfiguration(const std::string& configPath) = 0;
};
