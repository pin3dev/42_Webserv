
#include "Utils.hpp"

std::map<int, std::string> Utils::_errorHTML = Utils::initErrorMap();

std::map<int, std::string> Utils::initErrorMap()
{
    std::map<int, std::string> m;

	m[400] = "Bad Request";
	m[401] = "Unauthorized";
	m[403] = "Forbidden";
	m[404] = "Not Found";
	m[405] = "Method Not Allowed";
	m[406] = "Not Acceptable";
	m[408] = "Request Timeout";
	m[409] = "Conflict";
	m[411] = "Length Required";
	m[413] = "Payload Too Large";
	m[414] = "URI Too Long";
	m[415] = "Unsupported Media Type";
	m[500] = "Internal Server Error";
	m[501] = "Not Implemented";
	m[504] = "Gateway Timeout";
	m[505] = "HTTP Version Not Supported";
	m[508] = "Loop Detected";
	return m;
}

time_t Utils::_nowTime()
{
    return (std::time(NULL));
}
// getTimeStamp() "%a, %d %b %Y %H:%M:%S GMT"
// getTime() "%H:%M:%S"
std::string Utils::_getTimeStamp(const char *format)
{
	time_t		now = _nowTime();
	struct tm	tstruct;
	char		buf[80];

	tstruct = *localtime(&now); 
	strftime(buf, sizeof(buf), format, &tstruct);
	return (buf);
}

std::vector<std::string> Utils::_split(std::string const &str, char delim)
{
	std::vector<std::string> tokens;
	std::stringstream ss(str);
	std::string token;

	while (std::getline(ss, token, delim))
	{
		if (!token.empty())
			tokens.push_back(token);
	}
	return tokens;
}

void Utils::_trim(std::string &s, char const *set)
{
	s.erase(0, s.find_first_not_of(set));
	s.erase(s.find_last_not_of(set) + 1);
}

std::string Utils::_decoder(const std::string &url)
{
	std::string tmp = url;
	size_t pos;

	while ((pos = tmp.find("%20")) != std::string::npos)
		tmp.replace(pos, 3, " ");
    return tmp;
}

bool Utils::_isAt(const std::string &str, const std::string arr[3])
{
    for (int i = 0; i < 3; ++i)
	{
        if (arr[i] == str)
            return true;
    }
	return false;
}


bool	Utils::isExtensionOf(std::string const &ext, std::string const &fullname)
{
	if (fullname.length() < ext.length())
		return false;
	std::string tmp = fullname.substr((fullname.length() - ext.length()));
	return (tmp == ext);
}

bool Utils::isParentDirOf(const std::string& parent, const std::string& son)
{
    if (parent == "/" || parent.empty())
        return false;
    if (son.compare(0, parent.length(), parent) == 0)
	{
        if (son.length() == parent.length())
            return false;
        size_t indexAfterLocation = parent.length();
        if (!parent.empty() && parent[parent.size() - 1] != '/')
            indexAfterLocation++;
        if (son.length() > indexAfterLocation && son[--indexAfterLocation] == '/')
            return true;
    }
    return false;
}

bool Utils::_isRootFile(const std::string& url)
{
	size_t firstSlashPos = url.find_first_of('/');
    size_t lastSlashPos = url.find_last_of('/');
    size_t dotPos = url.find_last_of('.');

	// Se houver mais que uma barra
	if (firstSlashPos != lastSlashPos)
		return false;
    // Se não houver ponto
    if (dotPos == std::string::npos)
    {
	    return false;
	}
	// Se o ponto estiver antes da última barra, é um diretório
	if (dotPos < lastSlashPos && lastSlashPos != std::string::npos)
	{
		return false;
	}
	// Retorna verdadeiro se houver ao meno 1 caracter após o ponto, se não falso	
	return (dotPos + 1 < url.length());
}

bool  Utils::_isRoot(std::string const &url)
{
	if (url.size() == 1 && url[0] == '/')
		return true;
	if (Utils::_isRootFile(url) && !Utils::isExtensionOf(".py", url)) 
		return true;
	return false;
}

