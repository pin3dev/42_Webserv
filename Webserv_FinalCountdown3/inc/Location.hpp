/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pin3dev <pinedev@outlook.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 19:36:34 by pin3dev           #+#    #+#             */
/*   Updated: 2024/05/15 19:36:36 by pin3dev          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <vector>

typedef struct location_s
{
	std::string	root;
	std::string	redirect;
	std::string	tryFile;
	std::string	cgiPath;
	std::string	cgiExtension;
	std::string	uploadTo;
    bool		autoindex;
    bool		hasCGI;
    std::vector<std::string>    methods;
}				location_t;
