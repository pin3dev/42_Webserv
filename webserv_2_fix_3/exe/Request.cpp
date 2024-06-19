
#include "Request.hpp"
#include "Utils.hpp"

std::string const Request::_generalAllowedMethods[3] = {"GET", "POST", "DELETE"};
std::string const Request::_HTTPunsupported[3] = {"HTTP/0.9", "HTTP/1.0", "HTTP/2.0"};

/** 
 * **************************************
 * SECTION - CONSTRUCTORS & DESTRUCTORS
 * **************************************
*/

Request::Request() : _hasHeaders(false), _hasPayload(false)
{
    this->_version.clear();
    this->_url.clear();
    this->_method.clear();
    this->_host.clear();
    this->_payload.clear();
    this->_bodyLength = 0;
    this->_headerData.clear();
    this->_maxLength = 0; 
}

Request::~Request()
{}

/** 
 * **************************
 * SECTION - PUBLIC METHODS
 * **************************
*/

void Request::setHasHeaders(char *buffer, size_t size)
{
    std::string request(buffer, size);
    //std::cout << "\n\n\r\nREQUEST PART\n\n\r\n" << request << std::endl;
    std::stringstream ss(request);
    std::string curLine;

    //this->_setMethodAndURL(ss);
    ss >> this->_method >> this->_url >> this->_version;
    this->_url = Utils::_decoder(this->_url);

    //this->_setHeaders(ss);
    //std::cout << "\nHEADERS:\n" << request << std::endl;
	std::getline(ss, curLine, '\r');
    while (std::getline(ss, curLine, '\r') && curLine != "\n")
	{
		if (curLine.find(':') != std::string::npos) 
		{
			std::string key = curLine.substr(0, curLine.find(':'));
			std::string value = curLine.substr(curLine.find(':') + 1);
			Utils::_trim(key, " \t\r\v\f\n");
			Utils::_trim(value, " \t\r\v\f\n");
			this->_headerData.insert(make_pair(key, value));

		}
	}

    //this->_setHost();
/*         std::map<std::string, std::string>::iterator it =  this->_headerData.begin();
        for (; it != this->_headerData.end(); it++)
            std::cout << it->first << " : " << it->second << std::endl;
            //Utils::print_invisibles(it->second); */
    std::string hostAndPort = this->_headerData["Host"];
    //std::cout <<"HOST INTEIRO: " << hostAndPort<< std::endl;
    size_t sep = hostAndPort.find(':');
    if (sep != std::string::npos)
        this->_host = hostAndPort.substr(0, sep);
    else
        this->_host = hostAndPort;

    this->_hasHeaders = true;

    /*     std::cout << "CURLINE: ";
        Utils::print_invisibles(curLine) */;
    if (curLine != "\n")
        return ;

    /* std::cout << "METODO: " << _method << std::endl;
    std::cout << "URL: " << _url << std::endl;
    std::cout << "VERSAO: " << _version << std::endl;
    std::cout << "HOST: " << _host << std::endl; */
    
    size_t pos = 0;
    while (request.compare(pos, 4, "\r\n\r\n") != 0)
        pos++;
    pos += 4;

    if(this->isChunked())
    {
        this->setPayload(buffer + pos, size - pos);
        this->handleChunked();
    }
    else if (this->_method == "POST")
    {
        this->setPayload(buffer + pos, size - pos);
    }
}

/* oid Request::_setMethodAndURL(std::stringstream &ss)
{

    std::string		firstLine;
    ss >> this->_method >> this->_url >> this->_version;
    this->_url = Utils::_decoder(this->_url);
}

void	Request::_setHeaders(std::stringstream &ss)
{
	std::string curLine;
	while (std::getline(ss, curLine))
	{
		if (curLine == "\r" || curLine == "\r\n" || curLine.empty()) 
        	break;
		if (curLine.find(':') != std::string::npos) 
		{
			std::string key = curLine.substr(0, curLine.find(':'));
			std::string value = curLine.substr(curLine.find(':') + 1, curLine.find('\n'));
			Utils::_trim(key, " \t\r\v\f");
			Utils::_trim(value, " \t\r\v\f");
			this->_headerData[key] = value;
		}
	}
}

void	Request::_setHost()
{
    std::string hostAndPort = this->_headerData["Host"];
    size_t pos = hostAndPort.find(':');
    if (pos != std::string::npos)
        this->_host = hostAndPort.substr(0, pos);
    else
        this->_host = hostAndPort;
} */


