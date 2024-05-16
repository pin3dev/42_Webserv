/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pin3dev <pinedev@outlook.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 17:55:04 by pin3dev           #+#    #+#             */
/*   Updated: 2024/05/15 22:40:40 by pin3dev          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"
#include "Server.hpp"
#include "WebServ.hpp"

int main(int ac, char **av)
{
        //std::vector<Server>	servers = parser.startParsing();
        try
        {
                Parser parser(ac, av);
                WebServ program(parser.startParsing());
                program.runWebServ();
        }
        catch(const std::exception& e)
        {
                std::cerr << e.what() << '\n';
        }
        
}
