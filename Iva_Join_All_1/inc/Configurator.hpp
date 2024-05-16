/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Configurator.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pin3dev <pinedev@outlook.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 20:42:07 by pin3dev           #+#    #+#             */
/*   Updated: 2024/05/15 23:06:34 by pin3dev          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

//GENERAL INCLUDES
#include <stdexcept> //std::runtime_error

//ESPECIFIC INCLUDES
#include <sys/types.h> //socket(), bind(), listen(), accept(), setsockopt()
#include <sys/socket.h> //socket(), bind(), listen(), accept(), setsockopt()
#include <netdb.h> //addrinfo struct, getaddrinfo(), freeaddrinfo()
#include <cstring> //memset()
#include <unistd.h> //close() // ** TESTAR - SUBSTITUIR?
#include "Server.hpp"

//CUSTOM INCLUDES
class Configurator
{
    private:
        const static int    _maxConnections;
        struct addrinfo     _settings; //** ANTES ERA STATIC E DEPOIS DE UM TEMPO DEU PROBLEMA
        struct addrinfo*    _tmpAddr;
        Server&             _server;

        void _setReuse();
		void _getHostAddr();
		void _assignAddr();
		void _setPassive();
        void _createSocket();
        //void _freeTmpAddr();
        
    public:
        
        Configurator(Server &server);
        ~Configurator();

        void freeTmpAddr();
        void setSocketToVServer();
};
