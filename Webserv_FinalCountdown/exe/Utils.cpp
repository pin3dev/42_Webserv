/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pin3dev <pinedev@outlook.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/27 15:11:45 by pin3dev           #+#    #+#             */
/*   Updated: 2024/05/28 11:53:09 by pin3dev          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Utils.hpp"

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

/* bool Utils::_isAt(const std::string &str, const std::string (&arr)[3])
{
    return (std::find(std::begin(arr), std::end(arr), str) != std::end(arr));
} */

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
	std::string tmp = fullname.substr(fullname.length() - ext.length());
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


bool Utils::defaultGetUrl(std::string const &url, std::string const &method)
{
	if (method == "GET")
	{
		if (!Utils::isExtensionOf(".py", url))
			return true;
	}
	return false;
}
bool Utils::_isRootFile(const std::string& url)
{
	size_t firstSlashPos = url.find_first_of('/');
    size_t lastSlashPos = url.find_last_of('/');
    size_t dotPos = url.find_last_of('.');

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
	// Se houver mais que uma barra
	if (firstSlashPos != lastSlashPos)
		return false;
	// Retorna verdadeiro se houver ao meno 1 caracter após o ponto, se não falso	
	return (dotPos + 1 < url.length());
}

bool  Utils::_isRoot(std::string const &url)
{
	if (url.size() == 1 && url[0] == '/')
		return true;
	if (Utils::_isRootFile(url) && !Utils::isExtensionOf(".py", url)) //**MELHORAR ESSA EXTENSÃO PARA SER FORNECIDA PELO PROGRAMA
		return true;
	return false;
}

bool Utils::directoryExists(std::string const &fullpath)
{
	struct stat info;

	if (stat(fullpath.c_str(), &info) != 0)
		return ((std::cout << "O ARQUIVO: " << fullpath << " NÃO É UM DIRETORIO!\n"), false);
	else if (info.st_mode & S_IFDIR) 
		return ((std::cout << "O ARQUIVO: " << fullpath << " É UM DIRETORIO!\n"), true);
	return ((std::cout << "O ARQUIVO: " << fullpath << " NÃO É UM DIRETORIO!\n"), false);

}

bool Utils::fileExists(std::string const &fullpath)
{
	struct stat info;

	if (stat(fullpath.c_str(), &info) != 0)
		return ((std::cout << "O CAMINHO: " << fullpath << " NÃO É UM ARQUIVO!\n"), false);
	else if (info.st_mode & S_IFREG) 
		return ((std::cout << "O CAMINHO: " << fullpath << " É UM ARQUIVO!\n"), true);
	return ((std::cout << "O CAMINHO: " << fullpath << " NÃO É UM ARQUIVO!\n"), false);
}
std::string Utils::_itoa(int n)
{
	std::stringstream ss;
	ss << n;
	return ss.str();
}



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