bool Utils::directoryExists(std::string const &fullpath)
{
	struct stat info;

	if (stat(fullpath.c_str(), &info) != 0)
		return (false);
	else if (info.st_mode & S_IFDIR) 
		return (true);
	return (false);
}

bool Utils::fileExists(std::string const &fullpath)
{
	struct stat info;

	if (stat(fullpath.c_str(), &info) != 0)
		return (false);
	else if (info.st_mode & S_IFREG) 
		return (true);
	return (false);
}

bool Utils::isReadeableFile(std::string const &fullpath)
{
	struct stat fileStat;

	if (stat(fullpath.c_str(), &fileStat) != 0)
		return (false);
	else if ((fileStat.st_mode & S_IRUSR) != 0) 
		return (true);
	return (false);
}
std::string Utils::_itoa(int n)
{
	std::stringstream ss;
	ss << n;
	return ss.str();
}

std::string Utils::_defaultErrorPages(int status, std::string subText)
{
	std::string statusDescrip = _errorHTML[status];

	std::string BodyPage = 
	"<!DOCTYPE html>\n"
	"<html lang=\"pt-BR\">\n"
	"<head>\n"
	"    <meta charset=\"UTF-8\">\n"
	"    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1, maximum-scale=1\">\n"
	"    <link rel=\"canonical\" href=\"https://www.site1/index.html\" />\n"
	"    <link rel=\"shortcut icon\" href=\"/favicon.ico\">\n"
	"    <title>Error</title>\n"
	"    <link rel=\"stylesheet\" href=\"/styles.css\">\n"
	"    <link href=\"https://fonts.googleapis.com/css2?family=DM+Mono:ital,wght@0,300;0,400;0,500;1,300;1,400;1,500&family=Major+Mono+Display&display=swap\" rel=\"stylesheet\">\n"
	"</head>\n"
	"<body>\n"
	"    <div class=\"container-top\">\n"
	"        <h3 class=\"title\">Error " + Utils::_itoa(status) + ": " + statusDescrip + "</h3>\n"
	"        <p class=\"general-paragraph\">" + subText + " </p>\n"
	"        <footer>\n"
	"            <p>Copyright © 2024 Clara Franco & Ívany Pinheiro.</p>\n"
	"        </footer>\n"
	"    </div>\n"
	"</body>\n"
	"</html>\n";

	std::string HTMLHeaders = 
	"HTTP/1.1 " + Utils::_itoa(status) + " " + statusDescrip + "\n"
	"Date: " + Utils::_getTimeStamp("%a, %d %b %Y %H:%M:%S GMT") + "\n" +
	"Server: Webserv/1.0.0 (Linux)\n" +
	"Connection: keep-alive\n" +
	"Content-Type: text/html; charset=utf-8\n" +
	"Content-Length: " + Utils::_itoa(BodyPage.size()) + "\n\n";

	std::string HtmlErrorContent = HTMLHeaders + BodyPage;
	return (HtmlErrorContent);
}

/** 
 * **************************************
 * SECTION - TO DELETE
 * **************************************
 */

std::string Utils::getFileType(std::string const &file)
{
	std::map<std::string, std::string> types;

	types["txt"] = "text/plain";
	types["html"] = "text/html";
	types["css"] = "text/css";

	types["js"] = "application/javascript";
	types["py"] = "application/python";

	types["jpg"] = "image/jpg";
	types["jpeg"] = "image/jpeg";
	types["png"] = "image/png";
	types["gif"] = "image/gif";

	if (file.find_last_of(".") != std::string::npos)
	{
		std::string extension = file.substr(file.find_last_of(".") + 1);
		if (types.find(extension) != types.end())
			return (types[extension]);
	}
	return ("text/plain");
}

std::string Utils::getFileContent(std::string const &path)
{
	std::string content;
	std::ifstream file(path.c_str(), std::ios::binary | std::ios::in);

	content.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	file.close();
	return (content);
}

bool Utils::isMethodAllowed(const std::vector<std::string> &methods, const std::string &method)
{
    bool isAllowed = std::find(methods.begin(), methods.end(), method) != methods.end();
    if (isAllowed)
       	return (true);
    return (false);
}

