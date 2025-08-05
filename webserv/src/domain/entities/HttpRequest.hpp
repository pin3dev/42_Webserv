#pragma once

#include <string>
#include <map>

class HttpRequest {
private:
    std::string _method;
    std::string _uri;
    std::string _version;
    std::map<std::string, std::string> _headers;
    std::string _body;

public:
    HttpRequest();
    HttpRequest(const std::string& method, const std::string& uri, const std::string& version);
    ~HttpRequest();

    // Getters
    const std::string& getMethod() const;
    const std::string& getUri() const;
    const std::string& getVersion() const;
    const std::map<std::string, std::string>& getHeaders() const;
    const std::string& getBody() const;
    
    // Header operations
    void addHeader(const std::string& key, const std::string& value);
    std::string getHeader(const std::string& key) const;
    bool hasHeader(const std::string& key) const;
    
    // Body operations
    void setBody(const std::string& body);
    
    // Validation
    bool isValid() const;
};
