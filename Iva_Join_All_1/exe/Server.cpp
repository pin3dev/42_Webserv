/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pin3dev <pinedev@outlook.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 19:36:13 by pin3dev           #+#    #+#             */
/*   Updated: 2024/05/15 21:59:07 by pin3dev          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

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
        {
            std::cout << "Missing keyword '" + mandatory[i] + "' in server block" << std::endl;
            exit(1);
            //throw std::runtime_error(ERR_KEYWORD_MISSING(mandatory[i]));
        }	
	}

	for (int i = 0; i < 6; i++)
	{
		if (settings.find(forbidden[i]) != settings.end())
        {
            std::cout << "Keyword '" + forbidden[i] + "' is not allowed in server block" << std::endl;
            exit(1);
            //throw std::runtime_error(ERR_FORBIDDEN_KEYWORD(forbidden[i]));
        }
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
    // fazer depois this->errorResponse = this->createNotFoundResponse();
    this->isDefault = this->verifyServerAsDefault(servers);
    this->socket = 0;
}

std::string Server::ensureUniqueServerName(std::vector<Server> const &servers, std::string const &name)
{
	std::vector<Server>::const_iterator it;

    for (it = servers.begin(); it != servers.end(); it++) 
    {
        if (!name.empty() && it->serverName == name)
        {
            std::cout << "Duplicate server name '" + name + "'. Server names must be unique" << std::endl;
            exit(1);
            //throw std::runtime_error(ERR_DUPLICATE_NAME(name));
        }  
    }
	return name;
}

std::string Server::validateRootPath(std::string const &root)
{
	std::string tmp = root;

	if (tmp[tmp.length() - 1] != '/')
		tmp += "/";
	if (!directoryExists(tmp))
    {
        std::cout << "'" + tmp + "' is not a valid directory" << std::endl;
        exit(1);
		//throw std::runtime_error(ERR_DIRECTORY(tmp));
    }
	return (tmp);
}

std::string Server::validatePortNumber(std::string const &port) 
{
    int portNumber = std::atoi(port.c_str());

    if (port.find_first_not_of("0123456789") != std::string::npos || portNumber < 0 || portNumber > 65535)
    {
        std::cout << "Invalid port number: " + port << std::endl;
        exit(1);
        //throw std::runtime_error("Invalid port number: " + port);
    }
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
        {
            std::cout << "'" + size + "' is not a valid size. Size must be a number positive or a number followed by a sufix (b - B, k - K, m - M, g - G)" << std::endl;
            exit(1);
            //throw std::runtime_error(ERR_MAX_SIZE_INPUT(size));
        }   
    value = atof(numberPart.c_str());
    // value is treated as bytes by default. Conversions are only applied to K, M and G suffixes.
    if (suffix == "k" || suffix == "K")
        value *= 1024;
    else if (suffix == "m" || suffix == "M")
        value *= 1024 * 1024;
    else if (suffix == "g" || suffix == "G")
        value *= 1024 * 1024 * 1024;

    if (value > MAX_SIZE_LIMIT)
    {
        std::cout << "'" + size + "' is not a valid size. The max value allowed is 10G (10737418240 bytes)" << std::endl;
        exit(1);
        //throw std::runtime_error(ERR_MAX_SIZE_RANGE(size));
    }
    return static_cast<size_t>(value);
}

std::string Server::validatePagePath(std::string const &page)
{
    if (!page.empty())
    {
        size_t dot = page.find_last_of(".");
        
        if (dot == std::string::npos || page.substr(dot) != ".html" || page.length() <= std::string(".html").length())
        {
            std::cout << "'" + page + "' is a invalid file. Pages must be .html" << std::endl;
            exit(1);
            //throw std::runtime_error(ERR_PAGE_EXT(page));
        }
        std::string path = this->root + page;
        std::ifstream file(path.c_str());
        if (!file.good())
        {
            std::cout << "Couldn't open page '" + page + "'" << std::endl;
            exit(1);
            //throw std::runtime_error(ERR_PAGE_FIND(path));
        }
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

std::ostream &operator<<(std::ostream &out, Server const &server)
{
    std::string isDefault = "TRUE";
    if (!server.isDefaultServer())
        isDefault = "FALSE";
	out << "ServerName: " << server.getServerName() << std::endl;
	out << "Port: " << server.getPort() << std::endl;
	out << "Host: " << server.getHost() << std::endl;
	out << "Root: " << server.getRoot() << std::endl;
	out << "Index: " << server.getIndexPage() << std::endl;
	out << "ClientMaxBodySize: " << server.getClientMaxBodySize() << std::endl;
	out << "ErrorPage: " << server.getErrorPage() << std::endl;
	out << "IsDefault: " << isDefault << std::endl;
	
    std::map <std::string, location_t>::const_iterator it = server.getLocations().begin();
	for (; it != server.getLocations().end(); it++)
	{
		out << "location: " << it->first << std::endl;
		out << "	Root: " << it->second.root << std::endl;
		out << "	Methods: ";

        std::vector<std::string>::const_iterator it2 = it->second.methods.begin();
		for (; it2 != it->second.methods.end(); it2++)
		    out << *it2 << " ";
		out << std::endl;
		out << "	Return: " << it->second.redirect << std::endl;
		out << "	Autoindex: " << it->second.autoindex << std::endl;
		out << "	TryFile: " << it->second.tryFile << std::endl;
		out << "	HasCGI: " << it->second.hasCGI << std::endl;
		out << "	CGIPath: " << it->second.cgiPath << std::endl;
		out << "	CGIExtension: " << it->second.cgiExtension << std::endl;
		out << "	UploadTo: " << it->second.uploadTo << std::endl;
		out << std::endl;
	}
	// out << "Error Reponse ↓" << std::endl << server.getErrorResponse() << std::endl;
	return out;
}
