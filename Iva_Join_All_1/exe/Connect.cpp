/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connect.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pin3dev <pinedev@outlook.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 19:04:31 by pin3dev           #+#    #+#             */
/*   Updated: 2024/05/16 02:28:52 by pin3dev          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Connect.hpp"

Connect:: Connect(Server &server, int connect_fd) : _connect_fd(connect_fd), _vServer(&server), _ready(false)
{
    (void)_vServer;
    (void)_ready;
    (void)_connect_fd;
}

Connect::~Connect()
{}

void	Connect::appendToRequest(char const *buffer, size_t size)
{
    this->_setTimeStamp(); 
    //this->_ready = false;
	this->_request.append(buffer, size); //ARMAZENA A MENSSAGEM RECEBIDA DO CLIENTE CONCATENANDO COM O QUE JÁ FOI RECEBIDO
}

void	Connect::_setTimeStamp()
{
    this->_updateTimeStamp = std::time(NULL);
}
