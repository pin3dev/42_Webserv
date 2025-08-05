#pragma once

#include <string>
#include <map>

enum class HttpStatusCode {
    OK = 200,
    NOT_FOUND = 404,
    INTERNAL_SERVER_ERROR = 500,
    BAD_REQUEST = 400,
    METHOD_NOT_ALLOWED = 405
};

class HttpResponse {
private:
    HttpStatusCode _statusCode;
    std::string _reasonPhrase;
    std::map<std::string, std::string> _headers;
    std::string _body;
    std::string _version;

public:
    HttpResponse();
    HttpResponse(HttpStatusCode statusCode, const std::string& reasonPhrase = "");
    ~HttpResponse();

    // Getters
    HttpStatusCode getStatusCode() const;
    int getStatusCodeAsInt() const;
    const std::string& getReasonPhrase() const;
    const std::map<std::string, std::string>& getHeaders() const;
    const std::string& getBody() const;
    const std::string& getVersion() const;
    
    // Setters
    void setStatusCode(HttpStatusCode statusCode);
    void setReasonPhrase(const std::string& reasonPhrase);
    void setBody(const std::string& body);
    void setVersion(const std::string& version);
    
    // Header operations
    void addHeader(const std::string& key, const std::string& value);
    std::string getHeader(const std::string& key) const;
    bool hasHeader(const std::string& key) const;
    
    // Utility methods
    std::string toString() const;
    void setContentLength();
    
private:
    std::string getReasonPhraseForStatus(HttpStatusCode statusCode) const;
};
