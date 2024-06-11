/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connect.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pin3dev <pinedev@outlook.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 19:04:31 by pin3dev           #+#    #+#             */
/*   Updated: 2024/06/11 17:11:06 by pin3dev          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Connect.hpp"
#include "Utils.hpp"

/** 
 * **************************************
 * SECTION - CONSTRUCTORS & DESTRUCTORS
 * **************************************
*/

Connect:: Connect(Server &server, int connect_fd) : _connect_fd(connect_fd), _urlLoopCount(0), 
_myServer(&server), _fullPath(""), _effectiveUpload("")
{
    this->_updated = Utils::_nowTime();
	this->_rightLocation = this->_myServer->getLocations().end();
	this->_cgiLocation = this->_myServer->getLocations().find(".py");
    this->_myRequest.setMaxLength(_myServer->getClientMaxBodySize());
}

Connect::~Connect()
{
	//close(this->_connect_fd);
}



/** 
 * **************************
 * SECTION - PUBLIC METHODS
 * **************************
*/

void	Connect::appendToRequest(char const *buffer, size_t size)
{
    this->_setUpdate(Utils::_nowTime()); 
	this->_myRequest.toAppend(buffer, size); 
}

void	Connect::runRequest(std::vector<Server> &VecServers)
{
	std::cout << "RESPONDENDO REQUEST...\n";
    this->_setUpdate(Utils::_nowTime()); 
	try
	{
		this->_myRequest.checkStatusRequest();
		if (!this->_myRequest.getFlagToResponse())
			return ;
		this->_searchForNonDefaultServer(VecServers);
		this->_processRequest(this->getRequest().getURL(), this->getRequest().getMethod(), this->getServer().getRoot()); 
		std::cout << "REQUEST RESPONDIDO...\n";
	}
	catch(const std::exception& e)
	{
		std::string myWhat = e.what();
		write(this->_connect_fd, e.what(), myWhat.size());
		//**ACHO QUE TENHO QUE FECHAR A CONEX'AO AQUI
	}
	this->_urlLoopCount = 0;
	//this->_myRequest.setReadyToResponse(false);
/* 	this->_request.clear(); //
	this->_requestPayload.clear();  */
}
/** 
 * **************************
 * SECTION - GETTERS & SETTERS
 * **************************
*/


int		Connect::getConnectFd() const {return (this->_connect_fd);}
Request	&Connect::getRequest(){return (this->_myRequest);}
Server	&Connect::getServer(){return (*this->_myServer);}
time_t	Connect::getLastUpdated() const{return (this->_updated);}
void	Connect::_setUpdate(time_t now){this->_updated = now;}

bool	Connect::isExpired()
{
	bool isExpired = (Utils::_nowTime() - this->getLastUpdated()) > 60;
	return (isExpired);
}


void	Connect::setServer(Server *server)
{
	this->_myServer = server;
	this->_myRequest.setMaxLength(_myServer->getClientMaxBodySize());
}


/** 
 * **************************
 * SECTION - PRIVATE METHODS
 * **************************
*/


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
			this->setServer(&(*server));
			//this->_myRequest.setReadyToResponse(false);
			break;
		}	
	}
}


//------------------------------------------------------------------------------------------------------------


bool Connect::_tryServeErrorPage(const std::string &effectiveRoot)
{
	if (this->_myServer->getErrorPage().empty())
		return false;
	std::string fullErrorPath = effectiveRoot + this->_myServer->getErrorPage();
	std::cout << "ERROR PAGE: " << fullErrorPath << std::endl;
	if (Utils::isReadeableFile(fullErrorPath))
	{
		std::string response = Utils::autoHTML("404", "Not Found", fullErrorPath);
		write(this->_connect_fd, response.c_str(), response.length());
		return true;
	}
	return false;
}

void Connect::_serveFile(const std::string &fullPath, const std::string &effectiveRoot, int _connectSocket)
{
/*	std::ifstream file(fullPath.c_str(), std::ios::binary | std::ios::in);

 	if (!file.is_open())
		throw std::runtime_error(Utils::_defaultErrorPages(404, "O servidor nao possui permissao para abrir o arquivo."));
	file.close(); */
	if (Utils::isReadeableFile(fullPath) == false)
	{
		if (this->_tryServeErrorPage(effectiveRoot))
			return ;
		throw std::runtime_error(Utils::_defaultErrorPages(404, "O servidor nao possui permissao para abrir o arquivo."));
	}
	std::string response = Utils::autoHTML("200", "OK", fullPath);
	write(_connectSocket, response.c_str(), response.length()); 
}

