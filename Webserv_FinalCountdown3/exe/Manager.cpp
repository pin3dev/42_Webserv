/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Manager.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pin3dev <pinedev@outlook.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 19:04:31 by pin3dev           #+#    #+#             */
/*   Updated: 2024/06/07 16:03:59 by pin3dev          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Manager.hpp"
#include "Utils.hpp" //**TO DEBUG

/** 
 * **************************
 * SECTION - CONSTRUCTORS
 * **************************
*/

Manager:: Manager(pollfd &pollfd) : REFpollfd(pollfd), REFserver(NULL) , REFconnect(NULL), 
HEADpollfd(NULL), HEADserver(NULL), HEADconnect(NULL)
{
	this->_type = "";
	this->_IDpseudo = 0;
	this->IDconnect = 0;
}
Manager::~Manager()
{}


/** 
 * **************************
 * SECTION - SETTERS
 * **************************
*/


void Manager::setType(int DSAmount, int IDpollfd)
{
    this->_IDpseudo = IDpollfd - DSAmount;
    if (this->_IDpseudo < 0)
		this->_type = "SERVER";
	else
		this->_type = "CONNECT";
}

void Manager::setRefs(std::vector<pollfd> &pollfds, std::vector<Server>& servers, std::vector<Connect>& connects)
{
	this->HEADpollfd = &pollfds;
	this->HEADserver = &servers;
	this->HEADconnect = &connects;
	if (this->getType() == "SERVER")
		this->_setServerAddr(servers);
	else if (this->getType() == "CONNECT")
		this->_setConnectAddr(servers, connects);
}

void Manager::_setServerAddr(std::vector<Server>& servers)
{
	std::vector<Server>::iterator it = servers.begin();
	for (; it != servers.end(); it++)
	{
		if (it->getSocket() == this->getPollSocket())
		{
			this->REFserver = &(*it);
			break;
		}
	}
}

void Manager::_setConnectAddr(std::vector<Server>& servers, std::vector<Connect>& connect)
{
	this->REFserver = &servers[0];
	this->REFconnect = &connect[this->_IDpseudo];
	this->IDconnect = this->_IDpseudo;
}

void Manager::_setPollfdOfConnect(int connectSocket)
{
	pollfd newPoll = {0, 0, 0};
	newPoll.fd = connectSocket;
	newPoll.revents = 0;
	newPoll.events = POLLIN | POLLOUT;
	this->HEADpollfd->push_back(newPoll);
}
void Manager::_setNewConnect(int connectSocket)
{
	this->HEADconnect->push_back(Connect(*this->REFserver, connectSocket));
}
/* void Manager::_rmPollfdOfConnect()
{
	this->REFpollfd.fd = -1;
} */
void Manager::_rmConnect() 
{
    if (this->getID_C() >= 0 && this->getID_C() < (int)this->HEADconnect->size())
       this->HEADconnect->erase(this->HEADconnect->begin() + this->IDconnect);
}


/** 
 * **************************
 * SECTION - RUN MANAGER
 * **************************
*/

void  Manager::runManager()
{
	if (this->getPollMode() & POLLIN)
	{
		if (this->getType() == "SERVER")
			this->_createConnect();
		else if (this->getType() == "CONNECT")
			this->_storeRequest();
	}
	if (this->getPollMode() & POLLOUT)
	{
		if (this->getType() == "CONNECT")
			this->_askToresponse();
	}
	if (this->getPollMode() & (POLLERR | POLLHUP | POLLNVAL))
	{
		_closeConnect(); //**ADICIONAR UM TRATAMENTO DE ERRO ESPECIFICO QUE DEPOIS CHAME POR CLOSECONNECT
	}
}

void Manager::_createConnect()
{
	int newConnect_fd = accept(this->REFserver->getSocket(), NULL, NULL);
	if (newConnect_fd < 0)
		throw std::runtime_error("Error: accept() failed\n");
	
	fcntl(newConnect_fd, F_SETFL, O_NONBLOCK);
	
	this->_setNewConnect(newConnect_fd);
	this->_setPollfdOfConnect(newConnect_fd);
}

void Manager::_closeConnect()
{
	close(this->getPollSocket()); //**adicionar ao destructor do connect

	//this->_rmPollfdOfConnect();
	this->setSocket(-1);
	this->_rmConnect();
}


void Manager::_storeRequest()
{
	char	buffer[2048] = {0};
	int		bytes = recv(this->getPollSocket(), buffer, 2048, 0); //**VER MAIS SOBRE MASCARAS DE FLAGS
	
	std::string meubuffer = buffer;
	std::cout << "REQUEST LIDO: " << meubuffer << std::endl;
	if (bytes > 0)
		this->REFconnect->appendToRequest(buffer, bytes);
	else
		this->_closeConnect();
}

void Manager::_askToresponse()
{
	std::cout << "TENTANDO RESPONDER...\n";
	if (this->REFconnect->isExpired())
	{
		std::cout << "TIMEOUT\n";
		this->_closeConnect();
		return;
	}
	if (!this->REFconnect->getRequest().getFlagToResponse())
		return;
	this->REFconnect->runRequest(*this->HEADserver);
	if (!this->REFconnect->getRequest().getFlagToResponse())
	{
		std::cout << "TENTEI ARMAZENAR MAS VAI TER QUE TENTAR DE NOVO\n";
		return;
	}
	this->_closeConnect();
}


/** 
 * **************************
 * SECTION - GETTERS & SETTERS
 * **************************
*/


std::string Manager::getType() const {return this->_type;}
int Manager::getID_C() const {return (this->IDconnect);}

short Manager::getPollMode() const {return (this->REFpollfd.revents);}
int Manager::getPollSocket() const {return (this->REFpollfd.fd);}

void Manager::setSocket(int newFd) {this->REFpollfd.fd = newFd;}


