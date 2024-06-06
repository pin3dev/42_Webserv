/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pin3dev <pinedev@outlook.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/06 12:22:17 by pin3dev           #+#    #+#             */
/*   Updated: 2024/06/06 20:46:39 by pin3dev          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CGI.hpp"

void	printVec(std::vector<char*> &vec)
{
	std::vector<char*>::iterator it = vec.begin();
	for (; it != vec.end(); it++)
		std::cout << *it << std::endl;
}

CGI::CGI(std::string fullPath, std::string uploadTo, std::string request, size_t requestLength) : _outFile(0), _status(0), _fullPath(fullPath), _uploadTo(uploadTo),
_request(request), _requestLength(requestLength), _environ()
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
    remove(".CGIhtml");
    
}

void CGI::setNewEnv(std::string key, std::string value)
{
    std::string env;
    if (!value.empty() && !key.empty())
		env = key + "=" + value;
	std::cout << "adding env: " << env << std::endl;
    this->_environ.push_back(env);
}


/* void CGI::setNewEnv(std::string key, std::string value)
{
    if (!value.empty() && !key.empty())
    {
        std::string env = key + "=" + value;
        char* env_heap = new char[env.size() + 1];  // Aloca memória na heap
        std::copy(env.begin(), env.end(), env_heap);
        env_heap[env.size()] = '\0';  // Certifica que a string é terminada em NULL
        this->_environ.push_back(env_heap);
        std::cout << "adding env: " << env << std::endl;
    }
} */

void CGI::_exportEnvPath()
{
	this->_args[0] = strdup("python3");
	this->_args[1] = strdup(this->_fullPath.c_str());
	this->_args[2] = strdup(this->_uploadTo.c_str());
	this->_args[3] = NULL;

	this->_envp = new char*[this->_environ.size() + 1];
	for (size_t i = 0; i < this->_environ.size(); i++)
		this->_envp[i] = strdup(this->_environ[i].c_str());
	this->_envp[this->_environ.size()] = NULL;

}
/* {	
	this->_args.push_back(const_cast<char*>("python3"));
	this->_args.push_back(const_cast<char*>(this->_fullPath.c_str()));
	this->_args.push_back(const_cast<char*>("www/site1/upload"));
	this->_args.push_back(NULL);
} */

/* void	CGI::execute()
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
	if ((this->_outFile = open(".CGIhtml", O_CREAT | O_WRONLY | O_TRUNC, 0644)) == -1)
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
	for (size_t i = 0; this->_envp[i]; i++)
		std::cout << this->_envp[i] << std::endl;
	dup2(this->_pipefd[READ_END], STDIN_FILENO);
	close(this->_pipefd[READ_END]);
	dup2(this->_outFile, STDOUT_FILENO);
	close(this->_outFile);

	std::string python3 = "/usr/bin/python3";


	if (execve(python3.c_str(), this->_args, this->_envp) == -1)
		throw std::runtime_error("500 Internal Server Error");
} */
void	CGI::_run()
{
	int file, status, fd[2];

	this->_setPipeAndFile(file, fd);
	pid_t pid = fork();

	if (pid == -1)
		throw std::runtime_error("500 Internal Server Error");

	if (pid == 0)
		this->_executeBin(file, fd);
	else
	{
		close(fd[READ_END]);
		close(file);
		waitpid(pid, &status, 0);
	}

}

void	CGI::_setPipeAndFile(int &file, int (&fd)[2])
{
	if ((file = open(".cgi", O_CREAT | O_WRONLY | O_TRUNC, 0644)) == -1)
		throw std::runtime_error("500 Internal Server Error");

	if (pipe(fd) == -1)
		throw std::runtime_error("500 Internal Server Error");
	
	if (this->_requestLength > 0)
		fcntl(fd[WRITE_END], F_SETPIPE_SZ, this->_requestLength);
	
	write(fd[WRITE_END], this->_request.data(), this->_requestLength);
	close(fd[WRITE_END]);
}

void	CGI::_executeBin(int &file, int (&fd)[2])
{
	dup2(fd[READ_END], STDIN_FILENO);
	close(fd[READ_END]);
	dup2(file, STDOUT_FILENO);
	close(file);

	std::string python3 = "/usr/bin/python3";

	if (execve(python3.c_str(), this->_args, this->_envp) == -1)
		throw std::runtime_error("500 Internal Server Error");
}
