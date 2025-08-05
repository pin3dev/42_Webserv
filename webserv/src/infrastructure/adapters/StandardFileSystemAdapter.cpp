#include "StandardFileSystemAdapter.hpp"
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <cstring>

StandardFileSystemAdapter::StandardFileSystemAdapter() {}

StandardFileSystemAdapter::~StandardFileSystemAdapter() {}

std::string StandardFileSystemAdapter::readFile(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    
    if (!file.is_open()) {
        std::cerr << "Cannot open file for reading: " << filePath << std::endl;
        return "";
    }
    
    // Ler todo o conteúdo do arquivo
    std::ostringstream oss;
    oss << file.rdbuf();
    
    if (file.bad()) {
        std::cerr << "Error reading file: " << filePath << std::endl;
        return "";
    }
    
    return oss.str();
}

bool StandardFileSystemAdapter::writeFile(const std::string& filePath, const std::string& content) {
    std::ofstream file(filePath, std::ios::binary);
    
    if (!file.is_open()) {
        std::cerr << "Cannot open file for writing: " << filePath << std::endl;
        return false;
    }
    
    file << content;
    
    if (file.bad()) {
        std::cerr << "Error writing file: " << filePath << std::endl;
        return false;
    }
    
    return true;
}

bool StandardFileSystemAdapter::fileExists(const std::string& filePath) {
    struct stat statBuf;
    return stat(filePath.c_str(), &statBuf) == 0 && S_ISREG(statBuf.st_mode);
}

bool StandardFileSystemAdapter::directoryExists(const std::string& dirPath) {
    struct stat statBuf;
    return stat(dirPath.c_str(), &statBuf) == 0 && S_ISDIR(statBuf.st_mode);
}

bool StandardFileSystemAdapter::deleteFile(const std::string& filePath) {
    if (!fileExists(filePath)) {
        return false;
    }
    
    int result = unlink(filePath.c_str());
    
    if (result != 0) {
        std::cerr << "Error deleting file " << filePath << ": " << strerror(errno) << std::endl;
        return false;
    }
    
    return true;
}

long StandardFileSystemAdapter::getFileSize(const std::string& filePath) {
    struct stat statBuf;
    
    if (stat(filePath.c_str(), &statBuf) != 0) {
        std::cerr << "Error getting file size for " << filePath << ": " << strerror(errno) << std::endl;
        return -1;
    }
    
    if (!S_ISREG(statBuf.st_mode)) {
        return -1; // Não é um arquivo regular
    }
    
    return statBuf.st_size;
}

std::vector<std::string> StandardFileSystemAdapter::listDirectory(const std::string& dirPath) {
    std::vector<std::string> files;
    
    DIR* dir = opendir(dirPath.c_str());
    if (!dir) {
        std::cerr << "Cannot open directory: " << dirPath << std::endl;
        return files;
    }
    
    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string fileName = entry->d_name;
        
        // Pular entradas . e ..
        if (fileName == "." || fileName == "..") {
            continue;
        }
        
        files.push_back(fileName);
    }
    
    closedir(dir);
    return files;
}

bool StandardFileSystemAdapter::isRegularFile(const std::string& path) {
    struct stat statBuf;
    return stat(path.c_str(), &statBuf) == 0 && S_ISREG(statBuf.st_mode);
}

bool StandardFileSystemAdapter::isDirectory(const std::string& path) {
    struct stat statBuf;
    return stat(path.c_str(), &statBuf) == 0 && S_ISDIR(statBuf.st_mode);
}
