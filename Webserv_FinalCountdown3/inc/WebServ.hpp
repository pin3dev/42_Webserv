/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pin3dev <pinedev@outlook.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 17:52:33 by pin3dev           #+#    #+#             */
/*   Updated: 2024/06/11 18:17:48 by pin3dev          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream> //std::cout, std::endl
#include <vector> //std::vector
#include <string> //std::string

#include <csignal> //std::signal
#include <unistd.h> //SIGPIPE, SIG_IGN
#include <stdexcept> //std::runtime_error
#include <poll.h> //pollfd

#include "Server.hpp" //std::vector<Server>
#include "Configurator.hpp" //VServer used inside configVirtualServers()
#include "Manager.hpp" //Manager used inside _verifyPolling()
#include "Connect.hpp" //std::vector<Connect>

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
