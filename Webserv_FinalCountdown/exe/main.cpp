/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pin3dev <pinedev@outlook.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 17:55:04 by pin3dev           #+#    #+#             */
/*   Updated: 2024/06/03 20:31:41 by pin3dev          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"
#include "Server.hpp"
#include "WebServ.hpp"

/**TODO
 * Manager.cpp - Implementar mensagem geral de erro de POLLING
 * Request.cpp - Modificar valor inicial de _maxLength para 0, pois está configurado para tester
 * Connect.cpp - Verificar o tempo de expired como 60s, se é suficiente
 * Manager.cpp - Implementar, se necessário o reset do  servidor para um nonDefault 

 O PROGRAMA FUNCIONA BEM ATÉ QUE EU INSIRA UMA PORT QUE NÃO EXISTE, A PARTIR DAI, TODAS
 AS PORTAS, TANTO VALIDAS COMO INVALIDAS SÃO REJEITADAS, E QUANDO INSIRO UMA PORTA INVALIDA, O PROGRAMA DAR SEG-FAULT
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
