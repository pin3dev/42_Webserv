
#include "Configurator.hpp"

const int Configurator::_maxConnections = 10;

Configurator::Configurator(Server &server) : _tmpAddr(NULL), _server(server)
{
    std::memset(&this->_settings, 0, sizeof(this->_settings));
    this->_settings.ai_family = AF_INET;          // IPv4
    this->_settings.ai_socktype = SOCK_STREAM;    // TCP
    this->_settings.ai_protocol = IPPROTO_TCP;    // TCP
}

Configurator::~Configurator()
{
    std::memset(&this->_settings, 0, sizeof(this->_settings));
    this->freeTmpAddr();
}

void Configurator::setSocketToVServer()
{
    if (!this->_server.isDefaultServer())
        return;
    this->_createSocket();
    this->_setReuse();
    this->_getHostAddr();
    this->_assignAddr();
    this->_setPassive();
}

void Configurator::_createSocket()
{
	if (!this->_server.getSocket())
	{
		this->_server.setSocket(socket(AF_INET, SOCK_STREAM, 0));

		if (this->_server.getSocket() < 0)
			throw std::runtime_error("Problema no Configurator::_createSocket()");
	}
}

void Configurator::_setReuse()
{
	int active = 1;

	if (setsockopt(this->_server.getSocket(), SOL_SOCKET, SO_REUSEADDR, &active, sizeof(int)) < 0)
		throw std::runtime_error("Problema no Configurator::_setReuse()");
}

void Configurator::_getHostAddr()
{
	if (getaddrinfo(this->_server.getHost().c_str(), this->_server.getPort().c_str(), &this->_settings, &this->_tmpAddr) != 0)
		throw std::runtime_error("Problema no Configurator::_getHostAddr()");
}

void Configurator::_assignAddr()
{
	if (this->_tmpAddr)
	{
		if (bind(this->_server.getSocket(), this->_tmpAddr->ai_addr, this->_tmpAddr->ai_addrlen) < 0)
        	throw std::runtime_error("Problema no Configurator::_assignAddr()");
	}
}

void Configurator::_setPassive()
{
	if (listen(this->_server.getSocket(), this->_maxConnections) < 0)
		throw std::runtime_error("Problema no Configurator::_setPassive()");
}

void Configurator::freeTmpAddr()
{
    if (this->_tmpAddr)
    {
        freeaddrinfo(this->_tmpAddr);
        this->_tmpAddr = NULL;
    }
}


