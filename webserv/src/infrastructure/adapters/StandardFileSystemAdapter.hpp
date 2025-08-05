#pragma once

#include "../../domain/ports/IFileSystemAdapter.hpp"
#include <fstream>
#include <vector>
#include <sys/stat.h>
#include <dirent.h>

/**
 * Adaptador concreto para operações de sistema de arquivos
 * Implementa a interface IFileSystemAdapter usando APIs padrão do sistema
 */
class StandardFileSystemAdapter : public IFileSystemAdapter {
public:
    StandardFileSystemAdapter();
    ~StandardFileSystemAdapter();

    // Implementação da interface IFileSystemAdapter
    std::string readFile(const std::string& filePath) override;
    bool writeFile(const std::string& filePath, const std::string& content) override;
    bool fileExists(const std::string& filePath) override;
    bool directoryExists(const std::string& dirPath) override;
    bool deleteFile(const std::string& filePath) override;
    long getFileSize(const std::string& filePath) override;
    std::vector<std::string> listDirectory(const std::string& dirPath) override;

private:
    /**
     * Verifica se um caminho é um arquivo regular
     * @param path - caminho para verificar
     * @return true se é arquivo regular, false caso contrário
     */
    bool isRegularFile(const std::string& path);

    /**
     * Verifica se um caminho é um diretório
     * @param path - caminho para verificar
     * @return true se é diretório, false caso contrário
     */
    bool isDirectory(const std::string& path);
};
