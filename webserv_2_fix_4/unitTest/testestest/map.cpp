#include <map>
#include <string>
#include <exception>
#include <iostream>
#include <sstream>
#include <fstream>

void _trim(std::string &s, char const *set)
{
	s.erase(0, s.find_first_not_of(set));
	s.erase(s.find_last_not_of(set) + 1);
}

std::map<std::string, std::string>	_setHeaders(std::stringstream &ss)
{
    std::map<std::string, std::string> map;
	std::string curLine;
	while (std::getline(ss, curLine))
	{
		if (curLine == "\r" || curLine == "\r\n" || curLine.empty()) 
        	break;
		if (curLine.find(':') != std::string::npos) 
		{
			std::string key = curLine.substr(0, curLine.find(':'));
			std::string value = curLine.substr(curLine.find(':') + 1, curLine.find('\n'));
			_trim(key, " \t\r\v\f");
			_trim(value, " \t\r\v\f");

			if (value.length() == 0 || key.length() == 0)
				throw std::runtime_error("_setHeaders: 400 Bad Request");
			map[key] = value;
		}
	}
    return map;
}

void printMap(std::map<std::string, std::string> map)
{
    std::map<std::string, std::string>::iterator it = map.begin();

    for (; it != map.end(); it++)
    {
        std::cout << "[KEY]: " << it->first
        << " {VALUE}: " << it->second << std::endl;
    }
}

std::string	_setHost(std::map<std::string, std::string> map)
{
    std::string hostAndPort = map["Host"];
    size_t pos = hostAndPort.find(':');
    if (pos != std::string::npos)
        return hostAndPort.substr(0, pos);
    else
        return hostAndPort;
}


int main (void)
{
    try
	{
        std::ifstream	testFile("RequestTEST1.txt");
        if (!testFile.is_open())
                return (std::cerr << "RequestTEST1.txt not avalaible\n", 1);
        
        std::stringstream HTTPrequest;
        HTTPrequest << testFile.rdbuf();
        testFile.close();

        printMap(_setHeaders(HTTPrequest)); 
		std::cout << _setHost(_setHeaders(HTTPrequest));
	}
	catch(const std::exception& e)
	{
		std::cerr << "RESPONSE GENERATOR ERROR PAGE:" << e.what() << '\n';
	}
}