void Request::setPayload(char *buffer, size_t size)
{
    for (size_t i = 0; i < size; i++)
        this->_payload.push_back(buffer[i]); //armazena buffer

    for (size_t i = 0; i < sizeof(buffer); ++i)//limpa buffer
        buffer[i] = '\0';
}

void    Request::handleChunked()
{
    if (!Utils::findChunkedEOF(this->_payload))
        return;


    std::vector<char> curChunked = this->_payload;
    this->_payload.clear();

	size_t chunkCharSize = Utils::countCurChunkedSize(curChunked, 0);
	size_t chunkSize = Utils::countFullChunkedSize(curChunked, 0);
	size_t pos = chunkCharSize + 2;
	while (chunkSize > 0 && pos < curChunked.size())
	{
		for (size_t i = 0; i < chunkSize; i++)
			this->_payload.push_back(curChunked[pos + i]);
		pos += chunkSize + 2;
		chunkCharSize = Utils::countCurChunkedSize(curChunked, pos);
		chunkSize = Utils::countFullChunkedSize(curChunked, pos);
		pos += chunkCharSize + 2;
	}

/*     std::cout << "PAYLOAD CHUNKED: ";
    std::cout << this->_payload.size() << std::endl;
    std::cout << Utils::vecToString(this->_payload) << std::endl; */
	this->_hasPayload = true;
}

void Request::checkRequestContent()
{
    bool hasinvalidAmount = (this->_method.empty() || this->_url.empty() || this->_version.empty());
    bool hasAllowedMethod = Utils::_isAt(this->_method, this->_generalAllowedMethods);
    bool hasSensibleURL = this->_url.find("../") != std::string::npos && this->_url == "..";
    bool hasValidURLSize = this->_url.length() <= 1024;
    bool hasUnsupportedHTTP = Utils::_isAt(this->_version, this->_HTTPunsupported);
    bool hasValidHTTP = std::strcmp(this->_version.c_str(), "HTTP/1.1") == 0;
    bool isNormalPost = this->_method == "POST" && this->_headerData.find("Content-Length") != this->_headerData.end();
    bool isAnormalPost = this->_method == "POST" && this->_headerData.find("Content-Length") == this->_headerData.end();

    if (hasinvalidAmount)
        throw std::runtime_error(Utils::_defaultErrorPages(400, "Erro na primeira linha do request."));
    if (!hasAllowedMethod)
        throw std::runtime_error(Utils::_defaultErrorPages(501, "Os metodos permitidos sao: GET, POST e DELETE."));
    if (hasSensibleURL)
        throw std::runtime_error(Utils::_defaultErrorPages(400, "O URL requisitado tem um caminho sensivel como: .. ou ../"));
    if (!hasValidURLSize)
        throw std::runtime_error(Utils::_defaultErrorPages(414, "O URL requisitado passa do tamanho limite seguro."));
    if (hasUnsupportedHTTP)
        throw std::runtime_error(Utils::_defaultErrorPages(505, "A unica versao suportada para esse servidor e o HTTP 1.1"));
    if (!hasValidHTTP)
        throw std::runtime_error(Utils::_defaultErrorPages(400, "Versao HTTP errada."));

    std::map<std::string, std::string>::iterator it = this->_headerData.begin();
    for (; it != this->_headerData.end(); it++)
    {
        if (it->first.empty() || it->second.empty())
            throw std::runtime_error(Utils::_defaultErrorPages(406, "Header do request foge a estrutura key:value"));
    }

    if (isNormalPost)
    {
        std::string const lenght = this->_headerData["Content-Length"];
    
        bool isDigit = lenght.find_first_not_of("0123456789") == std::string::npos;
        if (!isDigit)
            throw std::runtime_error(Utils::_defaultErrorPages(409, "Request com valor de Content-Length mal definido")); //response
        
        char * end;
        unsigned long long size = std::strtoull(lenght.c_str(), &end, 10);
       // std::cout << "SIZE DO PAYLOAD: " << size << std::endl;
       // std::cout << "SIZE MAX: " << this->getMaxLength() << std::endl;
        //std::cout << "SIZE DO ULLONG_MAX: " << static_cast<unsigned long long>(ULLONG_MAX) << std::endl;
        if (size >  static_cast<unsigned long long>(ULLONG_MAX))
            throw std::runtime_error(Utils::_defaultErrorPages(413, "Request com Payload maior do que valor limite")); //response
        //std::cout << "SIZE: " << size << std::endl;
        
        if (end == lenght.c_str())
            throw std::runtime_error(Utils::_defaultErrorPages(500, "Problema na conversao do Content-Length definido"));

        if (size > this->getMaxLength())
            throw std::runtime_error(Utils::_defaultErrorPages(413, "Request com Payload maior que o definido pelo Servidor")); //response

        this->_bodyLength = static_cast<size_t>(size);
    }
    if (isAnormalPost)
    {
        if (!this->isChunked())
            throw std::runtime_error(Utils::_defaultErrorPages(411, "Request precisa de um payload com tamanho pre-determinado.")); //response
    }
    if (this->_method == "POST")
    {
        if (this->_headerData["Content-Type"].empty() ||
	    (this->_headerData["Content-Type"].find("multipart/form-data") == std::string::npos && 
		this->_headerData["Content-Type"].find("plain/text") == std::string::npos &&
		this->_headerData["Content-Type"].find("image/jpeg") == std::string::npos &&
		this->_headerData["Content-Type"].find("image/png") == std::string::npos &&
		this->_headerData["Content-Type"].find("application/pdf") == std::string::npos &&
		this->_headerData["Content-Type"].find("application/json") == std::string::npos &&
	    this->_headerData["Content-Type"].find("application/octet-stream") == std::string::npos &&
		this->_headerData["Content-Type"].find("video/mp4") == std::string::npos &&
		this->_headerData["Content-Type"].find("audio/mpeg") == std::string::npos ))
        {
           throw std::runtime_error(Utils::_defaultErrorPages(415, "Request foi feito para um tipo de midia nao suportada pelo servidor.")); //response
        }
    }

    /*VERIFICAR O TAMANHO DO HEADER?*/
}



