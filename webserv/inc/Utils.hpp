
#pragma once

#include <iostream>
#include <string>
#include <ctime>
#include <vector>
#include <map>
#include <sstream>
#include <sys/stat.h>
#include <dirent.h> 
#include <fstream>
#include <algorithm>

class Utils
{
    private:
        Utils();
        ~Utils();
        
    public:
        static std::map<int, std::string> _errorHTML;
        static std::map<int, std::string> initErrorMap();
        static std::string _defaultErrorPages(int status, std::string subText);
        static time_t _nowTime();
        static std::string _getTimeStamp(const char *format);
        static std::vector<std::string> _split(std::string const &str, char delim);
        static void _trim(std::string &s, char const *set);
        static std::string _decoder(const std::string &url);
        static bool _isAt(const std::string &str, const std::string arr[3]);
        static bool	isExtensionOf(std::string const &ext, std::string const &fullname);
        static bool isParentDirOf(const std::string& parent, const std::string& son);
        static bool defaultGetUrl(std::string const &url, std::string const &method);
        static bool _isRootFile(const std::string& url);
        static bool  _isRoot(std::string const &url);
        static std::string _itoa(int n);
        static bool fileExists(std::string const &fullpath);
        static bool directoryExists(std::string const &fullpath);
        static void print_invisibles(const std::string& text);
        static std::string vecToString(std::vector<char> & payload);
        //static void printVector(std::vector<char> &vec);

        static std::string generateResponseOK(std::string const &path);
        static std::string getFileContent(std::string const &path);
        static std::string getFileType(std::string const &file);
        static std::string autoHTML(std::string const &status, std::string const &statusDescrip, std::string const &fullPath);
        static bool isMethodAllowed(const std::vector<std::string> &methods, const std::string &method);
        static bool	_isRightLocation(std::string locName, std::string &url);
        static bool isReadeableFile(std::string const &fullpath);
        static bool findChunkedEOF(const std::vector<char>& payload);
        static size_t countChunkSize(const std::vector<char>& payload, size_t& pos);
};
