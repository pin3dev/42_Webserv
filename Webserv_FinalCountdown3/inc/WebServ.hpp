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


class WebServ
{
	private:
		static int  _DSAmount; // STATIC
		static bool gShutDown; // STATIC
		std::vector<pollfd> _polling; //ADMINISTRADO INTERNAMENTE
		std::vector<Server> _vServers; //ATRIBUIDO POR CONSTRUCTOR
		std::vector<Connect> _connects; //ADMINISTRADO INTERNAMENTE
	
	//METODOS PRIVADOS
		void _countAndCheckVirtualServers();
		void _configVirtualServers();
		void _setPollfdOfServer(int socket);
		
		void _runPolling();
		void _verifyPolling();
		void _cleaningPolling();
		void _cleanWebserv();


	//UTILS
		bool __isValidFD(int fd); //**COLOCAR NO UTILS
		
	public:
	//METODOS PUBLICOS
		static void signalHandler(int signum);
		
		WebServ(std::vector<Server> servers);
		~WebServ();

		void runWebServ();

		static void debugMsg(std::string msg); //**TO DELETE LATER
		void printServerVec(); //**TO DELETE LATER
		void printPollingVec(std::string status); //**TO DELETE LATER
		
	//GETTERS
		std::vector<Server> &getVecServers();
		std::vector<Connect> &getVecConnects();
		std::vector<pollfd> &getVecPolling();
		int getDSAmount();
};
