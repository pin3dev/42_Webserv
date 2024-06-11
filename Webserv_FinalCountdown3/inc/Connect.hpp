/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connect.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pin3dev <pinedev@outlook.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 19:05:17 by pin3dev           #+#    #+#             */
/*   Updated: 2024/06/11 18:32:23 by pin3dev          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <string>

#include "Utils.hpp"
#include "Server.hpp"
#include "Request.hpp"
#include "CGI.hpp"
#include "Location.hpp"

class Connect
{
	private:
		int 		_connect_fd;
		int         _urlLoopCount;
		time_t		_updated;
		
		Server* 	_myServer; 
		Request		_myRequest;

		std::map<std::string,location_t>::const_iterator _rightLocation;
		std::map<std::string,location_t>::const_iterator _cgiLocation;
		
		std::string _fullPath;
		std::string _effectiveUpload;

		bool	_isRedirect;

		void	_setUpdate(time_t now);
		void	_searchForNonDefaultServer(std::vector<Server> &servers);

		void 	_processRequest(const std::string &url, const std::string &method, const std::string &root);
		void    _exportEnviron(CGI &cgi);
		void	_defaultErrorPages(int status, std::string subText);

		void	_serveFile(const std::string &fullPath, const std::string &effectiveRoot, int _connectSocket);
		void	_serveAutoindex(std::string &fullPath, std::string &rootDir, int _connectSocket);
		bool	_serveTryFile(const std::string &tryFilePath, const std::string &effectiveRoot, int _connectSocket);
		bool	_tryServeErrorPage(const std::string &effectiveRoot);
		
	public:
		Connect(Server &server, int connect_fd);
		~Connect();

		void	appendToRequest(char const *buffer, size_t size);
		void	runRequest(std::vector<Server> &servers);

		//CHECKERS
		bool	hasExpired() const;
		bool	isReady() const;

		//GETTERS
		int		getConnectFd() const;
		Request &getRequest();
		Server &getServer();
		
		time_t	getLastUpdated() const;
		bool	getFlagToResponse() const;
		bool	isExpired();

		//SETTERS
		void	setServer(Server *server);
};

