/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pin3dev <pinedev@outlook.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/06 12:22:17 by pin3dev           #+#    #+#             */
/*   Updated: 2024/06/11 16:37:56 by pin3dev          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CGI.hpp"

CGI::CGI(std::string fullPath, std::string uploadTo, std::string request, size_t requestLength) : _outFile(0), _status(0), _fullPath(fullPath),
_uploadTo(uploadTo), _request(request), _requestLength(requestLength), _environ(){}

CGI::~CGI()
{
	for (size_t i = 0; i < this->_environ.size(); ++i)
        delete[] _environ[i];
}

void _handle_timeout(pid_t pid)
{
    kill(pid, SIGKILL);
}

bool _hasTimeOut(time_t start_time)
{
	const int timeout = 1;
	return (Utils::_nowTime() - start_time > timeout);
}

void CGI::_exportEnvPath()
{
	this->_environ.push_back(NULL);
	this->_args.push_back(const_cast<char*>("python3"));
	this->_args.push_back(const_cast<char*>(this->_fullPath.c_str()));
	this->_args.push_back(const_cast<char*>(this->_uploadTo.c_str()));
	this->_args.push_back(NULL);
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
 void	CGI::execute()
{
    this->_exportEnvPath();
    this->_setOutFile();
    this->_writeRequestToCGI();
	pid_t pid = fork();

	if (pid == -1)
		throw std::runtime_error(Utils::_defaultErrorPages(500, "Problema no CGI::execute()"));

	if (pid == 0)
		this->_runCGIandWriteHTML();
	else
	{
		close(this->_pipefd[READ_END]);
		close(this->_outFile);
 		time_t start_time = Utils::_nowTime();
		while (true)
		{
			pid_t result = waitpid(pid, &(this->_status), WNOHANG);
			if (result < 0)
				throw std::runtime_error(Utils::_defaultErrorPages(500, "Problema na execve do arquivo CGI."));
			else if (result == 0)
			{
				if (_hasTimeOut(start_time))
				{
					_handle_timeout(pid);
					throw std::runtime_error(Utils::_defaultErrorPages(500, "Problema de loop com o arquivo CGI."));
					break;
				}
			}
			else
				break;
		}
	}
}
void	CGI::_setOutFile()
{
	if ((this->_outFile = open("cgi.html", O_CREAT | O_WRONLY | O_TRUNC, 0644)) == -1)
		throw std::runtime_error(Utils::_defaultErrorPages(500, "Problema no CGI::_setOutFile()"));
}

void	CGI::_writeRequestToCGI()
{
	if (pipe(this->_pipefd) == -1)
		throw std::runtime_error(Utils::_defaultErrorPages(500, "Problema no CGI::_writeRequestToCGI()"));
	
	if (this->_requestLength > 0)
		fcntl(this->_pipefd[WRITE_END], F_SETPIPE_SZ, this->_requestLength);
	
	write(this->_pipefd[WRITE_END], this->_request.data(), this->_request.size());
	close(this->_pipefd[WRITE_END]);
} 

void	CGI::_runCGIandWriteHTML()
{
	dup2(this->_pipefd[READ_END], STDIN_FILENO);
	close(this->_pipefd[READ_END]);
	dup2(this->_outFile, STDOUT_FILENO);
	close(this->_outFile);

	std::string python3 = "/usr/bin/python3";

	if (execve(python3.c_str(), &(this->_args[0]), &(this->_environ[0])) == -1)
		throw std::runtime_error(Utils::_defaultErrorPages(500, "Problema no CGI::_runCGIandWriteHTML()"));
}
