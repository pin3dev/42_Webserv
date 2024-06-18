
#pragma once

#include <string>
#include <vector>

typedef struct location_s
{
	std::string	root;
	std::string	redirect;
	std::string	tryFile;
	std::string	cgiPath;
	std::string	cgiExtension;
	std::string	uploadTo;
    bool		autoindex;
    bool		hasCGI;
    std::vector<std::string>    methods;
}				location_t;
