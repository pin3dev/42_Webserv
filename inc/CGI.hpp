
#pragma once

#include <stdio.h> 
#include <sys/types.h> 
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h> 
#include <csignal> // Para C++
#include <iostream>
#include <cstring>
#include <exception>
#include <stdlib.h>
#include <sstream>
#include <fstream>
#include "Utils.hpp"

#define WRITE_END			1
#define READ_END			0

class CGI
{
    private:
        int         _pipefd[2];
        int         _outFile;
        int         _status;

        std::string _fullPath;
        std::string _uploadTo;
        size_t      _requestLength;
        std::vector<char>& _payload;

        std::vector<char*> _environ;
        std::vector<char*> _args;


        void    _setOutFile();
        int    _writeRequestToCGI(std::string& fname, FILE*& filePtr);
        void    _runCGIandWriteHTML();
        void    _exportEnvPath();
        
        void		_setArgvEnvp();

        void	_setPipeAndFile(int &file, int (&fd)[2]);
        void	_executeBin(int &file, int (&fd)[2]);

    public:
        CGI(std::string fullPath, std::string uploadTo, std::vector<char> &payload, size_t requestLength);
        ~CGI();
        
        void setNewEnv(std::string key, std::string value);
        void execute();

};

