/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pin3dev <pinedev@outlook.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 17:55:04 by pin3dev           #+#    #+#             */
/*   Updated: 2024/06/06 22:02:03 by pin3dev          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"
#include "Server.hpp"
#include "WebServ.hpp"

/**TODO
 * Manager.cpp - Implementar mensagem geral de erro de POLLING
 * Connect.cpp - Verificar o tempo de expired como 60s, se é suficiente
*/

int main(int ac, char **av)
{
        try
        {
                Parser parser(ac, av);
                WebServ program(parser.startParsing());
                program.runWebServ();
        }
        catch(const std::exception& e){std::cerr << e.what() << '\n';}
}
