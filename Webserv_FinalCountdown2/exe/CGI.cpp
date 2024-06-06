/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pin3dev <pinedev@outlook.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/06 12:22:17 by pin3dev           #+#    #+#             */
/*   Updated: 2024/06/06 21:51:04 by pin3dev          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CGI.hpp"

void	printVec(std::vector<char*> &vec)
{
	std::vector<char*>::iterator it = vec.begin();
	for (; it != vec.end(); it++)
		std::cout << *it << std::endl;
}

CGI::CGI(std::string fullPath, std::string uploadTo, std::string request, size_t requestLength) : _outFile(0), _status(0), _fullPath(fullPath),
_uploadTo(uploadTo), _request(request), _requestLength(requestLength), _environ()
{
	std::cout << "\nCGI CRIADO:\n"
		<< "outFile: " << _outFile << std::endl
		<< "status: " << _status << std::endl
		<< "fullPath: " << _fullPath << std::endl
		<< "request: " << _request << std::endl
		<< "requestLength: " << _requestLength << std::endl;
}

CGI::~CGI()
{
    //**verificar se ta aberto
    close(this->_pipefd[0]);
    close(this->_pipefd[1]);
    close(this->_outFile);
    //remove("cgi.txt");
    
}


void CGI::setNewEnv(std::string key, std::string value)
{
    if (!value.empty() && !key.empty())
    {
        std::string env = key + "=" + value;
        char* env_heap = new char[env.size() + 1];
        std::copy(env.begin(), env.end(), env_heap);
        env_heap[env.size()] = '\0'; 
        this->_environ.push_back(env_heap);
    }
}

void CGI::_exportEnvPath()
{
	this->_environ.push_back(NULL);
	this->_args.push_back(const_cast<char*>("python3"));
	this->_args.push_back(const_cast<char*>(this->_fullPath.c_str()));
	this->_args.push_back(const_cast<char*>(this->_uploadTo.c_str()/* "www/site1/upload" */));
	this->_args.push_back(NULL);
}

void	CGI::execute()
{
	//std::cout << "trying to execute...\n";
    this->_exportEnvPath();
	//printVec(this->_args);
	//printVec(this->_environ);
    this->_setOutFile();
    this->_writeRequestToCGI();
	pid_t pid = fork();

	if (pid == -1)
		throw std::runtime_error("500 Internal Server Error");

	if (pid == 0)
		this->_runCGIandWriteHTML();
	else
	{
		close(this->_pipefd[READ_END]);
		close(this->_outFile);
		waitpid(pid, &(this->_status), 0);
	}

}
void	CGI::_setOutFile()
{
	if ((this->_outFile = open("cgi.html", O_CREAT | O_WRONLY | O_TRUNC, 0644)) == -1)
		throw std::runtime_error("500 Internal Server Error");
}

void	CGI::_writeRequestToCGI()
{
	if (pipe(this->_pipefd) == -1)
		throw std::runtime_error("500 Internal Server Error");
	
	if (this->_requestLength > 0)
		fcntl(this->_pipefd[WRITE_END], F_SETPIPE_SZ, this->_requestLength);
	
	write(this->_pipefd[WRITE_END], this->_request.data(), this->_requestLength);
	close(this->_pipefd[WRITE_END]);
}



void	CGI::_runCGIandWriteHTML()
{
	for(size_t i = 0; this->_args[i]; i++)
		std::cout << this->_args[i] << std::endl;
	for(size_t i = 0; this->_environ[i]; i++)
		std::cout << this->_environ[i] << std::endl;
	dup2(this->_pipefd[READ_END], STDIN_FILENO);
	close(this->_pipefd[READ_END]);
	dup2(this->_outFile, STDOUT_FILENO);
	close(this->_outFile);

	std::string python3 = "/usr/bin/python3";


	if (execve(python3.c_str(), &(this->_args[0]), &(this->_environ[0])) == -1)
		throw std::runtime_error("500 Internal Server Error");
}

