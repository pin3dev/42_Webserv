/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pin3dev <pinedev@outlook.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 17:51:59 by pin3dev           #+#    #+#             */
/*   Updated: 2024/05/16 20:22:11 by pin3dev          ###   ########.fr       */
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
	
	//TO DELETE LATER
	printServerVec();
	debugMsg("WebServ Ready!\n\r\n");
}
WebServ::~WebServ()
{
	this->_vServers.clear();
	//this->_polling.clear();
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
		//freeTmpAddr();
	}
}

void WebServ::runWebServ()
{
	//printPollingVec("STARTING");
	write(1, "HI DO RUNWEBSERV()\n", 20);
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
	write(1, "HI DO VERIFYPOLLING()\n", 23);
	std::cout << "\n\n\tThis VECTOR POLLING size is: " << this->_polling.size() << std::endl;
	for (size_t i = 0; i < this->_polling.size(); i++)
	{
		std::cout << "\n\n\tThis FD POLLING is: " << this->_polling[i].fd << std::endl;
		Manager manager(this->_polling[i]);
		manager.setType(_DSAmount, i);
		manager.setRefs(this->getPolling(), this->getvServers(), this->getConnects());
		manager.runManager();
	}
}
 
void WebServ::_cleaningPolling()
{
	if ((int)this->_polling.size() == _DSAmount)
		return;
	write(1, "HI DO CLEANINGPOLLING()\n", 23);
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
	write(1, "HI DO RUNPOLLING()\n", 20);
	//printPollingVec("BEFORE POLL");
	if (poll(this->_polling.data(), this->_polling.size(), 200) < 0 && this->gShutDown == false)
		throw std::runtime_error("Polling error!\n");
	//printPollingVec("AFTER POLL");
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

	pollfd newPoll = {0, 0, 0};
	newPoll.fd = socket;
	newPoll.revents = 0;
	newPoll.events = POLLIN;
	std::cout << "\n\nNEWPOLL SERVER CREATED\n\n";
	this->_polling.push_back(newPoll);
}

/** 
 * **************************
 * SECTION - GETTERS
 * **************************
*/

int WebServ::getDSAmount(){return (_DSAmount);}
std::vector<Server> &WebServ::getvServers(){return (this->_vServers);}
std::vector<Connect> &WebServ::getConnects(){return (this->_connects);}
std::vector<pollfd> &WebServ::getPolling(){return (this->_polling);}

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
		debugMsg("Shutting down...\n");
	}
}

bool WebServ::__isValidFD(int fd)
{
	if (fd <= 3)
		return false;
	return true;
}

void WebServ::_countAndCheckVirtualServers() 
{
	std::vector<Server>::iterator it = this->_vServers.begin();
	for (; it != this->_vServers.end(); it++)
	{
		if (it->isDefaultServer())
			_DSAmount++;
	}
	if (_DSAmount <= 0)
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
