
#pragma once

#include <iostream>
#include <string>
#include <poll.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <fcntl.h> 
#include <unistd.h> 
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
