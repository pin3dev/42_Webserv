/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connect.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pin3dev <pinedev@outlook.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 19:05:17 by pin3dev           #+#    #+#             */
/*   Updated: 2024/05/16 02:27:12 by pin3dev          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

//GENERAL INCLUDES
#include <iostream>
#include <string>

//CUSTOM INCLUDES
#include "Server.hpp"

class Connect
{
	private:
		int 		_connect_fd;
		Server* 	_vServer;
		std::string	_request;

		time_t		_updateTimeStamp;
		bool		_ready;

		//UTILS
		void		_setTimeStamp();
		
	public:
		Connect(Server &server, int connect_fd);
		~Connect();

		void	appendToRequest(char const *buffer, size_t size);
};

