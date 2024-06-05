/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pin3dev <pinedev@outlook.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 19:11:34 by pin3dev           #+#    #+#             */
/*   Updated: 2024/06/05 20:12:32 by pin3dev          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once


//GENERAL INCLUDES
#include <iostream>
#include <string>
#include <map>
#include <vector>

//ESPECIFIC INCLUDES
//#include <sys/stat.h>
//#include <dirent.h> //to dir
//#include <fstream>
#include <unistd.h>
#include <sstream>
#include <algorithm>

//CUSTOM INCLUDES
//#include "Connect.hpp"
#include "Location.hpp"
#include "Utils.hpp"

class Response
{
    private:
        int         _urlLoopCount;
        int         _connect_fd;  //**CONFIGURADO NO CONSTRUCTOR DE CONNECT
        
        std::map<std::string, location_t> _servLoc; //**CONFIGURADO NO CONSTRUCTOR DE CONNECT
        std::map<std::string, std::string> _headerData;
/*         bool    _isCGI;
        bool    _isAutoIndex; */
        
        
        bool	_isRightLocation(std::string locName, std::string &url);
        bool 	_isMethodAllowed(std::vector<std::string> const &methods, std::string &method);
        void    _setConfigByLocation(std::map<std::string, location_t>::const_iterator &rightLocation, std::string &url, std::string &method, std::map<std::string, location_t> const &servLoc);
        std::map<std::string, location_t>::const_iterator _findRightLocation(std::string &url);


        void    _setLocationFlag(std::string &url);
        std::map<std::string, location_t>::const_iterator _searchForLocation(std::string &url);

    public:
        Response();
        ~Response();

        void processRequest(const std::string &url, const std::string &method, const std::string &root);
        
        void	runResponse(std::string &url, std::string method);
        void	initResponse(std::string &url);
        
        void    setServLoc(std::map<std::string, location_t> const &servLoc);
        int     getConnectFd();
        
        void    setConnectFd(int connectSocket);
        void    setHeaderData(std::map<std::string, std::string> const &headerData);
};
