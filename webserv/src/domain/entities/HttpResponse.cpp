#include "HttpResponse.hpp"
#include <sstream>

HttpResponse::HttpResponse() 
    : _statusCode(HttpStatusCode::OK), _version("HTTP/1.1") {
    _reasonPhrase = getReasonPhraseForStatus(_statusCode);
}

HttpResponse::HttpResponse(HttpStatusCode statusCode, const std::string& reasonPhrase)
    : _statusCode(statusCode), _version("HTTP/1.1") {
    if (reasonPhrase.empty()) {
        _reasonPhrase = getReasonPhraseForStatus(statusCode);
    } else {
        _reasonPhrase = reasonPhrase;
    }
}

HttpResponse::~HttpResponse() {}

HttpStatusCode HttpResponse::getStatusCode() const {
    return _statusCode;
}

int HttpResponse::getStatusCodeAsInt() const {
    return static_cast<int>(_statusCode);
}

const std::string& HttpResponse::getReasonPhrase() const {
    return _reasonPhrase;
}

const std::map<std::string, std::string>& HttpResponse::getHeaders() const {
    return _headers;
}

const std::string& HttpResponse::getBody() const {
    return _body;
}

const std::string& HttpResponse::getVersion() const {
    return _version;
}

void HttpResponse::setStatusCode(HttpStatusCode statusCode) {
    _statusCode = statusCode;
    _reasonPhrase = getReasonPhraseForStatus(statusCode);
}

void HttpResponse::setReasonPhrase(const std::string& reasonPhrase) {
    _reasonPhrase = reasonPhrase;
}

void HttpResponse::setBody(const std::string& body) {
    _body = body;
}

void HttpResponse::setVersion(const std::string& version) {
    _version = version;
}

void HttpResponse::addHeader(const std::string& key, const std::string& value) {
    _headers[key] = value;
}

std::string HttpResponse::getHeader(const std::string& key) const {
    auto it = _headers.find(key);
    return (it != _headers.end()) ? it->second : "";
}

bool HttpResponse::hasHeader(const std::string& key) const {
    return _headers.find(key) != _headers.end();
}

std::string HttpResponse::toString() const {
    std::ostringstream oss;
    
    // Status line
    oss << _version << " " << static_cast<int>(_statusCode) << " " << _reasonPhrase << "\r\n";
    
    // Headers
    for (const auto& header : _headers) {
        oss << header.first << ": " << header.second << "\r\n";
    }
    
    // Empty line
    oss << "\r\n";
    
    // Body
    oss << _body;
    
    return oss.str();
}

void HttpResponse::setContentLength() {
    addHeader("Content-Length", std::to_string(_body.length()));
}

std::string HttpResponse::getReasonPhraseForStatus(HttpStatusCode statusCode) const {
    switch (statusCode) {
        case HttpStatusCode::OK:
            return "OK";
        case HttpStatusCode::NOT_FOUND:
            return "Not Found";
        case HttpStatusCode::INTERNAL_SERVER_ERROR:
            return "Internal Server Error";
        case HttpStatusCode::BAD_REQUEST:
            return "Bad Request";
        case HttpStatusCode::METHOD_NOT_ALLOWED:
            return "Method Not Allowed";
        default:
            return "Unknown";
    }
}
