/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connect.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pin3dev <pinedev@outlook.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 19:04:31 by pin3dev           #+#    #+#             */
/*   Updated: 2024/06/08 21:55:39 by pin3dev          ###   ########.fr       */
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
_myServer(&server), _effectiveUpload(""), _fullPath("")
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
		//std::cout << e.what() << std::endl;
		//std::string ErrorResponse;
		//ErrorResponse = Utils::autoHTML(e.what(), "Error", "www/site1/404.html");//**TEMPORARIO
		
		std::string myWhat = e.what();
		write(this->_connect_fd, e.what(), myWhat.size());
		//this->_myResponse.errorHTML(e.what()); //**MUDAR PARA UMA FUNCAO AUTOMATICA
		//**ACHO QUE TENHO QUE FECHAR A CONEX'AO AQUI
	}
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


void serveFile(const std::string &fullPath, int _connectSocket)
{
	std::ifstream file(fullPath.c_str(), std::ios::binary | std::ios::in);

	if (!file.is_open()) //**ver outra forma de verificar se o arquivo pode ser aberto
	{
		write(_connectSocket, "HTTP/1.1 404 Not Found\n", 24);
		return;
	}
	file.close();

	std::string response = Utils::autoHTML("200", "OK", fullPath);
	write(_connectSocket, response.c_str(), response.length()); 
}

/* 
// Função para processar POST
void processPost(const std::string &path) {
    // Suponha que estamos recebendo dados para salvar em um arquivo
    // Esta parte deve lidar com a leitura do corpo da requisição e salvar o conteúdo no path
    //std::cout << "Processing POST request, saving data to: " << path << std::endl;
    // Implementação fictícia para salvar dados
    // Aqui você colocaria a lógica real para ler o corpo do POST e salvar o conteúdo
}

// Função para processar DELETE
void processDelete(const std::string &path) {
    //std::cout << "Processing DELETE request, deleting data from: " << path << std::endl;
} */

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
}


//VARIAVEIS IMPORTANTES CRIADAS:
/*
-RIGHTLOCATION
-EFECTIVEROOT 
-EFFECTIVEURL
-FULLPATH
-EFFECTIVECGIPATH

*/
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
	
	//ENCONTRAR LOCATION CERTO
	if (Utils::_isRoot(effectiveUrl))
		this->_rightLocation = this->_myServer->getLocations().find("/");
 	else
	{
		std::map<std::string, location_t>::const_iterator curLocation = this->_myServer->getLocations().begin();
		for (; curLocation != this->_myServer->getLocations().end(); curLocation++)
		{
			if (Utils::_isRightLocation(curLocation->first, effectiveUrl))
			{
				this->_rightLocation = curLocation;
				break;
			}
		}
	} 

	//VER SE TEM LOCATION PARA EXECUTAR O METODO
	if (method != "GET" && this->_rightLocation == this->_myServer->getLocations().end())
		throw std::runtime_error(Utils::_defaultErrorPages(403, "Nao foi encontrado location para o metodo solicitado"));

	//VERIFICAR SE O LOCATION TEM REDIRECT OU NOVO ROOT
	if (this->_rightLocation != this->_myServer->getLocations().end())
	{
		//std::cout << "LOCATION: " << this->_rightLocation->first << " ACHADO PARA: " << effectiveUrl << std::endl;
		const location_t &loc = this->_rightLocation->second;
		if (!loc.redirect.empty())
		{
			effectiveUrl = loc.redirect;
			//std::cout << "REDIRECIONANDO DE: " << url << " PARA: " << effectiveUrl << std::endl;
			if (this->_urlLoopCount > 3)
				throw std::runtime_error(Utils::_defaultErrorPages(508, "Foi detectado um loop de redirecionamentos."));
			this->_processRequest(effectiveUrl, method, root);
			return;
		}
		if (!loc.root.empty())
		{
			//std::cout << "ROTEANDO DE: " << root << " PARA: " << effectiveRoot << std::endl;
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
    if (method == "GET") //METODO GET
	{
		if (Utils::isExtensionOf(".py", fullPath))
		{
			CGI getCgi(fullPath, "", "", 0);
			this->_effectiveUpload = Utils::_getTimeStamp("%H:%M:%S");
			this->_exportEnviron(getCgi);
			getCgi.execute();
			std::string response = Utils::autoHTML("200", "OK","cgi.html");
			write(this->_connect_fd, response.c_str(), response.length()); 
		}
			
		//VERIFICAR SE O ARQUIVO EH SCRIPT
        //std::cout << "EFETUANDO METODO GET PARA PATH: " << fullPath << std::endl; 
        if (Utils::directoryExists(fullPath))
		{
			//GET PARA DIRETORIO
			if (this->_rightLocation != this->_myServer->getLocations().end() && !this->_rightLocation->second.tryFile.empty() && serveTryFile((effectiveRoot + this->_rightLocation->second.tryFile), this->getConnectFd())) //guardar o location no objeto e verificar se há um location
			{
                return;
            }
			else if (this->_rightLocation != this->_myServer->getLocations().end() && this->_rightLocation->second.autoindex)
			{
				//std::cout << "TENTANDO AUTOINDEX EM: " << this->_rightLocation->second.autoindex << std::endl;
                serveAutoindex(fullPath, this->getConnectFd());
            }
            else if (Utils::fileExists(fullPath + "index.html"))
			{
                serveFile((fullPath + "index.html"), this->getConnectFd());
            }
			else
			{
                throw std::runtime_error(Utils::_defaultErrorPages(403, "Nenhum arquivo index.html foi encontrado no diretorio"));
            }
        }
		else if (Utils::fileExists(fullPath))
		{
			//GET PARA FILE
			//verifica se eh um script e executa o script
            serveFile(fullPath, this->getConnectFd());
        }
		else
		{
            throw std::runtime_error(Utils::_defaultErrorPages(404, "O diretorio ou arquivo nao foi encontrado."));
        }
    }//------------------------------------------------------------------------------------------------------------
	else if (method == "POST") //METODO POST
	{
		if (this->_rightLocation->second.uploadTo.empty())
			throw std::runtime_error(Utils::_defaultErrorPages(501, "Nenhum diretorio definido para upload"));
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
		else //SE A REMOÇÃO DO ARQUIVO FALHAR 
		{
			std::string response = Utils::autoHTML("400", "OK", "");
			write(this->_connect_fd, response.c_str(), response.length());
		}
	}//------------------------------------------------------------------------------------------------------------
	else
	{
        throw std::runtime_error(Utils::_defaultErrorPages(405, "Os metodos permitidos sao GET, POST e DELETE."));
    }
}
