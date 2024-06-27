
#pragma once

#include <exception>
#include <algorithm>   
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cstdlib>  // Para strtoull
#include <climits>  // Para ULLONG_MAX
#include <sstream>
#include <cstring>
#include "Server.hpp"

class Request
{
    private:
        static std::string const _generalAllowedMethods[3];
        static std::string const _HTTPunsupported[3];
        
        std::string	_version;
		std::string _url;
		std::string	_method;
        std::string _host;
		std::vector<char>	_payload;
		size_t		_bodyLength;
        size_t      _maxLength;

        bool        _hasHeaders;
        bool        _hasPayload;
		std::map<std::string, std::string>	_headerData;
        
        void    _setMethodAndURL(std::stringstream &ss);
        void    _setHeaders(std::stringstream &ss);
        void    _setHost();
        void    _setBodyLength();

        bool    _isAt(std::string str);
    
    public:
        Request();
        ~Request();

        void	toAppend(char const *buffer, size_t size);
        void    checkRequestContent();

        std::string getVersion() const;
        std::string getURL() const;
        std::string getMethod() const;
        std::string getHost() const;
        std::vector<char> &getPayload();
        size_t  getBodyLength() const;
        size_t  getMaxLength() const;
        std::map<std::string, std::string> getHeadData() const;
        bool	isReadyToResponse();
        bool	hasHeaders() const;
        bool	hasPayload() const;
        bool    isChunked();

        void    setMaxLength(size_t length);
        void    setHasHeaders(char *buffer, size_t size);
        void    setPayload(char *buffer, size_t size);
        void    handleChunked();

       // void    setAllClean();
};
