#pragma once

#include <string>
#include <vector>
#include <map>

struct LocationConfig {
    std::string path;
    std::vector<std::string> allowedMethods;
    std::string cgiPath;
    std::string cgiExtension;
    std::string uploadPath;
    std::string tryFile;
    
    LocationConfig() = default;
    bool isMethodAllowed(const std::string& method) const;
};

class ServerConfig {
private:
    std::string _serverName;
    std::string _host;
    std::string _port;
    std::string _root;
    std::string _indexPage;
    std::string _errorPage;
    size_t _clientMaxBodySize;
    bool _isDefault;
    std::map<std::string, LocationConfig> _locations;

public:
    ServerConfig();
    ~ServerConfig();

    // Getters
    const std::string& getServerName() const;
    const std::string& getHost() const;
    const std::string& getPort() const;
    const std::string& getRoot() const;
    const std::string& getIndexPage() const;
    const std::string& getErrorPage() const;
    size_t getClientMaxBodySize() const;
    bool isDefault() const;
    const std::map<std::string, LocationConfig>& getLocations() const;
    
    // Setters
    void setServerName(const std::string& serverName);
    void setHost(const std::string& host);
    void setPort(const std::string& port);
    void setRoot(const std::string& root);
    void setIndexPage(const std::string& indexPage);
    void setErrorPage(const std::string& errorPage);
    void setClientMaxBodySize(size_t size);
    void setIsDefault(bool isDefault);
    
    // Location operations
    void addLocation(const std::string& path, const LocationConfig& location);
    const LocationConfig* getLocation(const std::string& path) const;
    const LocationConfig* findMatchingLocation(const std::string& uri) const;
    
    // Validation
    bool isValid() const;
};
