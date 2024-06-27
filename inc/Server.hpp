
#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <map>
#include <cstdlib>
#include "Location.hpp"

#define MAX_SIZE_LIMIT		10737418240.0	// 10GB

class Server
{
    private:
        std::string serverName;
        std::string root;
        std::string port;
        std::string indexPage;
        std::string errorPage;
        std::string host;
        size_t		clientMaxBodySize;
        bool		isDefault;
        int			socket;
        std::map<std::string, location_t> locations;
        
        std::string ensureUniqueServerName(std::vector<Server> const &servers, std::string const &name);
        std::string validateRootPath(std::string const &root);
        std::string validatePortNumber(std::string const &port);
        std::string validatePagePath(std::string const &page);
        size_t      parseSizeWithSuffix(std::string const &size);
        void        checkServerSettings(std::map<std::string, std::string> const &settings);
        void        applyServerSettings(std::map<std::string, std::string> &settings, std::vector<Server> const &servers, std::map<std::string, location_t> const &locationMap);
        bool        verifyServerAsDefault(std::vector<Server> const &servers);
        bool        directoryExists(std::string const &path);
    public:
        Server(std::map<std::string, std::string> &settings, std::vector<Server> const &servers, std::map<std::string, location_t> const &locationMap);
        ~Server();
        std::map<std::string, location_t> const &getLocations() const;
        std::string getServerName() const;
        std::string getRoot() const;
        std::string getPort() const;
        std::string getIndexPage() const;
        std::string getErrorPage() const;
        std::string getHost() const;
        size_t      getClientMaxBodySize() const;
        bool        isDefaultServer() const;
        int         getSocket() const;
        void        setSocket(int socket);
};

std::ostream &operator<<(std::ostream &out, Server const &server);
