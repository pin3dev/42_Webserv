
#pragma once

#include <stdexcept> 

#include <sys/types.h> 
#include <sys/socket.h> 
#include <netdb.h> 
#include <cstring> 
#include <unistd.h>  
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
