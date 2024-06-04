/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connect.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pin3dev <pinedev@outlook.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 19:04:31 by pin3dev           #+#    #+#             */
/*   Updated: 2024/06/03 21:26:23 by pin3dev          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Connect.hpp"


/** 
 * **************************************
 * SECTION - CONSTRUCTORS & DESTRUCTORS
 * **************************************
*/

Connect:: Connect(Server &server, int connect_fd) : _connect_fd(connect_fd), _myServer(&server)
{
    //this->_setUpdate(Utils::_nowTime());
    this->_myRequest.setMaxLength(_myServer->getClientMaxBodySize()); //**CONFIGURADO NO CONSTRUCTOR DE CONNECT
	this->_myResponse.setServLoc(_myServer->getLocations()); //**CONFIGURADO NO CONSTRUCTOR DE CONNECT
	this->_myResponse.setConnectFd(this->getConnectFd()); //**CONFIGURADO NO CONSTRUCTOR DE CONNECT
}

Connect::~Connect()
{
	//close(this->_connect_fd);
	//std::cout << "Connect destroyed" << std::endl;
}

/** 
 * **************************
 * SECTION - PUBLIC METHODS
 * **************************
*/

void	Connect::appendToRequest(char const *buffer, size_t size)
{
    //this->_setUpdate(Utils::_nowTime()); 
	this->_myRequest.toAppend(buffer, size); 
}

void	Connect::runRequest(std::vector<Server> &VecServers)
{
	//this->_sentRequest = true;
    //this->_setUpdate(_nowTime()); 
	try
	{
		//TESTE DE ERRO - SEGFAULT
/* 		(void)VecServers;
		std::string response = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: 30\n\nHello World! TO: " + this->getServer().getPort();
		std::cout << "SENDING RESPONSE" << std::endl;
		write(this->getConnectFd(), response.c_str(), response.length()); */
		this->_myRequest.checkStatusRequest();
		this->_searchForNonDefaultServer(VecServers);
		std::string url = this->getRequest().getURL();
		std::string method = this->getRequest().getMethod();
		this->_myResponse.processRequest(url, method, this->getServer().getRoot()); 
	}
	catch(const std::exception& e)
	{
		std::cout << "\n\tERROR DESCOMUNAL AQUI: " << e.what() << std::endl;
		//this->_myResponse.setStatus(e.what()); //**
	}

/* 	this->_request.clear(); //
	this->_requestPayload.clear(); 
	this->_sentRequest = false; */
}
/** 
 * **************************
 * SECTION - GETTERS & SETTERS
 * **************************
*/


int		Connect::getConnectFd() const {return (this->_connect_fd);}
Request	&Connect::getRequest(){return (this->_myRequest);}
Response &Connect::getResponse(){return (this->_myResponse);}
Server	&Connect::getServer(){return (*this->_myServer);}
time_t	Connect::getLastUpdated() const{return (this->_updated);}

void	Connect::resetServer(Server *server)
{
	this->_myServer = server;
	this->_myResponse.setServLoc(server->getLocations());
}
void	Connect::_setUpdate(time_t now){this->_updated = now;}


/** 
 * **************************
 * SECTION - PRIVATE METHODS
 * **************************
*/

bool	Connect::hasExpired() const
{
	return ((Utils::_nowTime() - this->_updated) > 60); //**
}

//**RESOLVER ESSE MÉTODO COM FLAGS MELHORES
/* bool	Connect::isReady() const
{
	return (this->_updated == true || this->_request.find(REQUEST_END) != std::string::npos);
} */

void Connect::_searchForNonDefaultServer(std::vector<Server> &VecServers)
{	
	std::string requestHost = this->getRequest().getHost();
	
	if (requestHost == this->getServer().getServerName()) 
		return;
	
	std::vector<Server>::iterator server = VecServers.begin();
	for (; server != VecServers.end(); server++)
	{
		if (requestHost == server->getServerName() && server->getHost() == this->getServer().getHost())
		{
			this->resetServer(&(*server));
			break;
		}	
	}
}
