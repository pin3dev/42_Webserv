/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pin3dev <pinedev@outlook.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 19:10:47 by pin3dev           #+#    #+#             */
/*   Updated: 2024/06/05 21:48:24 by pin3dev          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include "Utils.hpp"

/** 
 * **************************************
 * SECTION - CONSTRUCTORS & DESTRUCTORS
 * **************************************
 */

Response::Response() : _urlLoopCount(0), _connect_fd(0),  _servLoc(), _headerData()
{}

Response::~Response()
{}

/** 
 * *********************
 * SECTION - UTILS 
 * *********************
 */
//**ver esse Server: name
/* std::string autoHTML(std::string const &status, std::string const &statusDescrip, std::string const &fullPath)
{
	std::string fileContent = getFileContent(fullPath);
	int sizeFileContent = fileContent.length();
	
	std::string response = 
		"HTTP/1.1 " << status << " " << statusDescrip << "\n"
		"Date: " + Utils::_getTimeStamp("%a, %d %b %Y %H:%M:%S GMT") + "\n" +
		"Server: Webserv/1.0.0 (Linux)\n" +
		"Connection: keep-alive\n" +
		"Content-Type: " + getFileType(path) + "; charset=utf-8\n" +
		"Content-Length: " + Utils::_itoa(sizeFileContent) + "\n\n";

	return (response + fileContent);
} */


/* void _setEnvToCGI()
{
	setenv("CONTENT_TYPE", )
} */

void serveFile(const std::string &fullPath, int _connectSocket)
{
    std::cout << "Serving file: " << fullPath << std::endl;

	std::ifstream file(fullPath.c_str(), std::ios::binary | std::ios::in);

	if (!file.is_open())
	{
		write(_connectSocket, "HTTP/1.1 404 Not Found\n", 24);
		return;
	}
	file.close();

	/* autoHTML("200", "OK", fullPath); */
	std::string response = Utils::generateResponseOK(fullPath);
	write(_connectSocket, response.c_str(), response.length()); 
}

/* void serveError(int code, const std::string &message) {
    std::cout << "Error " << code << ": " << message << std::endl;
}
 */
bool isMethodAllowed(const std::vector<std::string> &methods, const std::string &method) {
    bool isAllowed = std::find(methods.begin(), methods.end(), method) != methods.end();
    if (isAllowed)
       	return ((std::cout << "O METODO: " << method << " É PERMITIDO NO LOCATION!\n"), true);
    return ((std::cout << "O METODO: " << method << " NÃO É PERMITIDO NO LOCATION!\n"), false);
}

// Função para processar POST
void processPost(const std::string &path) {
    // Suponha que estamos recebendo dados para salvar em um arquivo
    // Esta parte deve lidar com a leitura do corpo da requisição e salvar o conteúdo no path
    std::cout << "Processing POST request, saving data to: " << path << std::endl;
    // Implementação fictícia para salvar dados
    // Aqui você colocaria a lógica real para ler o corpo do POST e salvar o conteúdo
}

// Função para processar DELETE
void processDelete(const std::string &path) {
    std::cout << "Processing DELETE request, deleting data from: " << path << std::endl;
}

void serveAutoindex(const std::string &path, int _connectSocket)
{
	(void)_connectSocket, (void)path;
/*     DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(path.c_str())) != NULL)
	{
        std::cout << "Index of " << path << ":\n";
        while ((ent = readdir(dir)) != NULL)
            std::cout << ent->d_name << "\n";
        closedir(dir);
    }
	else
        throw std::runtime_error("500 Unable to open directory"); */
	
}

bool serveTryFile(const std::string &tryFilePath, int _connectSocket)
{
    if (Utils::fileExists(tryFilePath))
	{
        serveFile(tryFilePath, _connectSocket);
        return true;
    }
    return false;
}

/** 
 * **************************************
 * SECTION - GETTERS/SETTERS
 * **************************************
 */

void    Response::setConnectFd(int connectSocket){this->_connect_fd = connectSocket;}
int     Response::getConnectFd(){return (this->_connect_fd);}
void    Response::setServLoc(std::map<std::string, location_t> const &servLoc)
{
	this->_servLoc.clear();
	std::map<std::string, location_t>::const_iterator origMap = servLoc.begin();
	for (; origMap != servLoc.end(); origMap++)
	{
		this->_servLoc.insert(std::make_pair(origMap->first, origMap->second));
	}
}
void	Response::setHeaderData(std::map<std::string, std::string> const &headerData)
{
	this->_headerData.clear();
	std::map<std::string, std::string>::const_iterator origMap = headerData.begin();
	for (; origMap != headerData.end(); origMap++)
	{
		this->_headerData.insert(std::make_pair(origMap->first, origMap->second));
	}
}
/** 
 * **************************************
 * SECTION - RESPONSE
 * **************************************
 */


bool	Response::_isRightLocation(std::string locName, std::string &url)
{
	if (locName == url)
	{
		std::cout << locName << " é igual a " << url << std::endl;
		return true;
	}
	if (Utils::isParentDirOf(locName, url))
	{
		std::cout << locName << " é pai de " << url << std::endl;
		return true;
	}
	if (Utils::isExtensionOf(locName, url))
	{
		std::cout << locName << " é extensão de " << url << std::endl;
		return true;
	}
	return false;
}

bool 	Response::_isMethodAllowed(std::vector<std::string> const &methods, std::string &method)
{
	std::vector<std::string>::const_iterator curMethod = methods.begin();
	for ( ; curMethod != methods.end(); curMethod++) 
	{
		if (*curMethod == method) 
			return true;
	}
	return false;
}


