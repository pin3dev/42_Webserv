/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Manager.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pin3dev <pinedev@outlook.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 19:04:31 by pin3dev           #+#    #+#             */
/*   Updated: 2024/05/16 23:54:08 by pin3dev          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Manager.hpp"

Manager:: Manager(pollfd &pollfd) : REFpollfd(pollfd), REFserver(NULL) , REFconnect(NULL), 
HEADpollfd(NULL), HEADserver(NULL), HEADconnect(NULL)
{
	write(1, "\tHI DO CONSTRUCTORMANAGER()\n", 29);
	this->_type = "";
	this->_msgError = "";
	this->_IDpseudo = 0;
	this->IDconnect = 0;
	this->IDserver = 0;
	this->IDpollfd = 0;
}
Manager::~Manager()
{
	this->_type.clear();
	this->_type = "";
	this->_msgError.clear();
	this->_msgError = "";
	this->_IDpseudo = 0;
	this->IDconnect = 0;
	this->IDserver = 0;
	this->IDpollfd = 0;
	this->REFserver = NULL;
	this->REFconnect = NULL;
	this->HEADserver = NULL;
	this->HEADconnect = NULL;
	this->HEADpollfd = NULL;
}


/* void Manager::clearManager()
{
	this->_type.clear();
	this->_type = "";
	this->_msgError.clear();
	this->_msgError = "";
	this->_IDpseudo = 0;
	this->IDconnect = 0;
	this->IDserver = 0;
	this->IDpollfd = 0;
	this->REFserver = NULL;
	this->REFconnect = NULL;
	this->HEADserver = NULL;
	this->HEADconnect = NULL;
	this->HEADpollfd = NULL;
}

void Manager::updateManager(pollfd &pollfd)
{
	this->REFpollfd = pollfd;
} */

/** 
 * **************************
 * SECTION - SETTERS
 * **************************
*/

/* void Manager::_setAutoError()
{
	short mode = this->getPollMode();
	switch (mode)
	{
		case POLLERR:
			this->_msgError = "ERR_POLLERR";
			break;
		case POLLHUP:
			this->_msgError = "ERR_POLLHUP";
			break;
		case POLLNVAL:
			this->_msgError = "ERR_POLLNVAL";
			break;
		default:
			break;
	}
} */

void Manager::setType(int DSAmount, int IDpollfd)
{
	write(1, "\tHI DO SETTYPE()\n", 18);
	//std::cout << "_DSAmount: " << DSAmount << std::endl;
	//std::cout << "IDpollfd: " << IDpollfd << std::endl;
    this->_IDpseudo = IDpollfd - DSAmount;
	//std::cout << "IDpseudo: " << this->_IDpseudo << std::endl; // ** com size_t tava dando erro no calculo
    if (this->_IDpseudo < 0)
		this->_type = "SERVER";
	else
		this->_type = "CONNECTION";
    
	this->IDpollfd = IDpollfd;
	//this->_setAutoError(); 
}

void Manager::setRefs(std::vector<pollfd> &pollfds, std::vector<Server>& servers, std::vector<Connect>& connects)
{
	write(1, "\tHI DO SETREFS()\n", 18);
	this->HEADpollfd = &pollfds;
	this->HEADserver = &servers;
	this->HEADconnect = &connects;
	if (this->getType() == "SERVER")
		this->_setServerAddr(servers);
	else if (this->getType() == "CONNECTION")
		this->_setConnectAddr(servers, connects);
	//_printManager(*this);
	//std::cout << "IDserver: " << this->IDserver << std::endl;
	//std::cout << "IDconnect: " << this->IDconnect << std::endl;
}

void Manager::_setServerAddr(std::vector<Server>& servers)
{
	write(1, "\tHI DO SETSERVERADDR()\n", 24);
	std::vector<Server>::iterator it = servers.begin();
	for (; it != servers.end(); it++)
	{
		if (it->getSocket() == this->getSocket())
		{
			this->REFserver = &(*it);
			this->IDserver = it - servers.begin();
			return ;
		}
	}
}

void Manager::_setConnectAddr(std::vector<Server>& servers, std::vector<Connect>& connect)
{
	write(1, "\tHI DO SETCONNECTADDR()\n", 25);
	this->REFserver = &servers[0];
	this->IDserver = 0;
	this->REFconnect = &connect[this->_IDpseudo];
	this->IDconnect = this->_IDpseudo;
}

void Manager::_setPollfdOfConnect(int connectSocket)
{
	write(1, "\tHI DO SETPOLLOFCONNECT()\n", 27);
	pollfd newPoll = {0, 0, 0};
	newPoll.fd = connectSocket;
	newPoll.revents = 0;
	newPoll.events = POLLIN | POLLOUT;
	std::cout << "\n\nNEWPOLL CONNECT CREATED\n\n";
	this->HEADpollfd->push_back(newPoll);
}
void Manager::_setNewConnect(int connectSocket)
{
	write(1, "\tHI DO SETNEWCONNECT()\n ", 25);
	this->HEADconnect->push_back(Connect(*this->getServer(), connectSocket));
}
void Manager::_rmPollfdOfConnect()
{
	write(1, "\tHI DO RMPOLLOFCONNECT()\n", 26);
	//std::vector<pollfd>::iterator it = this->HEADpollfd->begin();
	//this->HEADpollfd->erase(it + this->getID_P());
	this->REFpollfd.fd = -1;
}
void Manager::_rmConnect()
{
	write(1, "\tHI DO RMCONNECT()\n", 20);
    if (this->getID_C() >= 0 && this->getID_C() < (int)this->HEADconnect->size())
        this->HEADconnect->erase(this->HEADconnect->begin() + this->getID_C());
	//std::vector<Connect>::iterator it = this->HEADconnect->begin();
	//this->HEADconnect->erase(it + (size_t)this->getID_C());
}

