
#include "Connect.hpp"
#include "Utils.hpp"

/** 
 * **************************************
 * SECTION - CONSTRUCTORS & DESTRUCTORS
 * **************************************
*/

Connect:: Connect(Server &server, int connect_fd) : _connect_fd(connect_fd), _urlLoopCount(0), 
_myServer(&server), _fullPath(""), _effectiveUpload(""), _isRedirect(false)
{
    this->_updated = Utils::_nowTime();
	this->_rightLocation = this->_myServer->getLocations().end();
	this->_cgiLocation = this->_myServer->getLocations().find(".py");
    this->_myRequest.setMaxLength(_myServer->getClientMaxBodySize());
}

Connect::~Connect()
{
	if (Utils::isReadeableFile("cgi.html"))
		remove("cgi.html");
	if (Utils::isReadeableFile(".inputFile"))
		remove(".inputFile");
}

/** 
 * **************************
 * SECTION - PUBLIC METHODS
 * **************************
*/


void	Connect::runRequest()
{
    this->setUpdate(Utils::_nowTime()); 
	try
	{
		this->getRequest().checkRequestContent();
		this->_processRequest(this->getRequest().getURL(), this->getRequest().getMethod(), this->getServer().getRoot()); 
	}
	catch(const std::exception& e)
	{
		std::string myWhat = e.what();
		write(this->_connect_fd, e.what(), myWhat.size());
	}
	this->_urlLoopCount = 0;
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
void	Connect::setUpdate(time_t now){this->_updated = now;}

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


void Connect::searchForNonDefaultServer(std::vector<Server> &VecServers)
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
			break;
		}	
	}
}

/** 
 * **************************
 * SECTION - PRIVATE METHODS
 * **************************
*/


bool Connect::_tryServeErrorPage(const std::string &effectiveRoot)
{
	if (this->_myServer->getErrorPage().empty())
		return false;
	std::string fullErrorPath = effectiveRoot + this->_myServer->getErrorPage();
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
	if (Utils::isReadeableFile(fullPath) == false)
	{
		if (this->_tryServeErrorPage(effectiveRoot))
			return ;
		throw std::runtime_error(Utils::_defaultErrorPages(404, "O servidor nao possui permissao para abrir o arquivo."));
	}
	std::string response;
	if (this->_isRedirect == true)
		response = Utils::autoHTML("307", "temporary redirect”", fullPath);
	else
		response = Utils::autoHTML("200", "OK", fullPath);
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
	cgi.setNewEnv("CHUNKED_CONTENT", this->_myRequest.getHeadData()["Expect"]);
	cgi.setNewEnv("PAYLOAD", Utils::vecToString(this->_myRequest.getPayload()));
}


void Connect::_processRequest(const std::string &url, const std::string &method, const std::string &root)
{
	this->_urlLoopCount++;
    std::string effectiveRoot = root; 
    std::string effectiveUrl = url;
	std::string effectiveCgiPath;
	std::string fullPath;
    
	if (effectiveUrl != "/" && effectiveUrl[effectiveUrl.length() - 1] == '/')
   		effectiveUrl.erase(effectiveUrl.length() - 1);

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
			}
		}
	} 

	if (method != "GET" && this->_rightLocation == this->_myServer->getLocations().end())
		throw std::runtime_error(Utils::_defaultErrorPages(403, "Nao foi encontrado location para o metodo solicitado"));

	if (this->_rightLocation != this->_myServer->getLocations().end())
	{
		const location_t &loc = this->_rightLocation->second;
		if (!loc.redirect.empty())
		{
			effectiveUrl = loc.redirect;
			if (this->_urlLoopCount > 3)
				throw std::runtime_error(Utils::_defaultErrorPages(508, "Foi detectado um loop de redirecionamentos."));
			this->_isRedirect = true;
			this->_processRequest(effectiveUrl, method, root);
			return;
		}
		if (!loc.root.empty())
		{
			effectiveRoot = loc.root;
		}
		if (!Utils::isMethodAllowed(loc.methods, method))
		{
			throw std::runtime_error(Utils::_defaultErrorPages(405, "Metodo nao permitido pelo location"));
			return;
		}
	}

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
 
    if (method == "GET")
	{
		if (Utils::isExtensionOf(".py", fullPath))
		{
			if (!Utils::fileExists(fullPath))
			{
				if (this->_tryServeErrorPage(effectiveRoot))
					return;
				throw std::runtime_error(Utils::_defaultErrorPages(404, "O servidor nao possui permissao para abrir o diretorio."));
			}
			std::vector<char> tmp;
			CGI getCgi(fullPath, "", tmp, 0);
			this->_effectiveUpload = Utils::_getTimeStamp("%H:%M:%S");
			this->_exportEnviron(getCgi);
			getCgi.execute();
			std::string response = Utils::getFileContent("cgi.html");
			//std::string response = Utils::autoHTML("200", "OK","cgi.html");
			if (write(this->_connect_fd, response.c_str(), response.length()) == -1)
				throw std::runtime_error(Utils::_defaultErrorPages(504, "O servidor nao foi capaz de responder ao request"));
			return;
		}
		if (Utils::directoryExists(fullPath))
		{
			if (this->_rightLocation != this->_myServer->getLocations().end() && !this->_rightLocation->second.tryFile.empty() && this->_serveTryFile((effectiveRoot + this->_rightLocation->second.tryFile), effectiveRoot, this->getConnectFd())) //guardar o location no objeto e verificar se há um location
			{
                return;
            }
			else if (this->_rightLocation != this->_myServer->getLocations().end() && this->_rightLocation->second.autoindex)
			{
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
    }
	else if (method == "POST") 
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
        this->_fullPath = fullPath;
		CGI postCgi(fullPath, _effectiveUpload, this->_myRequest.getPayload(), this->_myRequest.getBodyLength());
		this->_exportEnviron(postCgi);
		postCgi.execute();
		//std::string response = Utils::autoHTML("200", "OK","cgi.html");
		std::string response = Utils::getFileContent("cgi.html");
		if (write(this->_connect_fd, response.c_str(), response.length()) == -1)
			throw std::runtime_error(Utils::_defaultErrorPages(504, "O servidor nao foi capaz de responder ao request"));
    }
	else if (method == "DELETE")
	{
		if (remove(fullPath.c_str()) == 0)
		{
			std::string response = Utils::autoHTML("200", "OK", "");
			write(this->_connect_fd, response.c_str(), response.length());
		}
		else
		{
			throw std::runtime_error(Utils::_defaultErrorPages(400, "O arquivo nao foi encontrado no diretorio /upload."));
		}
	}
	else
	{
        throw std::runtime_error(Utils::_defaultErrorPages(405, "Os metodos permitidos sao GET, POST e DELETE."));
    }
}
