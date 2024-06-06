/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Manager.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pin3dev <pinedev@outlook.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 19:05:17 by pin3dev           #+#    #+#             */
/*   Updated: 2024/06/03 20:40:20 by pin3dev          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

//GENERAL INCLUDES
#include <iostream>
#include <string>
#include <poll.h> //pollfd
//ESPECIFIC INCLUDES
#include <sys/types.h> //socket(), bind(), listen(), accept(), setsockopt()
#include <sys/socket.h> //socket(), bind(), listen(), accept(), setsockopt()
#include <fcntl.h> //fcntl()
#include <unistd.h> //close(), write(), read(), fork(), execve(), dup2(), pipe()
//CUSTOM INCLUDES
#include "Server.hpp"
#include "Connect.hpp"

class Manager
{
	private:
		std::string _type;
		int		_IDpseudo;
		
		pollfd 		&REFpollfd;
		Server		*REFserver;
		Connect		*REFconnect;

		int 		IDconnect;

		std::vector<pollfd> *HEADpollfd;
		std::vector<Server> *HEADserver;
		std::vector<Connect> *HEADconnect;
		
		void _setServerAddr(std::vector<Server>& servers);
		void _setConnectAddr(std::vector<Server>& servers, std::vector<Connect>& connect);

		//Setters For WebServ
		void _setPollfdOfConnect(int connectSocket);
		void _setNewConnect(int connectSocket);
		void _rmPollfdOfConnect();
		void _rmConnect();
		
		//Run Manager
		void	_createConnect();
		void	_storeRequest();
		void 	_askToresponse();
		void	_closeConnect();
		
	public:
		Manager(pollfd &poll);
		~Manager();
		
		//functions used in Service
		void setType(int DSAmount, int IDpollfd);
		void setRefs(std::vector<pollfd> &pollfds, std::vector<Server>& servers, std::vector<Connect>& connects);
		
		void runManager();

		
		//Getters
		std::string getType() const;
		int getID_C() const;
		
		short getPollMode() const;
		int getPollSocket() const;

		//Setters
		void setSocket(int socket);
};

void _printManager(Manager &manager);
