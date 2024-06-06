/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pin3dev <pinedev@outlook.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/06 12:22:02 by pin3dev           #+#    #+#             */
/*   Updated: 2024/06/06 20:43:24 by pin3dev          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once


//GENERAL INCLUDES
/* #include <iostream>
#include <string>
#include <map>
#include <vector> */

//ESPECIFIC INCLUDES
#include <stdio.h> 
#include <sys/types.h> 
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h> 
#include <iostream>
#include <cstring>
#include <exception>

//CUSTOM INCLUDES
//#include "Connect.hpp"
//#include "Location.hpp"
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
        std::string _request;
        size_t      _requestLength;

/*         std::vector<char*> _environ;
        std::vector<char*> _args; */
        std::vector<std::string> _environ;
        char			**_envp;
		char			*_args[4];

        //char** _env;

        //void    _setOutFile();
        //void    _writeRequestToCGI();
        //void    _runCGIandWriteHTML();
        //void    _exportEnv();
        void    _exportEnvPath();
        

        void	_setPipeAndFile(int &file, int (&fd)[2]);
        void	_executeBin(int &file, int (&fd)[2]);

    public:
        CGI(std::string fullPath, std::string uploadTo, std::string request, size_t requestLength);
        ~CGI();
        
        void	_run();
        void setNewEnv(std::string key, std::string value);
        void execute();
};

