
#pragma once

#include <iostream> 
#include <vector> 
#include <string> 

#include <csignal> 
#include <unistd.h> 
#include <stdexcept> 
#include <poll.h> 

#include "Server.hpp" 
#include "Configurator.hpp" 
#include "Manager.hpp" 
#include "Connect.hpp"

class WebServ
{
	private:
		static int  _DSAmount; 
		static bool gShutDown; 
		std::vector<pollfd> _polling; 
		std::vector<Server> _vServers; 
		std::vector<Connect> _connects; 
	
	//METODOS PRIVADOS
		void _countAndCheckVirtualServers();
		void _configVirtualServers();
		void _setPollfdOfServer(int socket);
		
		void _runPolling();
		void _verifyPolling();
		void _cleaningPolling();
		void _cleanWebserv();


	//UTILS
		bool __isValidFD(int fd); 
		
	public:
	//METODOS PUBLICOS
		static void signalHandler(int signum);
		
		WebServ(std::vector<Server> servers);
		~WebServ();

		void runWebServ();	
	//GETTERS
		std::vector<Server> &getVecServers();
		std::vector<Connect> &getVecConnects();
		std::vector<pollfd> &getVecPolling();
		int getDSAmount();
};