/** 
 * **************************
 * SECTION - SETTERS/CHECKERS
 * **************************
*/


/* void	Request::checkStatusRequest()
{
	std::stringstream	ss(this->_request);
    if (this->getFlagToFirstRead())
    {
        this->_setMethodAndURL(ss);
        this->_setHeaders(ss);
        this->_setHost();
    }
	if (this->_method == "POST")
        this->_setPayload(ss);
} */

/* void Request::_setMethodAndURL(std::stringstream &ss)
{
    std::string		firstLine;
    std::getline(ss, firstLine);
    std::vector<std::string> method_URL_version = Utils::_split(firstLine, ' ');

    bool hasValidAmount = method_URL_version.size() == 3;
    bool hasAllowedMethod = Utils::_isAt(method_URL_version[0], this->_generalAllowedMethods);
    bool hasSensibleURL = method_URL_version[1].find("../") != std::string::npos && method_URL_version[1] == "..";
    bool hasValidURLSize = method_URL_version[1].length() <= 1024;
    bool hasUnsupportedHTTP = Utils::_isAt(method_URL_version[2], this->_HTTPunsupported);
    bool hasValidHTTP = std::strcmp(method_URL_version[2].c_str(), "HTTP/1.1\r") == 0;

    if (!hasValidAmount)
        throw std::runtime_error(Utils::_defaultErrorPages(400, "Erro na primeira linha do request."));
    if (!hasAllowedMethod)
        throw std::runtime_error(Utils::_defaultErrorPages(501, "Os metodos permitidos sao: GET, POST e DELETE."));
    if (hasSensibleURL)
        throw std::runtime_error(Utils::_defaultErrorPages(400, "O URL requisitado tem um caminho sensivel como: .. ou ../"));
    if (!hasValidURLSize)
        throw std::runtime_error(Utils::_defaultErrorPages(414, "O URL requisitado passa do tamanho limite seguro."));
    if (hasUnsupportedHTTP)
        throw std::runtime_error(Utils::_defaultErrorPages(505, "A unica versao suportada para esse servidor e o HTTP 1.1"));
    if (!hasValidHTTP)
        throw std::runtime_error(Utils::_defaultErrorPages(400, "Versao HTTP errada."));

    this->_method = method_URL_version[0];
    this->_url = Utils::_decoder(method_URL_version[1]);
} */

