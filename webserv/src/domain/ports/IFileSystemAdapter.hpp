#pragma once

#include "../entities/HttpRequest.hpp"
#include "../entities/HttpResponse.hpp"
#include <vector>

/**
 * Port para processamento de arquivos
 * Define o contrato para operações de sistema de arquivos
 */
class IFileSystemAdapter {
public:
    virtual ~IFileSystemAdapter() = default;
    
    /**
     * Lê o conteúdo de um arquivo
     * @param filePath - caminho para o arquivo
     * @return conteúdo do arquivo ou string vazia se erro
     */
    virtual std::string readFile(const std::string& filePath) = 0;
    
    /**
     * Escreve conteúdo em um arquivo
     * @param filePath - caminho para o arquivo
     * @param content - conteúdo a escrever
     * @return true se sucesso, false caso contrário
     */
    virtual bool writeFile(const std::string& filePath, const std::string& content) = 0;
    
    /**
     * Verifica se um arquivo existe
     * @param filePath - caminho para o arquivo
     * @return true se existe, false caso contrário
     */
    virtual bool fileExists(const std::string& filePath) = 0;
    
    /**
     * Verifica se um diretório existe
     * @param dirPath - caminho para o diretório
     * @return true se existe, false caso contrário
     */
    virtual bool directoryExists(const std::string& dirPath) = 0;
    
    /**
     * Remove um arquivo
     * @param filePath - caminho para o arquivo
     * @return true se sucesso, false caso contrário
     */
    virtual bool deleteFile(const std::string& filePath) = 0;
    
    /**
     * Obtém o tamanho de um arquivo
     * @param filePath - caminho para o arquivo
     * @return tamanho do arquivo ou -1 se erro
     */
    virtual long getFileSize(const std::string& filePath) = 0;
    
    /**
     * Lista arquivos em um diretório
     * @param dirPath - caminho para o diretório
     * @return lista de nomes de arquivos
     */
    virtual std::vector<std::string> listDirectory(const std::string& dirPath) = 0;
};
