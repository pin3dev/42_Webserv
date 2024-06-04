/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Manager.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pin3dev <pinedev@outlook.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 19:04:31 by pin3dev           #+#    #+#             */
/*   Updated: 2024/06/03 21:16:17 by pin3dev          ###   ########.fr       */
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
	//write(1, "\tHI DO CONSTRUCTORMANAGER()\n", 29); //TO DEBUG
	this->_type = "";
	this->_IDpseudo = 0;
	this->IDconnect = 0;
}
Manager::~Manager()
{
	this->_type.clear();
	this->_type = "";
	this->_IDpseudo = 0;
	this->IDconnect = 0;
	this->REFserver = NULL;
	this->REFconnect = NULL;
	this->HEADserver = NULL;
	this->HEADconnect = NULL;
	this->HEADpollfd = NULL;
}


/** 
 * **************************
 * SECTION - SETTERS
 * **************************
*/


void Manager::setType(int DSAmount, int IDpollfd)
{
	//write(1, "\tHI DO SETTYPE()\n", 18); //TO DEBUG
	//std::cout << "_DSAmount: " << DSAmount << std::endl; //TO DEBUG
	//std::cout << "IDpollfd: " << IDpollfd << std::endl; //TO DEBUG
    this->_IDpseudo = IDpollfd - DSAmount;
	//std::cout << "IDpseudo: " << this->_IDpseudo << std::endl; //TO DEBUG
    if (this->_IDpseudo < 0)
		this->_type = "SERVER";
	else
		this->_type = "CONNECTION";
}

void Manager::setRefs(std::vector<pollfd> &pollfds, std::vector<Server>& servers, std::vector<Connect>& connects)
{
	//write(1, "\tHI DO SETREFS()\n", 18); //TO DEBUG
	this->HEADpollfd = &pollfds;
	this->HEADserver = &servers;
	this->HEADconnect = &connects;
	if (this->getType() == "SERVER")
		this->_setServerAddr(servers);
	else if (this->getType() == "CONNECTION")
		this->_setConnectAddr(servers, connects);
	//_printManager(*this); //TO DEBUG
	//std::cout << "IDserver: " << this->IDserver << std::endl; //TO DEBUG
	//std::cout << "IDconnect: " << this->IDconnect << std::endl; //TO DEBUG
}

void Manager::_setServerAddr(std::vector<Server>& servers)
{
	//write(1, "\tHI DO SETSERVERADDR()\n", 24); //TO DEBUG
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
	//write(1, "\tHI DO SETCONNECTADDR()\n", 25); //TO DEBUG
	this->REFserver = &servers[0];
	this->REFconnect = &connect[this->_IDpseudo];
	this->IDconnect = this->_IDpseudo;
}

void Manager::_setPollfdOfConnect(int connectSocket)
{
	//write(1, "\tHI DO SETPOLLOFCONNECT()\n", 27); //TO DEBUG
	pollfd newPoll = {0, 0, 0};
	newPoll.fd = connectSocket;
	newPoll.revents = 0;
	newPoll.events = POLLIN | POLLOUT;
	std::cout << "\n\nNEWPOLL CONNECT CREATED\n\n"; //TO DEBUG
	this->HEADpollfd->push_back(newPoll);
}
void Manager::_setNewConnect(int connectSocket)
{
	//write(1, "\tHI DO SETNEWCONNECT()\n ", 25); //TO DEBUG
	this->HEADconnect->push_back(Connect(*this->REFserver, connectSocket));
}
void Manager::_rmPollfdOfConnect()
{
	//write(1, "\tHI DO RMPOLLOFCONNECT()\n", 26); //TO DEBUG
	this->REFpollfd.fd = -1;
}
void Manager::_rmConnect() 
{
	//write(1, "\tHI DO RMCONNECT()\n", 20); //TO DEBUG //**tinha mudado esse ID
	std::cout << "IDconnect: " << this->getID_C() << std::endl; //TO DEBUG
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
	//write(1, "\n\tHI DO RUNMANAGER()\n", 21); //TO DEBUG
	if (this->getPollMode() & POLLIN)
	{
		//std::cout << "entrei pollin" << std::endl;
		if (this->getType() == "SERVER")
			this->_createConnect();
		else if (this->getType() == "CONNECTION")
			this->_storeRequest();
	}
	if (this->getPollMode() & POLLOUT)
	{
		//std::cout << "entrei pollout" << std::endl;
		if (this->getType() == "CONNECTION")
			this->_askToresponse();
	}
	if (this->getPollMode() & (POLLERR | POLLHUP | POLLNVAL))
	{
		//std::cout << "entrei erro" << std::endl;
		_closeConnect(); //**ADICIONAR UM TRATAMENTO DE ERRO ESPECIFICO QUE DEPOIS CHAME POR CLOSECONNECT
	}
}

void Manager::_createConnect()
{
	//write(1, "\tHI DO CREATECONNECT()\n", 24); //TO DEBUG
	int newConnect_fd = accept(this->REFserver->getSocket(), NULL, NULL);
	if (newConnect_fd < 0)
		throw std::runtime_error("Error: accept() failed\n");
	
	fcntl(newConnect_fd, F_SETFL, O_NONBLOCK);
	
	this->_setNewConnect(newConnect_fd);
	this->_setPollfdOfConnect(newConnect_fd);
}

void Manager::_closeConnect()
{
	//write(1, "\tHI DO CLOSECONNECT()\n", 23); //TO DEBUG
	
	close(this->getPollSocket());

	this->_rmPollfdOfConnect();
	this->_rmConnect();
}


//**TALVEZ SEJA AQUI A INTEGRAÇÃO DO EOF PARA CHUNKS
void Manager::_storeRequest()
{
	//write(1, "\tHI DO STOREREQUEST()\n", 22); //TO DEBUG

	char	buffer[2048] = {0};
	int		bytes = recv(this->getPollSocket(), buffer, 2048, 0); //VER MAIS SOBRE MASCARAS DE FLAGS
	
	//write(1, "\n", 1); //TO DEBUG
	//write(1, buffer, bytes); //TO DEBUG
	//print_invisible(buffer); //TO DEBUG
	//write(1, "\n", 1); //TO DEBUG
	
	if (bytes > 0)
		this->REFconnect->appendToRequest(buffer, bytes);
	else //ISSO AQUI SO VAI SER EXECUTADO SE DER ERRO NA LEITURA DO REQUEST ou SE A CONEXAO FOR FECHADA PELO CLIENTE
		this->_closeConnect();
}

//**TALVEZ SEJA AQUI A INTEGRAÇÃO DO EOF PARA CHUNKS
void Manager::_askToresponse()
{
	//write(1, "\tHI DO ASKTORESPONSE()\n", 19); //TO DEBUG

	//**VERIFICAR SE A CONEXÃO NÃO ESTÁ EM TIMEOUT isTimeout(), SE SIM FECHA E RETORNA
	//**VERIFICAR SE A CONEXÃO ESTÁ PRONTA PARA ENVIAR RESPOSTA isReadyToSend(), SE NÃO RETORNA
	//**VEFICAR O SERVIDOR NON DEFAULT
	if (this->REFconnect->getRequest().getFlagToResponse())
	{
		this->REFconnect->runRequest(*this->HEADserver);
		this->_closeConnect();
	}
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