bool	Utils::_isRightLocation(std::string locName, std::string &url)
{
	if (locName == url)
	{
		return true;
	}
	if (Utils::isParentDirOf(locName, url))
	{
		return true;
	}
	if (Utils::isExtensionOf(locName, url))
	{
		return true;
	}
	return false;
}

std::string Utils::vecToString(std::vector<char> & payload)
{
	std::string str = "";
	for (size_t i = 0; i < payload.size(); i++)
		str += payload[i];
	return (str);
}

std::string Utils::autoHTML(std::string const &status, std::string const &statusDescrip, std::string const &fullPath)
{
	std::string fileContent = Utils::getFileContent(fullPath);
	int sizeFileContent = fileContent.length();
	
	std::string response = 
		"HTTP/1.1 " + status + " " + statusDescrip + "\n"
		"Date: " + Utils::_getTimeStamp("%a, %d %b %Y %H:%M:%S GMT") + "\n" +
		"Server: Webserv/1.0.0 (Linux)\n" +
		"Connection: keep-alive\n" +
		"Content-Type: " + Utils::getFileType(fullPath) + "; charset=utf-8\n" +
		"Content-Length: " + Utils::_itoa(sizeFileContent) + "\n\n";

	return (response + fileContent);
}

bool Utils::findChunkedEOF(const std::vector<char>& payload)
{
    for (size_t pos = 0; pos + 4 < payload.size(); ++pos)
	{
        if (payload[pos] == '0' && payload[pos + 1] == '\r' &&
            payload[pos + 2] == '\n' && payload[pos + 3] == '\r' &&
            payload[pos + 4] == '\n')
		{
			return true;
		}
    }
    return false;
}

// Conta o tamanho do chunk e retorna o tamanho e a nova posição
size_t Utils::countChunkSize(const std::vector<char>& payload, size_t& pos)
{
    std::string sizeStr;
    while (pos < payload.size() && payload[pos] != '\r')
	{
        sizeStr += payload[pos];
        ++pos;
    }
    pos += 2;
    return std::strtoul(sizeStr.c_str(), NULL, 16);
}


/* void inv(char& c)
{
	if (c < 32 || c == 127) {  // ASCII control characters
		switch (c) {
			case '\n':
				std::cout << "~\n";
				break;
			case '\r':
				std::cout << "^M";
				break;
			case '\t':
				std::cout << "^I";
				break;
			case '\b':
				std::cout << "^H";
				break;
			case '\f':
				std::cout << "^L";
				break;
			case '\v':
				std::cout << "^K";
				break;
			default:
				std::cout << "^" << char(c + '@'); // Printable control characters
				break;
		}
	} else if (c == ' ') {
		std::cout << "_";  // Replace spaces with underscore for visibility
	} else {
		std::cout << c;
	}
} */


/* void Utils::printVector(std::vector<char> &vec)
{
    for (size_t i = 0; i < vec.size(); i++)
    {
	   inv(vec[i]);
	}
	
	std::cout << std::endl;
} */

void Utils::print_invisibles(const std::string& text)
{
    for (std::string::const_iterator it = text.begin(); it != text.end(); ++it)
	{
        unsigned char c = *it;
        if (c < 32 || c == 127) {  // ASCII control characters
            switch (c) {
                case '\n':
                    std::cout << "~\n";
                    break;
                case '\r':
                    std::cout << "^M";
                    break;
                case '\t':
                    std::cout << "^I";
                    break;
                case '\b':
                    std::cout << "^H";
                    break;
                case '\f':
                    std::cout << "^L";
                    break;
                case '\v':
                    std::cout << "^K";
                    break;
                default:
                    std::cout << "^" << char(c + '@'); // Printable control characters
                    break;
            }
        } else if (c == ' ') {
            std::cout << "_";  // Replace spaces with underscore for visibility
        } else {
            std::cout << c;
        }
    }
    std::cout << "$" << std::endl; // Print $ at the end of the text
}
