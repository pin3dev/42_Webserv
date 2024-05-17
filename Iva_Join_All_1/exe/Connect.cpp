/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connect.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pin3dev <pinedev@outlook.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 19:04:31 by pin3dev           #+#    #+#             */
/*   Updated: 2024/05/17 18:13:45 by pin3dev          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Connect.hpp"

/** 
 * **************************
 * SECTION - UTILS
 * **************************
*/

time_t _nowTime()
{
    return (std::time(NULL));
}
// getTimeStamp() "%a, %d %b %Y %H:%M:%S GMT"
// getTime() "%H:%M:%S"
std::string _getTimeStamp(const char *format)
{
	time_t		now = _nowTime();
	struct tm	tstruct;
	char		buf[80];

	tstruct = *localtime(&now); 
	strftime(buf, sizeof(buf), format, &tstruct);
	return (buf);
}

/** 
 * **************************************
 * SECTION - CONSTRUCTORS & DESTRUCTORS
 * **************************************
*/

Connect:: Connect(Server &server, int connect_fd) : _connect_fd(connect_fd), _myServer(&server), _ready(false)
{
    this->_setUpdate(_nowTime());
    this->_myRequest.setMaxLength(_myServer->getClientMaxBodySize());
}

Connect::~Connect()
{}

/** 
 * **************************
 * SECTION - PUBLIC METHODS
 * **************************
*/

void	Connect::appendToRequest(char const *buffer, size_t size)
{
    this->_setUpdate(_nowTime()); 
    //this->_ready = false; //**to update
	this->_myRequest.toAppend(buffer, size); 
}

void	Connect::runRequest()
{
	//this->_sentRequest = true;
    this->_setUpdate(_nowTime()); 
	try
	{
		this->_myRequest.checkStatusRequest();
        
		//std::string root = this->_myServer->getRoot();
		//std::string resource = getPathWithoutSlashAtEnd(this->_resource);
		//this->_checkLocation(root, resource, 0);
	}
	catch(const std::exception& e)
	{
		this->_myResponse.setStatus(e.what()); //**VERIFICAR APLICAÇÃO DESSE METODO
	}

/* 	this->_request.clear(); //
	this->_requestPayload.clear(); 
	this->_sentRequest = false; */
}
/** 
 * **************************
 * SECTION - SETTERS
 * **************************
*/

void	Connect::_setUpdate(time_t now)
{
    this->_updated = now;
}

/** 
 * **************************
 * SECTION - PRIVATE METHODS
 * **************************
*/

bool	Connect::hasExpired() const
{
	return ((_nowTime() - this->_updated) > 60); //60 seconds //**verificar esse tempo
}

//**RESOLVER ESSE MÉTODO COM FLAGS MELHORES
/* bool	Connect::isReady() const
{
	return (this->_updated == true || this->_request.find(REQUEST_END) != std::string::npos);
} */


/** 
 * **************************
 * SECTION - TO DELETE
 * **************************
*/

/* void	Client::_checkLocation(std::string &root, std::string &resource, size_t loopCount)
{
	if (loopCount > MAX_LOOP_COUNT) //MAX DE LOOP DE RECURSÃO
		throw std::runtime_error(RS_508);

	locationMap::const_iterator location = this->_server.getLocations().begin(); //ITERATOR PARA O MAP DE LOCALIZAÇÕES DO SERVIDOR
	for (; location != this->_server.getLocations().end(); location++) //CORRE TODOS OS KEYS DE LOCALIZAÇÕES DO SERVIDOR
	{
		if (this->_locationIsRootAndResourceNot(location->first, resource)) //VERIFICA SE A REGRA É PARA "/" E SE O URL NÃO É "/"
			continue;
		if (!this->_resourceHasLocation(location->first, resource) && this->_method != DELETE) //VERIFICA SE O URL NEM É UM SUFIXO DA REGRA ATUAL, E SE O METODO REQUISITADO NÃO É DELETE
			continue;
		if (!this->_methodMatches(location->second.methods)) //VERIFICA SE O METODO REQUISITADO NÃO É PERMITIDO NA REGRA ATUAL
		{
			if (!this->_hasValidDelete(resource)) //VERIFICA SE O METODO DELETE NÃO É PERMITIDO NO REPOSITORIO PAI DO ARQUIVO REQUISITADO
				throw std::runtime_error(RS_405);
		}
		if (this->_hasRedirection(resource, root, loopCount, location->second.redirect, location->first))//VERIFICA SE O LOCATION ATUAL TEM REDIRECIONAMENTO, E REALIZA O REDIRECIONAMENTO DO URL E CHAMANDO A FUNÇAO RECURSIVAMENTE
			return; //MORRE AQUI
		this->_updateRootIfLocationHasIt(root, location->second.root); //VERIFICA SE HA UM NOVO ROOT DETERMINADO NO LOCATION ATUAL E ATUALIZA O ROOT DO SERVIDOR
		if (this->_hasValidPath(resource, root, location->second))//VER SE O CAMINHO DO URL REQUISITADO É VALIDO PARA GET COM REGRAS DE AUTOINDEX, TRYFILE OU APENAS UM ARQUIVO INDEX E ENVIA O CONTEUDO NO SOCKET DE CONEXÃO
			return; //MORRE AQUI
		break; 
	}
	if (this->_hasInvalidLocation(location, resource))
		throw std::runtime_error(RS_403);
	this->_updateResourceWhenHasCgiPath(resource, location); 
	this->_methodsManager(root, resource, location->second);
} */

