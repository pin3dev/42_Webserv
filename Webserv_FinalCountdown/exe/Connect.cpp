/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connect.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pin3dev <pinedev@outlook.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 19:04:31 by pin3dev           #+#    #+#             */
/*   Updated: 2024/06/05 19:50:12 by pin3dev          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Connect.hpp"
#include "Utils.hpp"


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


std::string getFileType(std::string const &file)
{
	std::map<std::string, std::string> types;

	types["txt"] = "text/plain";
	types["html"] = "text/html";
	types["css"] = "text/css";

	types["js"] = "application/javascript";
	types["py"] = "application/python";

	types["jpg"] = "image/jpg";
	types["jpeg"] = "image/jpeg";
	types["png"] = "image/png";
	types["gif"] = "image/gif";

	if (file.find_last_of(".") != std::string::npos)
	{
		std::string extension = file.substr(file.find_last_of(".") + 1);
		if (types.find(extension) != types.end())
			return (types[extension]);
	}
	return ("text/plain");
}
std::string getFileContent(std::string const &path)
{
	std::string content;
	std::ifstream file(path.c_str(), std::ios::binary | std::ios::in);

	content.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	file.close();
	return (content);
}

std::string autoHTML(std::string const &status, std::string const &statusDescrip, std::string const &fullPath)
{
	std::string fileContent = Utils::getFileContent(fullPath);
	int sizeFileContent = fileContent.length();
	
	std::string response = 
		"HTTP/1.1 " + status + " " + statusDescrip + "\n"
		"Date: " + Utils::_getTimeStamp("%a, %d %b %Y %H:%M:%S GMT") + "\n" +
		"Server: Webserv/1.0.0 (Linux)\n" +
		"Connection: keep-alive\n" +
		"Content-Type: " + Utils::getFileType(fullPath) + "; charset=utf-8\n" +
		"Content-Length: " + Utils::_itoa(sizeFileContent) + "\n\n";

	return (response + fileContent);
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
		this->_myRequest.checkStatusRequest();
		this->_myResponse.setHeaderData(this->_myRequest.getHeadData());
		this->_searchForNonDefaultServer(VecServers);
		std::string url = this->getRequest().getURL();
		std::string method = this->getRequest().getMethod();
		this->_myResponse.processRequest(url, method, this->getServer().getRoot()); 
	}
	catch(const std::exception& e)
	{
		//std::cout << "\n\tERROR DESCOMUNAL AQUI: " << e.what() << std::endl;
		std::cout << e.what() << std::endl;
		std::string ErrorResponse;
		ErrorResponse = autoHTML("404", "Error", "www/site1/404.html");//**TEMPORARIO
		write(this->_connect_fd, ErrorResponse.c_str(), ErrorResponse.size());
		//this->_myResponse.errorHTML(e.what()); //**MUDAR PARA UMA FUNCAO AUTOMATICA
		//**ACHO QUE TENHO QUE FECHAR A CONEX'AO AQUI
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

/* bool	Connect::hasExpired() const
{
	return ((Utils::_nowTime() - this->_updated) > 60); 
} */

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
