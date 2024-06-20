
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
        throw std::runtime_error("Error: Couldn't open .conf file.");
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
        throw std::runtime_error("Error: Conf block must start with \"server\".");
	
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
        throw std::runtime_error("Error: Bracket error detected.");
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
        throw std::runtime_error("Error: Location must be followed by \"{\"." );
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
            throw std::runtime_error("Error: More than one value.");
		value += currentChar;
		currentChar = this->fileStream.get();
	}
    if (value.empty())
        throw std::runtime_error("Error: No value found before semicolon.");
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
        throw std::runtime_error("Error: Bracket error detected.");
    if (!this->foundServer)
        throw std::runtime_error("Error: No server configuration found.");
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
        std::getline(this->fileStream, discardedLine); 
        currentChar = this->fileStream.get(); 
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
        throw std::runtime_error("Error: Invalid keyword: " + keyword);
    if (keyword == "server")
    {
        this->foundServer = true;
        if(checkNextToken(currentChar) != "{")
            throw std::runtime_error("Error: Server should be followed by \"{\"");
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
        throw std::runtime_error("Error: Too many arguments");
    else if(ac == 2)
        this->confPath = av[1];
    else
        this->confPath = "configs/default.conf";
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
		throw std::runtime_error("Error: Extension should be .conf");
}

std::vector<std::string> Parser::getStringTokens(std::string const &s, char c)
{
    std::vector<std::string> tokens;
    std::stringstream ss(s);
    std::string token;
    std::vector<std::string> possibleMethods;

    possibleMethods.push_back("GET");
    possibleMethods.push_back("POST");
    possibleMethods.push_back("DELETE");

    while (std::getline(ss, token, c))
    {
        if (!token.empty())
        {
            if (std::find(possibleMethods.begin(), possibleMethods.end(), token) == possibleMethods.end())
            {
                throw std::runtime_error("Error: This server only supports GET, POST, and DELETE methods");
            }
            tokens.push_back(token);
        }
    }
    return tokens;
}

