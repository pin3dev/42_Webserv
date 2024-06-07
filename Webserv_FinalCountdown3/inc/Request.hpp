/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pin3dev <pinedev@outlook.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 19:11:34 by pin3dev           #+#    #+#             */
/*   Updated: 2024/06/07 10:39:29 by pin3dev          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

//GENERAL INCLUDES
#include <exception>
#include <algorithm>   // std::find
#include <iostream>
#include <string>
#include <vector>
#include <map>

//ESPECIFIC INCLUDES
#include <sstream>
#include <cstring>

//CUSTOM INCLUDES

class Request
{
    private:
        static std::string const _generalAllowedMethods[3];
        static std::string const _HTTPunsupported[3];
        
        std::string	_request;

		std::string _url;
		std::string	_method;
        std::string _host;
		std::string	_payload;
		size_t		_bodyLength;
        size_t      _maxLength; //**CONFIGURADO NO CONSTRUCTOR DE CONNECT
        bool		_readyToResponse;
		std::map<std::string, std::string>	_headerData;
        
        void    _setMethodAndURL(std::stringstream &ss);
        void    _setHeaders(std::stringstream &ss);
        void    _setHost();
        void    _setPayload(std::stringstream &ss);
        void    _setBodyLength();

        bool    _isAt(std::string str);

    
    public:
        Request();
        ~Request();

        void	toAppend(char const *buffer, size_t size);
        void    checkStatusRequest();

        std::string getRequest() const;
        std::string getURL() const;
        std::string getMethod() const;
        std::string getHost() const;
        std::string getPayload() const;
        size_t  getBodyLength() const;
        size_t  getMaxLength() const;
        std::map<std::string, std::string> getHeadData() const;
        bool	getFlagToResponse() const;


        void    setMaxLength(size_t length);
        void    setReadyToResponse(bool status);
};