/** 
 * **************************
 * SECTION - GETTERS & SETTERS
 * **************************
*/


std::string Manager::getType() const {return this->_type;}
std::string Manager::getErrorMsg() const {return this->_msgError;}

short Manager::getMode() const {return (this->REFpollfd.revents);}
int Manager::getSocket() const {return (this->REFpollfd.fd);}

int Manager::getID_P() const {return (this->IDpollfd);}
int Manager::getID_S() const {return (this->IDpollfd);}
int Manager::getID_C() const {return (this->IDpollfd);}

Connect *Manager::getConnect() const {return (this->REFconnect);}
Server *Manager::getServer() const {return (this->REFserver);}
pollfd &Manager::getPollfd() const {return (this->REFpollfd);}

void Manager::setSocket(int newFd) {this->REFpollfd.fd = newFd;}


/** 
 * **************************
 * SECTION - UTILS
 * **************************
*/

void _printManager(Manager &manager)
{
	std::cout << "------------------\n";
	std::cout << "MANAGER INFO\n";
	std::cout << "------------------\n";
	std::cout << "- fd: " << manager.getSocket() << std::endl;
	std::cout << "- type: " << manager.getType() << std::endl;
	std::cout << "- revents: ";
	if (manager.getMode() & POLLIN)
		std::cout << "POLLIN";
	if (manager.getMode() & POLLOUT)
		std::cout << " POLLOUT\n";
	if (manager.getMode() & POLLERR)
		std::cout << " POLLERR\n";
	if (manager.getMode() & POLLHUP)
		std::cout << " POLLHUP\n";
	if (manager.getMode()& POLLNVAL)
		std::cout << " POLLNVAL\n";
	if (manager.getMode() == 0)
		std::cout << "NO REVENTS\n";
}

/** 
 * **************************
 * SECTION - RUN MANAGER
 * **************************
*/

//** MODIFICAR FUNÇÃO PARA QUE O RUNPOLLING SEJA MULTIPLEXING
void  Manager::runManager()
{
	write(1, "\n\tHI DO RUNMANAGER()\n", 21);
	if (this->getMode() & POLLIN)
	{
		if (this->getType() == "SERVER")
			this->_createConnect();
		else if (this->getType() == "CONNECTION")
			this->_readRequest();
	}
	if (this->getMode() & POLLOUT)
	{
		if (this->getType() == "CONNECTION")
			this->_response();
	}
	if (this->getMode() & (POLLERR | POLLHUP | POLLNVAL))
		_closeConnect();
}

void Manager::_createConnect()
{
	write(1, "\tHI DO CREATECONNECT()\n", 24);
	int newConnect_fd = accept(this->REFserver->getSocket(), NULL, NULL);
	if (newConnect_fd < 0)
		throw std::runtime_error("Error: accept() failed\n");
	
	fcntl(newConnect_fd, F_SETFL, O_NONBLOCK);
	
	this->_setNewConnect(newConnect_fd);
	this->_setPollfdOfConnect(newConnect_fd);
}

void Manager::_closeConnect()
{
	write(1, "\tHI DO CLOSECONNECT()\n", 23);
	
	close(this->getSocket());

	this->_rmPollfdOfConnect();
	this->_rmConnect();
	//this->setSocket(-1);
}

void Manager::_readRequest()
{
	write(1, "\tHI DO READREQUEST()\n", 22);
/* 	char buffer[1024];
	ssize_t bytes_read = read(this->REFconnect->getSocket(), buffer, sizeof(buffer) - 1);
	if (bytes_read > 0) {
		buffer[bytes_read] = '\0'; // Null-terminate o buffer
		std::cout << "Requisição recebida: " << buffer << std::endl;
		this->REFconnect->setRequest(buffer); // Assumindo que Connect tem um método setRequest
	} else if (bytes_read == 0) {
		// Conexão fechada pelo cliente
		close(this->REFconnect->getSocket());
		this->REFconnect->setSocket(-1); // Assumindo que -1 significa desconectado
	} else {
		std::cerr << "Erro ao ler da conexão" << std::endl;
	} */

	char	buffer[2048] = {0};
	int		bytes = recv(this->getSocket(), buffer, 2048, 0);
	write(1, "\n", 1);
	write(1, buffer, bytes);
	write(1, "\n", 1);
	if (bytes > 0)
		this->getConnect()->appendToRequest(buffer, bytes);
	else
		this->_closeConnect();
}

void Manager::_response()
{
	write(1, "\tHI DO RESPONSE()\n", 19);
/* 	// Enviar a resposta ao cliente
	std::string response = this->REFconnect->getResponse(); // Assumindo que Connect tem um método getResponse
	ssize_t bytes_written = write(this->REFconnect->getSocket(), response.c_str(), response.size());
	if (bytes_written < 0) {
		std::cerr << "Erro ao enviar resposta" << std::endl;
	} else {
		// Sucesso ao enviar a resposta
		std::cout << "Resposta enviada: " << response << std::endl;
	} */
	std::string response = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: 12\n\nHello World!";
	std::cout << "SENDING RESPONSE" << std::endl;
	write(this->getSocket(), response.c_str(), response.length());
	this->_closeConnect();
}
