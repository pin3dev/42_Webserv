/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Configurator.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pin3dev <pinedev@outlook.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 20:42:07 by pin3dev           #+#    #+#             */
/*   Updated: 2024/05/27 19:51:20 by pin3dev          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <stdexcept> //std::runtime_error

#include <sys/types.h> //socket(), bind(), listen(), accept(), setsockopt()
#include <sys/socket.h> //socket(), bind(), listen(), accept(), setsockopt()
#include <netdb.h> //addrinfo struct, getaddrinfo(), freeaddrinfo()
#include <cstring> //memset()
#include <unistd.h> //close() 
#include "Server.hpp"

class Configurator
{
    private:
        const static int    _maxConnections;
        struct addrinfo     _settings; 
        struct addrinfo*    _tmpAddr;
        Server&             _server;

        void _setReuse();
		void _getHostAddr();
		void _assignAddr();
		void _setPassive();
        void _createSocket();
        
    public:
        
        Configurator(Server &server);
        ~Configurator();

        void freeTmpAddr();
        void setSocketToVServer();
};
