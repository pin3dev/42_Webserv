/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pin3dev <pinedev@outlook.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 19:10:47 by pin3dev           #+#    #+#             */
/*   Updated: 2024/06/11 17:25:43 by pin3dev          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include "Utils.hpp"

std::string const Request::_generalAllowedMethods[3] = {"GET", "POST", "DELETE"};
std::string const Request::_HTTPunsupported[3] = {"HTTP/0.9\r", "HTTP/1.0\r", "HTTP/2.0\r"};

/** 
 * **************************************
 * SECTION - CONSTRUCTORS & DESTRUCTORS
 * **************************************
*/

Request::Request() : _readyToResponse(false), _firstRead(true)
{
    this->_request.clear();
    this->_url.clear();
    this->_method.clear();
    this->_host.clear();
    this->_payload.clear();
    this->_bodyLength = 0;
    this->_headerData.clear();
    this->_maxLength = 0; 
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
    
    std::string partRequest(buffer, size);

    if (partRequest.find("\r\n") != std::string::npos)
        this->setReadyToResponse(true);
}

/** 
 * **************************
 * SECTION - SETTERS/CHECKERS
 * **************************
*/

void	Request::checkStatusRequest()
{
	std::stringstream	ss(this->_request);
    if (this->getFlagToFirstRead())
    {
        this->_setMethodAndURL(ss);
        this->_setHeaders(ss);
        this->_setHost();
    }
	if (this->_method == "POST")
        this->_setPayload(ss);
}

void Request::_setMethodAndURL(std::stringstream &ss)
{
    std::string		firstLine;
    std::getline(ss, firstLine);
    std::vector<std::string> method_URL_version = Utils::_split(firstLine, ' ');

    bool hasValidAmount = method_URL_version.size() == 3;
    bool hasAllowedMethod = Utils::_isAt(method_URL_version[0], this->_generalAllowedMethods);
    bool hasSensibleURL = method_URL_version[1].find("../") != std::string::npos && method_URL_version[1] == "..";
    bool hasValidURLSize = method_URL_version[1].length() <= 1024;
    bool hasUnsupportedHTTP = Utils::_isAt(method_URL_version[2], this->_HTTPunsupported);
    bool hasValidHTTP = std::strcmp(method_URL_version[2].c_str(), "HTTP/1.1\r") == 0;

    if (!hasValidAmount)
        throw std::runtime_error(Utils::_defaultErrorPages(400, "Erro na primeira linha do request."));
    if (!hasAllowedMethod)
        throw std::runtime_error(Utils::_defaultErrorPages(501, "Os metodos permitidos sao: GET, POST e DELETE."));
    if (hasSensibleURL)
        throw std::runtime_error(Utils::_defaultErrorPages(400, "O URL requisitado tem um caminho sensivel como: .. ou ../"));
    if (!hasValidURLSize)
        throw std::runtime_error(Utils::_defaultErrorPages(414, "O URL requisitado passa do tamanho limite seguro."));
    if (hasUnsupportedHTTP)
        throw std::runtime_error(Utils::_defaultErrorPages(505, "A unica versao suportada para esse servidor e o HTTP 1.1"));
    if (!hasValidHTTP)
        throw std::runtime_error(Utils::_defaultErrorPages(400, "Versao HTTP errada."));

    this->_method = method_URL_version[0];
    this->_url = Utils::_decoder(method_URL_version[1]);
}

void	Request::_setHeaders(std::stringstream &ss)
{
	std::string curLine;
	while (std::getline(ss, curLine))
	{
		if (curLine == "\r" || curLine == "\r\n" || curLine.empty()) 
        	break;
		if (curLine.find(':') != std::string::npos) 
		{
			std::string key = curLine.substr(0, curLine.find(':'));
			std::string value = curLine.substr(curLine.find(':') + 1, curLine.find('\n'));
			Utils::_trim(key, " \t\r\v\f");
			Utils::_trim(value, " \t\r\v\f");

			if (value.length() == 0 || key.length() == 0)
				throw std::runtime_error(Utils::_defaultErrorPages(406, "Header do request foge a estrutura key:value"));
			this->_headerData[key] = value;
		}
	}
}

void	Request::_setHost()
{
    std::string hostAndPort = this->_headerData["Host"];
    size_t pos = hostAndPort.find(':');
    if (pos != std::string::npos)
        this->_host = hostAndPort.substr(0, pos);
    else
        this->_host = hostAndPort;
}

void    Request::_setPayload(std::stringstream &ss)
{
    this->_setBodyLength();

    if (!this->getFlagToFirstRead())
    {
        std::string line;
        while (std::getline(ss, line))
        {
            if (line == "\r")
                break;
        }
    }
    
    std::streampos pos = ss.tellg();
/*     if (pos == -1)
        throw std::runtime_error("Nothing else to read. No Payload!\n"); */
	std::stringstream binarySs(ss.str(), std::stringstream::in | std::stringstream::binary);
	binarySs.seekg(pos);

    std::string tempPayload(this->getBodyLength(), '\0');
    binarySs.read(&tempPayload[0], this->getBodyLength());

    std::streamsize bytesRead = binarySs.gcount();
    if (bytesRead < static_cast<std::streamsize>(this->getBodyLength()))
    {
        this->setReadyToResponse(false);
        this->setFirstRead(false);
        return ;
    }
    if (this->_headerData["Content-Type"] == "plain/text")
        this->_payload.assign(tempPayload.begin(), tempPayload.end());
	else
    {
        this->_payload.resize(this->getBodyLength()); 
        binarySs.read(&this->_payload[0], this->getBodyLength());
    }
}

void    Request::_setBodyLength()
{
    bool hasContentLength = this->_headerData.find("Content-Length") != this->_headerData.end();
    if (!hasContentLength)
        throw std::runtime_error(Utils::_defaultErrorPages(411, "Request sem Content-Length definido"));
    
    std::string const lenght = this->_headerData["Content-Length"];
    
    bool isDigit = lenght.find_first_not_of("0123456789") == std::string::npos;
    if (!isDigit)
        throw std::runtime_error(Utils::_defaultErrorPages(409, "Request com Content-Length mal definido")); //response
    
    size_t size = std::atoi(lenght.c_str());
    if (size > this->getMaxLength())
        throw std::runtime_error(Utils::_defaultErrorPages(413, "Request com Payload maior que o definido pelo Content-Length")); //response

    this->_bodyLength = size;
}

/** 
 * **************************
 * SECTION - GETTERS/SETTERS
 * **************************
*/

std::string Request::getRequest() const {return (this->_request);}
std::string Request::getURL() const {return (this->_url);}
std::string Request::getMethod() const {return (this->_method);}
std::string Request::getHost() const {return (this->_host);}
std::string Request::getPayload() const {return (this->_payload);}
size_t  Request::getBodyLength() const {return (this->_bodyLength);}
size_t Request::getMaxLength() const {return (this->_maxLength);}
std::map<std::string, std::string> Request::getHeadData() const {return (this->_headerData);}
bool	Request::getFlagToResponse() const{return (this->_readyToResponse);}
bool	Request::getFlagToFirstRead() const{return (this->_firstRead);}


void    Request::setFirstRead(bool status){this->_firstRead = status;}
void   Request::setMaxLength(size_t length){this->_maxLength = length;}
void    Request::setReadyToResponse(bool status){this->_readyToResponse = status;}