void Connect::_serveAutoindex(std::string &fullPath, std::string &effectiveRoot, int _connectSocket)
{
	DIR *root;
	dirent *current;

	root = opendir(fullPath.c_str());
	if (root == NULL)
	{
		if (this->_tryServeErrorPage(effectiveRoot))
			return ;
		throw std::runtime_error(Utils::_defaultErrorPages(404, "O servidor nao possui permissao para abrir o diretorio."));
	}
	
	if (fullPath.substr(0, effectiveRoot.length()) == effectiveRoot)
        fullPath.erase(0, effectiveRoot.length());
	
	std::string bodyPage = 
	"<!DOCTYPE html>\n"
	"<html lang=\"pt-BR\">\n"
	"\n"
	"<head>\n"
	"    <!-- Idioma -->\n"
	"    <meta charset=\"UTF-8\">\n"
	"    <!-- Responsividade -->\n"
	"    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1, maximum-scale=1\">\n"
	"    <!-- Defines -->\n"
	"    <link rel=\"canonical\" href=\"https://www.site1/index.html\" />\n"
	"    <link rel=\"shortcut icon\" href=\"/favicon.ico\">\n"
	"    <title>Cliva Website</title>\n"
	"    <!-- Estilo -->\n"
	"    <link rel=\"stylesheet\" href=\"/styles.css\">\n"
	"    <!-- Fonts -->\n"
	"    <link href=\"https://fonts.googleapis.com/css2?family=Major+Mono+Display&display=swap\" rel=\"stylesheet\">\n"
	"    <link href=\"https://fonts.googleapis.com/css2?family=DM+Mono:ital,wght@0,300;0,400;0,500;1,300;1,400;1,500&family=Major+Mono+Display&display=swap\" rel=\"stylesheet\">\n"
	"    <link href=\"https://fonts.googleapis.com/css2?family=Foldit:wght@100..900&family=Libre+Barcode+39+Text&display=swap\" rel=\"stylesheet\">\n"
	"</head>\n"
	"<body>\n"
	"    <div class=\"container-top\">\n"
	"        <header>\n"
	"            <h2 class=\"site-logo\">Cliva Webserv </h2>\n"
	"            <h5 class=\"site-slogan\">A única coisa especial aqui é sua avaliação</h5>\n"
	"            <nav>\n"
	"                <ul>\n"
	"                    <li><a href=\"/index.html\">Início</a></li>\n"
	"                    <li><a href=\"/tutorial.html\">Tutorial</a></li>\n"
	"                    <li><a href=\"/carregar.html\">Carregar</a></li>\n"
	"                    <li><a href=\"/apagar.html\">Apagar</a></li>\n"
	"                    <li><a href=\"/timestamp.html\">Tempo</a></li>\n"
	"                </ul>\n"
	"            </nav>\n"
	"        </header>\n"
	"\n"
	"        <div class=\"main-content\">\n"
	"            <h3>Autoindex ligado para /" + fullPath + "</h3>\n"
	"			 <p>Abaixo voce podera acessar os arquivos presentes no diretorio atual apenas com um clique</p>";

	while ((current = readdir(root)) != NULL)
	{
		if (current->d_name == std::string(".") || current->d_name == std::string(".."))
			continue;
		bodyPage += "			<li><a href='/" + fullPath + "/" + std::string(current->d_name) + "'>" + std::string(current->d_name) + "</a></li>";
	}
	bodyPage +=
		"        </div>\n"
		"\n"
		"</body>\n"
		"\n"
		"</html>";
	closedir(root);
	
	std::string HTMLHeaders = 
	"HTTP/1.1 200 OK \n"
	"Date: " + Utils::_getTimeStamp("%a, %d %b %Y %H:%M:%S GMT") + "\n" +
	"Server: Webserv/1.0.0 (Linux)\n" +
	"Connection: close\n" +
	"Content-Type: text/html; charset=utf-8\n" +
	"Content-Length: " + Utils::_itoa(bodyPage.size()) + "\n\n";

	std::string HtmlAutoIndexContent = HTMLHeaders + bodyPage;

	write(_connectSocket, HtmlAutoIndexContent.c_str(), HtmlAutoIndexContent.length());
}


bool Connect::_serveTryFile(const std::string &tryFilePath, const std::string &effectiveRoot, int _connectSocket)
{
    if (Utils::fileExists(tryFilePath))
	{
        this->_serveFile(tryFilePath, effectiveRoot, _connectSocket);
        return true;
    }
    return false;
}

