
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
	std::signal(SIGPIPE, SIG_IGN); 
	std::signal(SIGINT, signalHandler); 
	_countAndCheckVirtualServers();
	_configVirtualServers();
}
WebServ::~WebServ()
{
	this->_cleanWebserv();
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
	while (!gShutDown)
	{
		_runPolling();
		_verifyPolling();
		_cleaningPolling();
	}
}

void WebServ::_verifyPolling()
{
	for (size_t i = 0; i < this->_polling.size(); i++)
	{
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
	if (poll(this->_polling.data(), this->_polling.size(), 200) < 0 && this->gShutDown == false)
		throw std::runtime_error("Timeout do Polling.");
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

void WebServ::signalHandler(int signum)
{
	if (signum == SIGINT)
	{
		gShutDown = true;
	}
}

bool WebServ::__isValidFD(int fd)
{
	if (fd < 2)
		return false;
	return true;
}

void WebServ::_countAndCheckVirtualServers() 
{
	std::vector<Server>::iterator it = this->_vServers.begin();
	for (; it != this->_vServers.end(); it++)
	{
		if (it->isDefaultServer())
			this->_DSAmount++;
	}
	if (this->_DSAmount <= 0)
		throw std::runtime_error("Nenhum servidor default foi definido!");
}

void WebServ::_cleanWebserv()
{
	std::vector<pollfd>::iterator it = this->_polling.begin();
	for (; it != this->_polling.end(); it++)
	{
		if (it->fd > 0)
			close (it->fd);
	}
}


