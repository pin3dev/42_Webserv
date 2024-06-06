/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pin3dev <pinedev@outlook.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 19:36:03 by pin3dev           #+#    #+#             */
/*   Updated: 2024/06/05 21:24:42 by pin3dev          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"

Parser::Parser(int ac, char **av)
{
    this->verifyArguments(ac, av);
    this->verifyConfFile();
    this->initAttributes();
}

Parser::~Parser()
{
	this->fileStream.close();
}

void Parser::initAttributes() 
{
    initializeValidKeywords();
    this->fileStream.open(this->confPath.c_str());
    if (!this->fileStream.is_open())
    {
        std::cout << "erro abertura" << std::endl;
        exit(1); //REMOVER
    }
    this->bracketOpen = 0;
    this->foundServer = false;
}

std::vector<Server> Parser::startParsing()
{
    char currentChar = this->fileStream.get();
    std::string token = checkNextToken(currentChar);
    std::vector<Server>	servers;

    while (token != "END")
	{
        servers.push_back(this->parseAndCreateServer(token, currentChar, servers));
		token = this->checkNextToken(currentChar);
	}
    return(servers);
}

Server Parser::parseAndCreateServer(std::string &token, char& currentChar, std::vector<Server> servers)
{ 
    std::pair<std::string, location_t> locationPair;
    std::map<std::string, location_t> locationMap;
    if (token != "server")
    {
		std::cout << "novo bloco deveria iniciar com server" << std::endl; 
        exit (1); //REMOVER
        //throw std::runtime_error(ERR_BLOCK_START(this->_token.value, intToString(this->_line)));
    }
	this->settings.clear();
	while (token != "}" && token != "END")
	{			
        if(token != "server")
            this->addSettingPair(token, currentChar, this->settings);
        if(token == "location")
        {
            locationPair = getLocationPair(currentChar);
            locationMap.insert(locationPair);
        }
        token = this->checkNextToken(currentChar);
	}
    if (this->bracketOpen == 0)
        return(Server(this->settings, servers, locationMap));
    else
    {
        //throw std::runtime_error("Unclosed brackets detected at the end of file.");
        std::cout << "Unclosed brackets detected at the end of file." <<std::endl;
        exit(1);
    }
}

std::pair<std::string, location_t> Parser::getLocationPair(char& currentChar)
{
    std::string locationValue;
	std::map<std::string, std::string> locationSettings;
    location_t locationStructure;

    locationValue = this->settings["location"];
    Utils::_trim(locationValue, " \t\r\v\f");
    this->settings.erase("location");
    locationSettings = extractLocationSettings(currentChar);
    locationStructure = fillLocationStructure(locationSettings);

    return (std::make_pair(locationValue, locationStructure));
}

std::map<std::string, std::string> Parser::extractLocationSettings(char& currentChar) 
{
    std::map<std::string, std::string> locationSettings;
    std::string token;

    if(this->checkNextToken(currentChar) != "{")
    {
        std::cout << "location deve ser seguido de {" << std::endl;
        exit(1); //REMOVER
    }
    token = this->checkNextToken(currentChar);
    while (!this->fileStream.eof() && token != "}")
    {
        this->addSettingPair(token, currentChar, locationSettings);
        token = this->checkNextToken(currentChar);
    }
    return(locationSettings);
}

location_t Parser::fillLocationStructure(std::map<std::string, std::string> const &locationSettings)
{
    location_t	location;
    std::string path;

    if (locationSettings.find("root") != locationSettings.end()) 
    {
        path = locationSettings.find("root")->second;
        if (!path.empty() && path[path.length() - 1] != '/')
            path += "/";
        location.root = path;
    }
    if (locationSettings.find("cgi_path") != locationSettings.end()) 
    {
        path = locationSettings.find("cgi_path")->second;
        if (!path.empty() && path[path.length() - 1] != '/')
            path += "/";
        location.cgiPath = path;
    }
    if (locationSettings.find("upload_to") != locationSettings.end())
    {
        path = locationSettings.find("upload_to")->second;
        if (!path.empty() && path[0] == '/')
            path.erase(0, 1);
        location.uploadTo = path;
    }
    if (locationSettings.find("allow_methods") != locationSettings.end())
        location.methods = getStringTokens(locationSettings.find("allow_methods")->second, ' ');
    
    if (locationSettings.find("return") != locationSettings.end())
        location.redirect = locationSettings.find("return")->second;

    if (locationSettings.find("try_file") != locationSettings.end())
        location.tryFile = locationSettings.find("try_file")->second;

    if (locationSettings.find("cgi_ext") != locationSettings.end())
        location.cgiExtension = locationSettings.find("cgi_ext")->second;

    if (locationSettings.find("autoindex") != locationSettings.end())
        location.autoindex = locationSettings.find("autoindex")->second == "on" ? true : false;
    else
        location.autoindex = false;
    
    if (!location.cgiPath.empty() && !location.cgiExtension.empty())
        location.hasCGI = true;
    else
        location.hasCGI = false;
    
    return location;          
}

void Parser::addSettingPair(std::string const &key, char& currentChar, std::map<std::string, std::string>& settings)
{
    if(key == "{")
        return;
    std::string value = "";
    while (!this->fileStream.eof() && std::isspace(currentChar))
        currentChar = this->fileStream.get();
    while (!this->fileStream.eof())
	{
        if(currentChar == ';')
        {
            currentChar = this->fileStream.get();
            break;
        }
        if(currentChar == '{')
            break;
        if (std::isspace(currentChar) && key != "allow_methods" && key != "location")
        {
            std::cout << "mais de um valor" << std::endl;
            exit(1);
            //throw std::runtime_error(ERR_MANY_VALUES(keyword, intToString(this->_line)));
        }
		value += currentChar;
		currentChar = this->fileStream.get();
	}
    if (value.empty())
    {
        std::cout << "Erro: Nenhum valor encontrado antes do ponto e virgula." << std::endl;
        exit(1);
        //throw std::runtime_error("Nenhum valor encontrado antes do ponto e virgula.");
    }
    settings[key] = value;
}

std::string Parser::checkNextToken(char &currentChar) 
{
    std::string token = "";
    
    while (!this->fileStream.eof()) 
	{
        if (this->skipSpacesAndComments(currentChar))
        {
            continue;
        }
        if (currentChar == '{')
        {
            this->bracketOpen++;
            currentChar = this->fileStream.get();
            return("{");
        }
        if (currentChar == '}')
        {
            this->bracketOpen--;
            currentChar = this->fileStream.get();
            return("}");
        }
        if (std::isalpha(currentChar))
        {
            return (extractKeyword(currentChar, token));
        }
        token += currentChar;
        currentChar = this->fileStream.get();
    }
    if (this->bracketOpen != 0)
        throw std::runtime_error("Unclosed brackets detected at the end of file.");
    if (!this->foundServer)
        throw std::runtime_error("No server configuration found.");
    token = "END";
    return token;
}


bool Parser::skipSpacesAndComments(char &currentChar) 
{
    if (std::isspace(currentChar)) 
    {
        while (!this->fileStream.eof() && std::isspace(currentChar)) 
        {
            currentChar = this->fileStream.get();
        }
        return true;
    }
    if (currentChar == '#') 
    {
        std::string discardedLine;
        std::getline(this->fileStream, discardedLine);  // Le e descarta o restante da linha
        currentChar = this->fileStream.get();  // Le o proximo caractere apos a nova linha
        return true;
    }
    return false;
}

std::string Parser::extractKeyword(char &currentChar, std::string &keyword) 
{
    while (!this->fileStream.eof() && (std::isalnum(currentChar) || currentChar == '_')) 
    {
        keyword += currentChar;
        currentChar = this->fileStream.get();
    }
    if (validKeywords.find(keyword) == validKeywords.end()) 
    {
        std::cout << "Invalid keyword" + keyword << std::endl;
        exit(1); // REMOVER
        //throw std::runtime_error("Invalid keyword: " + keyword); // ver
    }
    if (keyword == "server")
    {
        this->foundServer = true;
        if(checkNextToken(currentChar) != "{")
        {
            std::cout << "server deveria ser seguido de {" << std::endl; 
            exit (1); //REMOVER
            //throw std::runtime_error(ERR_BLOCK_START(this->_token.value, intToString(this->_line)));
        }
    }
    return (keyword);
}

void Parser::initializeValidKeywords() 
{
    validKeywords.insert("allow_methods");
    validKeywords.insert("autoindex");
    validKeywords.insert("cgi_ext");
    validKeywords.insert("cgi_path");
    validKeywords.insert("client_max_body_size");
    validKeywords.insert("error_page");
    validKeywords.insert("host");
    validKeywords.insert("index");
    validKeywords.insert("listen");
    validKeywords.insert("location");
    validKeywords.insert("return");
    validKeywords.insert("root");
    validKeywords.insert("server");
    validKeywords.insert("server_name");
    validKeywords.insert("try_file");
    validKeywords.insert("upload_to");
}

void Parser::verifyArguments(int ac, char **av)
{
    if(ac > 2)
    {
        std::cout << "erro muitos argumentos" << std::endl;
        exit(1); //REMOVER
    }
    else if(ac == 2)
    {
        this->confPath = av[1];
    }
    else
    {
        std::cout << "this->confPath vai receber caminho padrao" << std::endl;
        exit(1); //REMOVER
    }
}

void Parser::verifyConfFile()
{
    std::string filename = this->confPath;
	size_t slash = this->confPath.find_last_of("/");
    size_t dot;

	if (slash != std::string::npos)
		filename = this->confPath.substr(slash + 1);
    dot = filename.find_last_of(".");
	if (dot == std::string::npos || filename.substr(dot) != ".conf" || filename.length() <= std::string(".conf").length())
	{
		std::cout << "erro extensao conf" << std::endl;
        exit(1); //REMOVER
    }
}

std::vector<std::string> Parser::getStringTokens(std::string const &s, char c)
{
	std::vector<std::string> tokens;
	std::stringstream ss(s);
	std::string token;

	while (std::getline(ss, token, c))
	{
		if (!token.empty())
			tokens.push_back(token);
	}
	return tokens;
}

/////////////////////////DEBBUG

void Parser::printSettings() const 
{
    std::cout << "\nSettings content:" << std::endl;
    for (std::map<std::string, std::string>::const_iterator it = settings.begin(); it != settings.end(); ++it) 
    {
        std::cout << "Key: " << it->first << " Value: " << it->second << std::endl;
    }
    std::cout << std::endl;
}

void Parser::printLocations(std::map<std::string, location_t> locationMap) const 
{
    std::cout << "Locations content:" << std::endl;
    for (std::map<std::string, location_t>::const_iterator it = locationMap.begin(); it != locationMap.end(); ++it) 
    {
        const location_t& loc = it->second;
        std::cout << "Location Key: " << it->first << std::endl;
        std::cout << "  Root: " << loc.root << std::endl;
        std::cout << "  Methods: ";
        for (size_t i = 0; i < loc.methods.size(); ++i) {
            std::cout << loc.methods[i] << (i < loc.methods.size() - 1 ? ", " : "");
        }
        std::cout << std::endl;
        std::cout << "  Redirect: " << loc.redirect << std::endl;
        std::cout << "  Autoindex: " << (loc.autoindex ? "true" : "false") << std::endl;
        std::cout << "  Try File: " << loc.tryFile << std::endl;
        std::cout << "  Has CGI: " << (loc.hasCGI ? "true" : "false") << std::endl;
        std::cout << "  CGI Path: " << loc.cgiPath << std::endl;
        std::cout << "  CGI Extension: " << loc.cgiExtension << std::endl;
        std::cout << "  Upload To: " << loc.uploadTo << std::endl;
    }
}
