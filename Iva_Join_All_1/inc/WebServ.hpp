/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pin3dev <pinedev@outlook.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 17:52:33 by pin3dev           #+#    #+#             */
/*   Updated: 2024/05/16 14:40:19 by pin3dev          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

//GENERAL INCLUDES
#include <iostream> //std::cout, std::endl
#include <vector> //std::vector
#include <string> //std::string

//ESPECIFIC INCLUDES
#include <csignal> //std::signal
#include <unistd.h> //SIGPIPE, SIG_IGN
#include <stdexcept> //std::runtime_error
#include <poll.h> //pollfd

//CUSTOM INCLUDES
#include "Server.hpp" //std::vector<Server>
#include "Configurator.hpp" //VServer used inside configVirtualServers()
#include "Manager.hpp" //Manager used inside _verifyPolling()
#include "Connect.hpp" //std::vector<Connect>

//DEFINES FOR POLLING
#define S 1
#define C 2

class WebServ
{
	private:
		static int  _DSAmount; // STATIC
		static bool gShutDown; // STATIC
		std::vector<pollfd> _polling; //ADMINISTRADOR INTERNAMENTE
		std::vector<Server> _vServers; //ATRIBUIDO POR CONSTRUCTOR
		std::vector<Connect> _connects; //ADMINISTRADOR INTERNAMENTE
	
	//METODOS PRIVADOS
		void _countAndCheckVirtualServers();
		void _configVirtualServers();
		void _setPollfdOfServer(int socket);
		void _runPolling();
		void _verifyPolling();
		void _cleaningPolling();
		

	//UTILS
		bool __isValidFD(int fd);
		
	public:
	//METODOS PUBLICOS
		static void signalHandler(int signum);
		static void debugMsg(std::string msg); //**TO DELETE LATER
		
		WebServ(std::vector<Server> servers);
		~WebServ();
		
		void runWebServ();

		void printServerVec(); //**TO DELETE LATER
		void printPollingVec(std::string status); //**TO DELETE LATER
		
	//GETTERS
		std::vector<Server> &getvServers();
		std::vector<Connect> &getConnects();
		std::vector<pollfd> &getPolling();
		int getDSAmount();
};
