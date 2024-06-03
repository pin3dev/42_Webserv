/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connect.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pin3dev <pinedev@outlook.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 19:05:17 by pin3dev           #+#    #+#             */
/*   Updated: 2024/06/03 21:10:19 by pin3dev          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

//GENERAL INCLUDES
#include <iostream>
#include <string>

//CUSTOM INCLUDES
#include "Server.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Utils.hpp"

class Connect
{
	private:
		int 		_connect_fd;
		
		Server* 	_myServer; 
		Request		_myRequest;
		Response	_myResponse;

		time_t		_updated;
		bool		_readyToResponse;

		//UTILS
		void	_setUpdate(time_t now);
		void	_searchForNonDefaultServer(std::vector<Server> &servers);
		
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
		Response &getResponse();
		Server &getServer();
		time_t	getLastUpdated() const;
		bool	getFlagToResponse() const;

		void	resetServer(Server *server);
};

