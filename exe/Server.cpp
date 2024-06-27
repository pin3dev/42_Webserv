
#include "Server.hpp"
#include <unistd.h>

Server::Server(std::map<std::string, std::string> &settings, std::vector<Server> const &servers, std::map<std::string, location_t> const &locationMap)
{
    this->checkServerSettings(settings);
    this->applyServerSettings(settings, servers, locationMap);
}

Server::~Server(){}

std::string Server::getServerName() const {return this->serverName;}
std::string Server::getRoot() const {return this->root;}
std::string Server::getPort() const {return this->port;}
size_t Server::getClientMaxBodySize() const {return this->clientMaxBodySize;}
std::string Server::getIndexPage() const {return this->indexPage;}
std::string Server::getErrorPage() const {return this->errorPage;}
std::string Server::getHost() const {return this->host;}
std::map<std::string, location_t> const &Server::getLocations() const {return this->locations;}
bool Server::isDefaultServer() const {return this->isDefault;}
int Server::getSocket() const {return this->socket;}
void Server::setSocket(int socket) {this->socket = socket;}


void Server::checkServerSettings(std::map<std::string, std::string> const &settings)
{
	std::string const mandatory[] = {"host", "index", "listen", "root", "client_max_body_size"};
	std::string const forbidden[] = {"allow_methods", "autoindex", "cgi_ext", "cgi_path", "try_file", "upload_to"};

	for (int i = 0; i < 5; i++)
	{
		if (settings.find(mandatory[i]) == settings.end())
            throw std::runtime_error("Error: Missing keyword '" + mandatory[i] + "' in server block");	
	}

	for (int i = 0; i < 6; i++)
	{
		if (settings.find(forbidden[i]) != settings.end())
            throw std::runtime_error("Keyword '" + forbidden[i] + "' is not allowed in server block");
	}
}

void Server::applyServerSettings(std::map<std::string, std::string> &settings, std::vector<Server> const &servers, std::map<std::string, location_t> const &locationMap)
{
    this->serverName = this->ensureUniqueServerName(servers, settings["server_name"]);
    this->root = this->validateRootPath(settings["root"]);
    this->port = this->validatePortNumber(settings["listen"]);
    this->clientMaxBodySize = this->parseSizeWithSuffix(settings["client_max_body_size"]);
    this->indexPage = this->validatePagePath(settings["index"]);
    this->errorPage = this->validatePagePath(settings["error_page"]);
    this->host = settings["host"];
    this->locations = locationMap;
    this->isDefault = this->verifyServerAsDefault(servers);
    this->socket = 0;
}

std::string Server::ensureUniqueServerName(std::vector<Server> const &servers, std::string const &name)
{
	std::vector<Server>::const_iterator it;

    for (it = servers.begin(); it != servers.end(); it++) 
    {
        if (!name.empty() && it->serverName == name)
            throw std::runtime_error("Error: Duplicate server name");  
    }
	return name;
}

std::string Server::validateRootPath(std::string const &root)
{
	std::string tmp = root;

	if (tmp[tmp.length() - 1] != '/')
		tmp += "/";
	if (!directoryExists(tmp))
        throw std::runtime_error("Error: Root does not have a valid directory");
	return (tmp);
}

std::string Server::validatePortNumber(std::string const &port) 
{
    int portNumber = atoi(port.c_str());

    if (port.find_first_not_of("0123456789") != std::string::npos || portNumber < 0 || portNumber > 65535)
        throw std::runtime_error("Error: Invalid port number: " + port);
    return port;
}

size_t Server::parseSizeWithSuffix(std::string const &size) 
{
    std::string numberPart;
    std::string suffix;
    double value = 0.0;

    if (isdigit(size[size.length() - 1]))
        numberPart = size;
    else 
    {
        numberPart = size.substr(0, size.length() - 1);
        suffix = size.substr(size.length() - 1, 1);
    }

    if ((!suffix.empty() && suffix.find_first_not_of("bBkKmMgG") != std::string::npos) ||
        numberPart.find_first_not_of("0123456789.") != std::string::npos)
            throw std::runtime_error("Error: '" + size + "' is not a valid size.");  
    value = atof(numberPart.c_str());
    // value is treated as bytes by default. Conversions are only applied to K, M and G suffixes.
    if (suffix == "k" || suffix == "K")
        value *= 1024;
    else if (suffix == "m" || suffix == "M")
        value *= 1024 * 1024;
    else if (suffix == "g" || suffix == "G")
        value *= 1024 * 1024 * 1024;

    if (value > MAX_SIZE_LIMIT)
        throw std::runtime_error("Error: '" + size + "' is not a valid size.");
    return static_cast<size_t>(value);
}

std::string Server::validatePagePath(std::string const &page)
{
    if (!page.empty())
    {
        size_t dot = page.find_last_of(".");
        
        if (dot == std::string::npos || page.substr(dot) != ".html" || page.length() <= std::string(".html").length())
            throw std::runtime_error("Error: Pages must be .html");
        std::string path = this->root + page;
        std::ifstream file(path.c_str());
        if (!file.good())
            throw std::runtime_error("Error: Couldn't open page '" + page + "'");
        file.close();
    }
    return page;
}

bool Server::verifyServerAsDefault(std::vector<Server> const &servers) 
{
    std::vector<Server>::const_iterator it;
    for (it = servers.begin(); it != servers.end(); ++it) 
    {
        if (it->host == this->host && it->port == this->port)
            return false;
    }
    return true;
}

bool Server::directoryExists(std::string const &path)
{
	struct stat info;

	if (stat(path.c_str(), &info) != 0)
		return false;
	else if (info.st_mode & S_IFDIR)
		return true;
	return false;
}