/** 
 * **************************************
 * SECTION - CHAT
 * **************************************
 */

void	printMap(std::map<std::string, location_t> const &servLoc)
{
	std::map<std::string, location_t>::const_iterator it = servLoc.begin();
	for (; it != servLoc.end(); it++)
	{
		std::cout << "KEY: " << it->first << std::endl;
	}
}


// Função principal para processar a requisição
void Response::processRequest(const std::string &url, const std::string &method, const std::string &root)
{
	this->_urlLoopCount++;
	//printMap(this->_servLoc);
    std::string effectiveRoot = root; 
    std::string effectiveUrl = url;
	std::string effectiveCgiPath;
	std::string fullPath;
	std::map<std::string, location_t>::const_iterator rightLocation = this->_servLoc.end();
    std::cout << "EFETUANDO PROCESSAMENTO DE REQUEST PARA ROOT: " << effectiveRoot << " E URL: " << effectiveUrl << " NO SOCKET: " << this->getConnectFd() << std::endl;
	
	//ENCONTRAR LOCATION CERTO
	if (Utils::_isRoot(effectiveUrl))
		rightLocation = this->_servLoc.find("/");
 	else
	{
		std::map<std::string, location_t>::const_iterator curLocation = this->_servLoc.begin();
		for (; curLocation != this->_servLoc.end(); curLocation++)
		{
			if (_isRightLocation(curLocation->first, effectiveUrl))
			{
				rightLocation = curLocation;
				break;
			}
		}
	} 

	//VER SE TEM LOCATION PARA EXECUTAR O METODO
	if (method != "GET" && rightLocation == this->_servLoc.end())
		throw std::runtime_error("403 Forbidden");


	if (rightLocation != this->_servLoc.end())
	{
		std::cout << "LOCATION: " << rightLocation->first << " ACHADO PARA: " << effectiveUrl << std::endl;
		const location_t &loc = rightLocation->second;
		if (!loc.redirect.empty())
		{
			effectiveUrl = loc.redirect;
			std::cout << "REDIRECIONANDO DE: " << url << " PARA: " << effectiveUrl << std::endl;
			if (this->_urlLoopCount > 3)
				throw std::runtime_error("Too many redirects or root changes\n");
			processRequest(effectiveUrl, method, root);
			return;
		}
		if (!loc.root.empty())
		{
			std::cout << "ROTEANDO DE: " << root << " PARA: " << effectiveRoot << std::endl;
			effectiveRoot = loc.root;
		}
		if (!isMethodAllowed(loc.methods, method))
		{
			throw std::runtime_error("405 Method Not Allowed");
			return;
		}
	}


	//**SEMPRE VAI VIR COM UMA BARRA, QUE PRECISA SER REMOVIDA NA HORA DE UNIR O FULLPATH
	if (!effectiveUrl.empty() && effectiveUrl[0] == '/')
	{
        effectiveUrl.erase(0, 1);
    }
	
	//**PRECISO VERIFICAR EXT DE PY PARA CONCATENAR O PATH DO UPLOAD
    if (method == "POST" && Utils::isExtensionOf(".py", effectiveUrl))
	{
		if (rightLocation->second.cgiPath.empty())
			throw std::runtime_error("403 Forbidden: No upload path specified");
		if (rightLocation->second.cgiPath[0] == '/')
		{
			effectiveCgiPath = rightLocation->second.cgiPath;
			effectiveCgiPath = effectiveCgiPath.erase(0, 1);
		}
		fullPath = effectiveRoot + effectiveCgiPath + effectiveUrl;
	}
	else
	{
		fullPath = effectiveRoot + effectiveUrl;
	}
 
    if (method == "GET") //METODO GET
	{
        std::cout << "EFETUANDO METODO GET PARA PATH: " << fullPath << std::endl; 
        if (Utils::directoryExists(fullPath))
		{
			if (rightLocation != this->_servLoc.end() && !rightLocation->second.tryFile.empty() && serveTryFile((effectiveRoot + rightLocation->second.tryFile), this->getConnectFd())) //guardar o location no objeto e verificar se há um location
			{
                return;
            }
			else if (rightLocation != this->_servLoc.end() && rightLocation->second.autoindex)
			{
				std::cout << "TENTANDO AUTOINDEX EM: " << rightLocation->second.autoindex << std::endl;
                serveAutoindex(fullPath, this->getConnectFd());
            }
            else if (Utils::fileExists(fullPath + "index.html"))
			{
                serveFile((fullPath + "index.html"), this->getConnectFd());
            }
			else
			{
                throw std::runtime_error("403 Forbidden: No index file in directory");
            }
        }
		else if (Utils::fileExists(fullPath))
		{
			//verifica se eh um script e executa o script
            serveFile(fullPath, this->getConnectFd());
        }
		else
		{
            throw std::runtime_error("404 Not Found: File or directory does not exist");
        }
    }
	else if (method == "POST") //METODO POST
	{
        std::cout << "EFETUANDO METODO POST PARA PATH: " << fullPath << std::endl; 
        //processPost(fullPath);
    }
	else if (method == "DELETE") //METODO DELETE
	{
        std::cout << "EFETUANDO METODO DELETE PARA PATH: " << fullPath << std::endl; 
        //processDelete(fullPath);
    }
	else
	{
        throw std::runtime_error("405 Method Not Allowed");
    }
}