void Connect::_exportEnviron(CGI &cgi)
{
	cgi.setNewEnv("GATEWAY_INTERFACE", "CGI/1.1");
	cgi.setNewEnv("SERVER_PROTOCOL", "HTTP/1.1");
	cgi.setNewEnv("SERVER_SOFTWARE", "Webserv/1.0");
	cgi.setNewEnv("ROOT_FOLDER" , this->_myServer->getRoot());
	cgi.setNewEnv("SCRIPT_FILENAME" , this->_fullPath);
	cgi.setNewEnv("REQUEST_METHOD", this->_myRequest.getMethod());
	cgi.setNewEnv("CONTENT_TYPE" ,this->_myRequest.getHeadData()["Content-Type"]);
	cgi.setNewEnv("CONTENT_LENGHT", this->_myRequest.getHeadData()["Content-Length"]);
	cgi.setNewEnv("UPLOAD_PATH", this->_effectiveUpload);
	cgi.setNewEnv("USER_AGENT" ,this->_myRequest.getHeadData()["User-Agent"]);
	//if (this->_myRequest.getHeadData()["User-Agent"].find("curl"))
		cgi.setNewEnv("PAYLOAD", this->_myRequest.getPayload());
}


void Connect::_processRequest(const std::string &url, const std::string &method, const std::string &root)
{

	//std::cout << "\n\t1\n";
	this->_urlLoopCount++;
	//std::cout << "\n\t2\n";
    std::string effectiveRoot = root; 
    std::string effectiveUrl = url;
	std::string effectiveCgiPath;
	std::string fullPath;
	//std::map<std::string, location_t>::const_iterator rightLocation = this->_myServer->getLocations().end();
    std::cout << "EFETUANDO: " << method << " REQUEST PARA ROOT: " << effectiveRoot << " E URL: " << effectiveUrl << " NO SOCKET: " << this->getConnectFd() << std::endl;
    
    // Remover a barra final se o URL não for a raiz e terminar com '/'
	//NORMATIZAR OS URL
	if (effectiveUrl != "/" && effectiveUrl[effectiveUrl.length() - 1] == '/')
   		effectiveUrl.erase(effectiveUrl.length() - 1);

	//ENCONTRAR LOCATION CERTO
	if (Utils::_isRoot(effectiveUrl))
		this->_rightLocation = this->_myServer->getLocations().find("/");
 	else
	{
		std::map<std::string, location_t>::const_iterator curLocation = this->_myServer->getLocations().begin();
		for (; curLocation != this->_myServer->getLocations().end(); curLocation++)
		{
			if (Utils::_isRightLocation(curLocation->first, effectiveUrl))//**MELHORAR A VERIFICACAO
			{
				//std::cout << "LOCATION: " << this->_rightLocation->first << " ACHADO PARA: " << effectiveUrl << std::endl;
				this->_rightLocation = curLocation;
			}
		}
	} 

	//VER SE TEM LOCATION PARA EXECUTAR O METODO
	if (method != "GET" && this->_rightLocation == this->_myServer->getLocations().end())
		throw std::runtime_error(Utils::_defaultErrorPages(403, "Nao foi encontrado location para o metodo solicitado"));

	//VERIFICAR SE O LOCATION TEM REDIRECT OU NOVO ROOT
	if (this->_rightLocation != this->_myServer->getLocations().end())
	{
		std::cout << "LOCATION: " << this->_rightLocation->first << " ACHADO PARA: " << effectiveUrl << std::endl;
		const location_t &loc = this->_rightLocation->second;
		if (!loc.redirect.empty())
		{
			effectiveUrl = loc.redirect;
			std::cout << "REDIRECIONANDO DE: " << url << " PARA: " << effectiveUrl << std::endl;
			if (this->_urlLoopCount > 3)
				throw std::runtime_error(Utils::_defaultErrorPages(508, "Foi detectado um loop de redirecionamentos."));
			this->_processRequest(effectiveUrl, method, root);
			return;
		}
		if (!loc.root.empty())
		{
			std::cout << "ROTEANDO DE: " << root << " PARA: " << effectiveRoot << std::endl;
			effectiveRoot = loc.root;
		}
		if (!Utils::isMethodAllowed(loc.methods, method))
		{
			throw std::runtime_error(Utils::_defaultErrorPages(405, "Metodo nao permitido pelo location"));
			return;
		}
	}

//-----------------------------------------------------------------------------------------

	//LIMPEZA DE ROOTS E PATHS PARA CONCATENACAO
	if (!effectiveUrl.empty() && effectiveUrl[0] == '/')
	{
        effectiveUrl.erase(0, 1);
    }
	if ((method == "POST" || method == "GET") && Utils::isExtensionOf(".py", effectiveUrl))
	{
		if (this->_rightLocation->second.cgiPath.empty())
			throw std::runtime_error(Utils::_defaultErrorPages(403, "Nao foi encontrado path para cgi no location"));
		if (this->_rightLocation->second.cgiPath[0] == '/')
		{
			effectiveCgiPath = this->_rightLocation->second.cgiPath;
			effectiveCgiPath = effectiveCgiPath.erase(0, 1);
		}
		fullPath = effectiveRoot + effectiveCgiPath + effectiveUrl;
	}
	else
	{
		fullPath = effectiveRoot + effectiveUrl;
	}


	std::cout << "FULLPATH: " << fullPath << std::endl;
//--------------------------------------------------------------------------------------------------- 
	//EXECUCAO DOS METODOS
    if (method == "GET")
	{
        std::cout << "EFETUANDO METODO GET PARA PATH: " << fullPath << std::endl; 
		if (Utils::isExtensionOf(".py", fullPath))
		{
			if (!Utils::fileExists(fullPath))
			{
				if (this->_tryServeErrorPage(effectiveRoot))
					return;
				throw std::runtime_error(Utils::_defaultErrorPages(404, "O servidor nao possui permissao para abrir o diretorio."));
			}
			CGI getCgi(fullPath, "", "", 0);
			this->_effectiveUpload = Utils::_getTimeStamp("%H:%M:%S");
			this->_exportEnviron(getCgi);
			getCgi.execute();
			std::string response = Utils::autoHTML("200", "OK","cgi.html");
			write(this->_connect_fd, response.c_str(), response.length()); 
			return;
		}
		if (Utils::directoryExists(fullPath))
		{
			std::cout << "PATH: " << fullPath << " EH UM DIR!\n";
			//GET PARA DIRETORIO
			if (this->_rightLocation != this->_myServer->getLocations().end() && !this->_rightLocation->second.tryFile.empty() && this->_serveTryFile((effectiveRoot + this->_rightLocation->second.tryFile), effectiveRoot, this->getConnectFd())) //guardar o location no objeto e verificar se há um location
			{
                return;
            }
			else if (this->_rightLocation != this->_myServer->getLocations().end() && this->_rightLocation->second.autoindex)
			{
				//std::cout << "TENTANDO AUTOINDEX EM: " << this->_rightLocation->second.autoindex << std::endl;
                this->_serveAutoindex(fullPath, effectiveRoot, this->getConnectFd());
				return ;
            }
            else if (Utils::fileExists(fullPath + "index.html"))
			{
                this->_serveFile((fullPath + "index.html"), effectiveRoot, this->getConnectFd());
            }
			else
			{
                throw std::runtime_error(Utils::_defaultErrorPages(403, "Nenhum arquivo index.html foi encontrado no diretorio"));
            }
        }
		else if (Utils::fileExists(fullPath))
		{
            this->_serveFile(fullPath, effectiveRoot, this->getConnectFd());
        }
		else
		{
			if (this->_tryServeErrorPage(effectiveRoot))
				return ;
            throw std::runtime_error(Utils::_defaultErrorPages(404, "O diretorio ou arquivo nao foi encontrado."));
        }
    }//------------------------------------------------------------------------------------------------------------
	else if (method == "POST") //METODO POST
	{
		if (this->_rightLocation->second.uploadTo.empty())
			throw std::runtime_error(Utils::_defaultErrorPages(501, "Nenhum diretorio definido para upload"));
		if (!Utils::fileExists(fullPath))
		{
			if (this->_tryServeErrorPage(effectiveRoot))
				return;
			throw std::runtime_error(Utils::_defaultErrorPages(404, "O servidor nao possui permissao para abrir o diretorio."));
		}
		this->_effectiveUpload = effectiveRoot + this->_rightLocation->second.uploadTo;
        //std::cout << "EFETUANDO METODO POST PARA PATH: " << fullPath << std::endl; 
        this->_fullPath = fullPath;
		//std::cout << "REQUEST ANTES DE MANDAR PRO CGI: " << this->_myRequest.getRequest() << std::endl;
		CGI postCgi(fullPath, _effectiveUpload, this->_myRequest.getRequest(), this->_myRequest.getBodyLength());
		this->_exportEnviron(postCgi);
		postCgi.execute();
		std::string response = Utils::autoHTML("200", "OK","cgi.html");
		write(this->_connect_fd, response.c_str(), response.length()); 
    }//------------------------------------------------------------------------------------------------------------
	else if (method == "DELETE") //METODO DELETE
	{
		if (remove(fullPath.c_str()) == 0)
		{
			std::string response = Utils::autoHTML("200", "OK", "");
			write(this->_connect_fd, response.c_str(), response.length());
		}
/* 		else //SE A REMOÇÃO DO ARQUIVO FALHAR 
		{
			std::string response = Utils::autoHTML("400", "OK", "");
			write(this->_connect_fd, response.c_str(), response.length());
		} */
	}//------------------------------------------------------------------------------------------------------------
	else
	{
        throw std::runtime_error(Utils::_defaultErrorPages(405, "Os metodos permitidos sao GET, POST e DELETE."));
    }
}
