/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pin3dev <pinedev@outlook.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 19:10:47 by pin3dev           #+#    #+#             */
/*   Updated: 2024/05/17 20:11:36 by pin3dev          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

std::string const Request::_generalAllowedMethods[3] = {"GET", "POST", "DELETE"};
std::string const Request::_HTTPunsupported[3] = {"HTTP/0.9", "HTTP/1.0", "HTTP/2.0"};

/** TODO:
 * 1. Verificar se é necessário o \r ou \n\r ao final da primeira linha do request
 * 2. Verificar se o _maxLength está sendo setado corretamente
 * 3. Refazer armazenamento do payload
*/

/** 
 * **************************
 * SECTION - UTILS
 * **************************
*/

std::vector<std::string> _split(std::string const &str, char delim)
{
	std::vector<std::string> tokens;
	std::stringstream ss(str);
	std::string token;

	while (std::getline(ss, token, delim))
	{
		if (!token.empty())
			tokens.push_back(token);
	}
	return tokens;
}

void _trim(std::string &s, char const *set)
{
	s.erase(0, s.find_first_not_of(set));
	s.erase(s.find_last_not_of(set) + 1);
}

std::string _decoder(const std::string &url)
{
	std::string tmp = url;
	size_t pos;

	while ((pos = tmp.find("%20")) != std::string::npos)
		tmp.replace(pos, 3, " ");
    return tmp;
}

bool _isAt(const std::string &str, const std::string (&arr)[3])
{
    return (std::find(std::begin(arr), std::end(arr), str) != std::end(arr));
}


/** 
 * **************************************
 * SECTION - CONSTRUCTORS & DESTRUCTORS
 * **************************************
*/

Request::Request()
{
    this->_request.clear();
    this->_url.clear();
    this->_method.clear();
    this->_payload.clear();
    this->_bodyLength = 0;
    this->_headerData.clear();
    this->_maxLength = 10000; //***SO PARA TESTE, DEPOIS CONFIGURAR PARA 0 E DEPOIS COM O SET CONFIGURAR PARA TAMANHO PASSADO PELO SERVER
}
Request::~Request()
{}

/** 
 * **************************
 * SECTION - PUBLIC METHODS
 * **************************
*/

void	Request::toAppend(char const *buffer, size_t size)
{
    this->_request.append(buffer, size);
}

/** 
 * **************************
 * SECTION - SETTERS/CHECKERS
 * **************************
*/

void	Request::checkStatusRequest()
{
	std::stringstream	ss(this->_request);
    this->_setMethodAndURL(ss);
	this->_setHeaders(ss);
	if (this->_method == "POST")
        this->_setPayload(ss);
}

void Request::_setMethodAndURL(std::stringstream &ss)
{
    std::string		firstLine;
    std::getline(ss, firstLine);
    std::vector<std::string> method_URL_version = _split(firstLine, ' ');

    bool hasValidAmount = method_URL_version.size() == 3;
    bool hasAllowedMethod = _isAt(method_URL_version[0], this->_generalAllowedMethods);
    bool hasSensibleURL = method_URL_version[1].find("../") != std::string::npos && method_URL_version[1] == "..";
    bool hasValidURLSize = method_URL_version[1].length() <= 1024;
    bool hasUnsupportedHTTP = _isAt(method_URL_version[2], this->_HTTPunsupported);
    bool hasValidHTTP = method_URL_version[2] == "HTTP/1.1";
    
    if (!hasValidAmount)
        throw std::runtime_error("400 Bad Request"); //response
    if (!hasAllowedMethod)
        throw std::runtime_error("501 Not Implemented"); //responses
    if (hasSensibleURL)
        throw std::runtime_error("400 Bad Request"); //response
    if (!hasValidURLSize)
        throw std::runtime_error("414 URI Too Long"); //response
    if (hasUnsupportedHTTP)
        throw std::runtime_error("505 HTTP Version Not Supported"); //response
    if (!hasValidHTTP)
        throw std::runtime_error("400 Bad Request"); //response

    this->_method = method_URL_version[0];
    this->_url = _decoder(method_URL_version[1]);
}

void	Request::_setHeaders(std::stringstream &ss)
{
	std::string curLine;
	while (std::getline(ss, curLine))
	{
		if (curLine == "\r") 
			break;

		if (curLine.find(':') != std::string::npos) 
		{
			std::string key = curLine.substr(0, curLine.find(':'));
			std::string value = curLine.substr(curLine.find(':') + 1, curLine.find('\n'));
			_trim(key, " \t\r\v\f");
			_trim(value, " \t\r\v\f");

			if (value.length() == 0 || key.length() == 0)
				throw std::runtime_error("400 Bad Request");
			this->_headerData[key] = value;
		}
	}
}


void    Request::_setPayload(std::stringstream &ss)
{
    this->_setBodyLength();

    //** refazer armazenamento do payload
}

void    Request::_setBodyLength()
{
    bool hasContentLength = this->_headerData.find("Content-Length") != this->_headerData.end();
    if (!hasContentLength)
        throw std::runtime_error("411 Length Required"); //response
    
    std::string const lenght = this->_headerData["Content-Length"];
    
    bool isDigit = lenght.find_first_not_of("0123456789") == std::string::npos;
    if (!isDigit)
        throw std::runtime_error("409 Conflict"); //response
    
    size_t size = std::atoi(lenght.c_str());
    if (size > this->getMaxLength())
        throw std::runtime_error("413 Payload Too Large"); //response

    this->_bodyLength = size;
}

/** 
 * **************************
 * SECTION - GETTERS/SETTERS
 * **************************
*/

std::string Request::getURL() const {return (this->_url);}
std::string Request::getMethod() const {return (this->_method);}
std::string Request::getPayload() const {return (this->_payload);}
size_t  Request::getBodyLength() const {return (this->_bodyLength);}
size_t Request::getMaxLength() const {return (this->_maxLength);}
std::map<std::string, std::string> Request::getHeadData() const {return (this->_headerData);}

void   Request::setMaxLength(size_t length){this->_maxLength = length;}


