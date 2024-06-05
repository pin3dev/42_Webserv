/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pin3dev <pinedev@outlook.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 17:51:59 by pin3dev           #+#    #+#             */
/*   Updated: 2024/06/03 20:43:34 by pin3dev          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"

bool WebServ::gShutDown = false;
int  WebServ::_DSAmount = 0;

/** 
 * **************************
 * SECTION - MAIN METHODS
 * **************************
*/

WebServ::WebServ(std::vector<Server> servers) : _polling(), _vServers(servers), _connects()
{
	std::signal(SIGPIPE, SIG_IGN); //CONFIGURAÇÃO PARA IGNORAR SIGPIPE (PIPE QUEBRADO OU SOCKET FECHADO PELO SERVER)
	std::signal(SIGINT, signalHandler); //CONFIGURA SINAL PARA CTRL+C
	_countAndCheckVirtualServers();
	_configVirtualServers();
	
	printServerVec(); //TO DEBUG 
	//debugMsg("WebServ Ready!\n\r\n");  //TO DEBUG 
}
WebServ::~WebServ()
{
	this->_vServers.clear();
	this->_polling.clear();
	this->_connects.clear();
}

void WebServ::_configVirtualServers()
{
	std::vector<Server>::iterator it = this->_vServers.begin();
	for (; it != this->_vServers.end(); it++)
	{
		Configurator conf(*it);
		conf.setSocketToVServer();
		this->_setPollfdOfServer(it->getSocket());
	}
}

void WebServ::runWebServ()
{
	//printPollingVec("STARTING"); //TO DEBUG
	//write(1, "HI DO RUNWEBSERV()\n", 20); //TO DEBUG
	while (!gShutDown)
	{
		_runPolling();
		_verifyPolling();
		_cleaningPolling();
	}
}
//** TRANSFORMAR FUNÇÃO EM MULTIPLEXING
void WebServ::_verifyPolling()
{
	//write(1, "HI DO VERIFYPOLLING()\n", 23); //TO DEBUG
	//std::cout << "\n\n\tThis VECTOR POLLING size is: " << this->_polling.size() << std::endl; //TO DEBUG
	//std::cout << "\n\n\tThis VECTOR SERVER size is: " << this->_vServers.size() << std::endl; //TO DEBUG
	//std::cout << "\n\n\tThis VECTOR CONNECT size is: " << this->_connects.size() << std::endl; //TO DEBUG
	for (size_t i = 0; i < this->_polling.size(); i++)
	{
		//std::cout << "\n\n\tThis FD POLLING is: " << this->_polling[i].fd << std::endl; //TO DEBUG
		Manager manager(this->_polling[i]);
		manager.setType(_DSAmount, i);
		manager.setRefs(this->getVecPolling(), this->getVecServers(), this->getVecConnects());
		manager.runManager();
	}
}
 
void WebServ::_cleaningPolling()
{
	if ((int)this->_polling.size() == _DSAmount)
		return;
	//write(1, "HI DO CLEANINGPOLLING()\n", 25);
    for (std::vector<pollfd>::iterator it = this->_polling.begin(); it != this->_polling.end();)
    {
        if (it->fd == -1)
            it = this->_polling.erase(it);
        else
            ++it;
    }
}

void WebServ::_runPolling()
{
	//write(1, "HI DO RUNPOLLING()\n", 20);
	//printPollingVec("BEFORE POLL"); //TO DEBUG
	if (poll(this->_polling.data(), this->_polling.size(), 200) < 0 && this->gShutDown == false)
		throw std::runtime_error("Polling error!\n");
	//printPollingVec("AFTER POLL"); //TO DEBUG
}

/** 
 * **************************
 * SECTION - SETTERS
 * **************************
*/

void WebServ::_setPollfdOfServer(int socket)
{
	if (!this->__isValidFD(socket))
		return;

	pollfd newPoll = {0, 0, 0}; //MEMSET
	newPoll.fd = socket;
	newPoll.revents = 0;
	newPoll.events = POLLIN;
	//std::cout << "\n\nNEWPOLL SERVER CREATED\n\n";
	this->_polling.push_back(newPoll);
}

/** 
 * **************************
 * SECTION - GETTERS
 * **************************
*/

int WebServ::getDSAmount(){return (_DSAmount);}
std::vector<Server> &WebServ::getVecServers(){return (this->_vServers);}
std::vector<Connect> &WebServ::getVecConnects(){return (this->_connects);}
std::vector<pollfd> &WebServ::getVecPolling(){return (this->_polling);}

/** 
 * **************************
 * SECTION - UTILS METHODS
 * **************************
*/


/** 
 * **************************
 * SECTION - CHECKERS
 * **************************
*/

void WebServ::signalHandler(int signum)
{
	if (signum == SIGINT)
	{
		gShutDown = true;
		debugMsg("Keyboard interrupt received, exiting.\n");
	}
}

bool WebServ::__isValidFD(int fd)
{
	if (fd < 2) //** verificar se é 3 ou 2
		return false;
	return true;
}

//**ver se posso incluir isso em utils
void WebServ::_countAndCheckVirtualServers() 
{
	std::vector<Server>::iterator it = this->_vServers.begin();
	for (; it != this->_vServers.end(); it++)
	{
		if (it->isDefaultServer())
			this->_DSAmount++;
	}
	if (this->_DSAmount <= 0)
		throw(std::runtime_error("No virtual default server found!"));
}

/** 
 * **************************
 * SECTION - TO DEBUG AND DELETE LATERS
 * **************************
*/


void WebServ::debugMsg(std::string msg)
{
	std::cout << msg;
}

void WebServ::printServerVec()
{
	std::vector<Server>::iterator it = this->_vServers.begin();
	for (; it != this->_vServers.end(); it++)
	{
		std::cout << (*it) << std::endl;
	}
} 
void WebServ::printPollingVec(std::string status)
{
	std::vector<pollfd>::iterator it = this->_polling.begin();
	for (; it != this->_polling.end(); it++)
	{
		std::cout << "------------------\n";
		std::cout << "STRUCT POLLING ";
		std::cout << status << std::endl;;
		std::cout << "------------------\n";
		std::cout << "- fd: " << it->fd << std::endl;
		std::cout << "- events: ";
		if (it->events & POLLIN)
			std::cout << "POLLIN";
		if (it->events & POLLOUT)
			std::cout << " POLLOUT\n";
		if (it->events == 0)
			std::cout << "NO EVENTS\n";
		std::cout << "\n- revents: ";
		if (it->revents & POLLIN)
			std::cout << "POLLIN\n";
		if (it->revents & POLLOUT)
			std::cout << " POLLOUT\n";
		if (it->revents & POLLERR)
			std::cout << " POLLERR\n";
		if (it->revents & POLLHUP)
			std::cout << " POLLHUP\n";
		if (it->revents & POLLNVAL)
			std::cout << " POLLNVAL\n";
		if (!it->revents)
			std::cout << "NO REVENTS\n";
		std::cout << "------------------\n";
	}
} 
/* 
*/
