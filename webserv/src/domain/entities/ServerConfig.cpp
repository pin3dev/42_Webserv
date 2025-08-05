#include "ServerConfig.hpp"
#include <algorithm>

bool LocationConfig::isMethodAllowed(const std::string& method) const {
    return std::find(allowedMethods.begin(), allowedMethods.end(), method) != allowedMethods.end();
}

ServerConfig::ServerConfig() 
    : _serverName(""), _host("localhost"), _port("80"), _root(""),
      _indexPage("index.html"), _errorPage(""), _clientMaxBodySize(1048576),
      _isDefault(false) {}

ServerConfig::~ServerConfig() {}

const std::string& ServerConfig::getServerName() const {
    return _serverName;
}

const std::string& ServerConfig::getHost() const {
    return _host;
}

const std::string& ServerConfig::getPort() const {
    return _port;
}

const std::string& ServerConfig::getRoot() const {
    return _root;
}

const std::string& ServerConfig::getIndexPage() const {
    return _indexPage;
}

const std::string& ServerConfig::getErrorPage() const {
    return _errorPage;
}

size_t ServerConfig::getClientMaxBodySize() const {
    return _clientMaxBodySize;
}

bool ServerConfig::isDefault() const {
    return _isDefault;
}

const std::map<std::string, LocationConfig>& ServerConfig::getLocations() const {
    return _locations;
}

void ServerConfig::setServerName(const std::string& serverName) {
    _serverName = serverName;
}

void ServerConfig::setHost(const std::string& host) {
    _host = host;
}

void ServerConfig::setPort(const std::string& port) {
    _port = port;
}

void ServerConfig::setRoot(const std::string& root) {
    _root = root;
}

void ServerConfig::setIndexPage(const std::string& indexPage) {
    _indexPage = indexPage;
}

void ServerConfig::setErrorPage(const std::string& errorPage) {
    _errorPage = errorPage;
}

void ServerConfig::setClientMaxBodySize(size_t size) {
    _clientMaxBodySize = size;
}

void ServerConfig::setIsDefault(bool isDefault) {
    _isDefault = isDefault;
}

void ServerConfig::addLocation(const std::string& path, const LocationConfig& location) {
    _locations[path] = location;
}

const LocationConfig* ServerConfig::getLocation(const std::string& path) const {
    auto it = _locations.find(path);
    return (it != _locations.end()) ? &it->second : nullptr;
}

const LocationConfig* ServerConfig::findMatchingLocation(const std::string& uri) const {
    // Find the most specific location that matches the URI
    const LocationConfig* bestMatch = nullptr;
    size_t bestMatchLength = 0;
    
    for (const auto& pair : _locations) {
        const std::string& locationPath = pair.first;
        if (uri.find(locationPath) == 0 && locationPath.length() > bestMatchLength) {
            bestMatch = &pair.second;
            bestMatchLength = locationPath.length();
        }
    }
    
    return bestMatch;
}

bool ServerConfig::isValid() const {
    return !_serverName.empty() && !_host.empty() && !_port.empty() && !_root.empty();
}
