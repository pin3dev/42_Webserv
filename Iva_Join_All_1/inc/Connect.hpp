/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connect.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pin3dev <pinedev@outlook.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 19:05:17 by pin3dev           #+#    #+#             */
/*   Updated: 2024/05/17 18:07:35 by pin3dev          ###   ########.fr       */
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

class Connect
{
	private:
		int 		_connect_fd;
		
		Server* 	_myServer;
		Request		_myRequest;
		Response	_myResponse;

		time_t		_updated;
		bool		_ready;

		//UTILS
		void		_setUpdate(time_t now);
		
	public:
		Connect(Server &server, int connect_fd);
		~Connect();

		void	appendToRequest(char const *buffer, size_t size);
		void	runRequest();

		//CHECKERS
		bool	hasExpired() const;
		bool	isReady() const;
};