/* void	Request::_setHeaders(std::stringstream &ss)
{
	std::string curLine;
	while (std::getline(ss, curLine))
	{
		if (curLine == "\r" || curLine == "\r\n" || curLine.empty()) 
        	break;
		if (curLine.find(':') != std::string::npos) 
		{
			std::string key = curLine.substr(0, curLine.find(':'));
			std::string value = curLine.substr(curLine.find(':') + 1, curLine.find('\n'));
			Utils::_trim(key, " \t\r\v\f");
			Utils::_trim(value, " \t\r\v\f");

			if (value.length() == 0 || key.length() == 0)
				throw std::runtime_error(Utils::_defaultErrorPages(406, "Header do request foge a estrutura key:value"));
			this->_headerData[key] = value;
		}
	}
} */

/* void	Request::_setHost()
{
    std::string hostAndPort = this->_headerData["Host"];
    size_t pos = hostAndPort.find(':');
    if (pos != std::string::npos)
        this->_host = hostAndPort.substr(0, pos);
    else
        this->_host = hostAndPort;
} */

/* void    Request::_setPayload(std::stringstream &ss)
{
    this->_setBodyLength();

    if (!this->getFlagToFirstRead())
    {
        std::string line;
        while (std::getline(ss, line))
        {
            if (line == "\r")
                break;
        }
    }
    
    std::streampos pos = ss.tellg();
    if (pos == -1)
        throw std::runtime_error("Nothing else to read. No Payload!\n");
	std::stringstream binarySs(ss.str(), std::stringstream::in | std::stringstream::binary);
	binarySs.seekg(pos);

    std::string tempPayload(this->getBodyLength(), '\0');
    binarySs.read(&tempPayload[0], this->getBodyLength());

    std::streamsize bytesRead = binarySs.gcount();
    if (bytesRead < static_cast<std::streamsize>(this->getBodyLength()))
    {
        this->setReadyToResponse(false);
        this->setFirstRead(false);
        return ;
    }
    if (this->_headerData["Content-Type"] == "plain/text")
        this->_payload.assign(tempPayload.begin(), tempPayload.end());
	else
    {
        this->_payload.resize(this->getBodyLength()); 
        binarySs.read(&this->_payload[0], this->getBodyLength());
    }
} */

/* void    Request::_setBodyLength()
{
    bool hasContentLength = this->_headerData.find("Content-Length") != this->_headerData.end();
    if (!hasContentLength)
        throw std::runtime_error(Utils::_defaultErrorPages(411, "Request sem Content-Length definido"));
    
    std::string const lenght = this->_headerData["Content-Length"];
    
    bool isDigit = lenght.find_first_not_of("0123456789") == std::string::npos;
    if (!isDigit)
        throw std::runtime_error(Utils::_defaultErrorPages(409, "Request com Content-Length mal definido")); //response
    
    size_t size = std::atoi(lenght.c_str());
    if (size > this->getMaxLength())
        throw std::runtime_error(Utils::_defaultErrorPages(413, "Request com Payload maior que o definido pelo Content-Length")); //response

    this->_bodyLength = size;
} */

/** 
 * **************************
 * SECTION - GETTERS/SETTERS
 * **************************
*/

std::string Request::getVersion() const {return (this->_version);}
std::string Request::getURL() const {return (this->_url);}
std::string Request::getMethod() const {return (this->_method);}
std::string Request::getHost() const {return (this->_host);}
std::vector<char> &Request::getPayload() {return (this->_payload);}
size_t  Request::getBodyLength() const {return (this->_bodyLength);}
size_t Request::getMaxLength() const {return (this->_maxLength);}
std::map<std::string, std::string> Request::getHeadData() const {return (this->_headerData);}
bool	Request::hasHeaders() const {return (this->_hasHeaders);}
bool	Request::hasPayload() const {return (this->_hasPayload);}

void   Request::setMaxLength(size_t length){this->_maxLength = length;}

bool    Request::isChunked()
{
    if (this->_headerData.find("Transfer-Encoding") == this->_headerData.end())
    {
        return (false);
    }
    std::string chunked = this->_headerData["Transfer-Encoding"];
    return (!chunked.empty() && chunked == "chunked");
}

bool    Request::isReadyToResponse()
{
    if (!this->hasHeaders())
        return (false);
    if (this->isChunked())
    {
        if (!this->hasPayload())
            return (false);
    }
    if (this->getPayload().size() < this->_bodyLength)
        return (false);
    return (true);
}




