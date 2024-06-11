/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pin3dev <pinedev@outlook.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/06 12:22:02 by pin3dev           #+#    #+#             */
/*   Updated: 2024/06/07 16:14:31 by pin3dev          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <stdio.h> 
#include <sys/types.h> 
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h> 
#include <iostream>
#include <cstring>
#include <exception>
#include <stdlib.h>
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

        std::vector<char*> _environ;
        std::vector<char*> _args;


        void    _setOutFile();
        void    _writeRequestToCGI();
        void    _runCGIandWriteHTML();
        void    _exportEnvPath();
        
        void		_setArgvEnvp();

        void	_setPipeAndFile(int &file, int (&fd)[2]);
        void	_executeBin(int &file, int (&fd)[2]);

    public:
        CGI(std::string fullPath, std::string uploadTo, std::string request, size_t requestLength);
        ~CGI();
        
        void setNewEnv(std::string key, std::string value);
        void execute();

};

