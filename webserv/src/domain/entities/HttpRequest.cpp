#include "HttpRequest.hpp"

HttpRequest::HttpRequest() 
    : _method(""), _uri(""), _version(""), _body("") {}

HttpRequest::HttpRequest(const std::string& method, const std::string& uri, const std::string& version)
    : _method(method), _uri(uri), _version(version), _body("") {}

HttpRequest::~HttpRequest() {}

const std::string& HttpRequest::getMethod() const {
    return _method;
}

const std::string& HttpRequest::getUri() const {
    return _uri;
}

const std::string& HttpRequest::getVersion() const {
    return _version;
}

const std::map<std::string, std::string>& HttpRequest::getHeaders() const {
    return _headers;
}

const std::string& HttpRequest::getBody() const {
    return _body;
}

void HttpRequest::addHeader(const std::string& key, const std::string& value) {
    _headers[key] = value;
}

std::string HttpRequest::getHeader(const std::string& key) const {
    auto it = _headers.find(key);
    return (it != _headers.end()) ? it->second : "";
}

bool HttpRequest::hasHeader(const std::string& key) const {
    return _headers.find(key) != _headers.end();
}

void HttpRequest::setBody(const std::string& body) {
    _body = body;
}

bool HttpRequest::isValid() const {
    return !_method.empty() && !_uri.empty() && !_version.empty();
}
