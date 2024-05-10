#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <map>
#include "Server.hpp"
#include "Location.hpp"

class Parser
{
	private:
        std::map<std::string, std::string> settings;
        std::set<std::string>   validKeywords;
        std::ifstream           fileStream;
        std::string             confPath;
        bool                    foundServer;
        int                     bracketOpen;

        std::string checkNextToken(char &currentChar);
        std::string extractKeyword(char &currentChar, std::string &keyword);
        location_t  fillLocationStructure(std::map<std::string, std::string> const &locationSettings);
        Server      parseAndCreateServer(std::string &token, char& currentChar, std::vector<Server> servers);
        bool        skipSpacesAndComments(char &currentChar);
        void        addSettingPair(std::string const &key, char& currentChar, std::map<std::string, std::string>& settings);
        void        initAttributes();
        void        initializeValidKeywords();
        void        verifyArguments(int ac, char **av);
        void        verifyConfFile();
        std::pair<std::string, location_t> getLocationPair(char& currentChar);
        std::map<std::string, std::string> extractLocationSettings(char& currentChar);
        std::vector<std::string>           getStringTokens(std::string const &s, char c);
    public:
        Parser(int ac, char **av);
        ~Parser();
        std::vector<Server> startParsing();

        void printSettings() const; // DEBBUG
        void printLocations(std::map<std::string, location_t> locationMap) const; // DEBBUG
};