/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pin3dev <pinedev@outlook.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 19:10:47 by pin3dev           #+#    #+#             */
/*   Updated: 2024/05/17 18:54:54 by pin3dev          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

/** 
 * **************************************
 * SECTION - CONSTRUCTORS & DESTRUCTORS
 * **************************************
 */

Response::Response()
{}

Response::~Response()
{}

/** 
 * **************************************
 * SECTION - GETTERS/SETTERS
 * **************************************
 */



void    Response::setStatus(std::string const &status){this->_status = status;}


/** 
 * **************************************
 * SECTION - PUBLIC METHODS
 * **************************************
 */

/* //CRIA UM HTML COM CORPO FIXADO E DEVOLVE COMO RESPOSTA AO REQUEST
void	Request::_writeErrorResponse(std::string const &error)
{
	std::string body =
		"    <div class=\"error-container\">\n"
		"        <h1 class=\"error-heading\">Erro " + error + "</h1>\n"
		"        <p class=\"error-message\">Sorry, an error occurred during the processing of your request.</p>\n"
		"	 	 <p>Go to:\n"
		"			 <a href=\"index.html\">Back to Home</a>\n"
		"    </div>\n";

	std::string response = generateResponseWithCustomHTML(error, "Error", body); 
	write(this->_socket, response.c_str(), response.length());

	if (!this->_method.empty() && !this->_resource.empty())
		printInfo(this->_server.getHost() + ":" + this->_server.getPort() + " " + this->_method + " " + this->_resource + " -> " + error, RED);
	else
		printInfo(error, RED);
}
//TAVA COMO UTILS
//CRIA PAGINA HTML COM PARTES DA RESPOSTA DO CGI E CABEÇALHO DE SUCESSO
std::string generateResponseWithCustomHTML(std::string const &code, std::string const &title, std::string const &body)
{
	std::string customHTML =
        "<!DOCTYPE html>\n"
        "<html lang=\"en\">\n"
        "<head>\n"
        "    <meta charset=\"UTF-8\">\n"
        "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
        "    <title>" + title + "</title>\n"
		"    <link rel=\"icon\" href=\"favicon.ico\" type=\"image/x-icon\">\n"
		"    <link rel=\"stylesheet\" href=\"style.css\"\n"
        "</head>\n"
        "<body>\n" + body +
        // "    <div class=\"container\">\n" + body +
        // "    </div>\n"
        "</body>\n"
        "</html>\n";
	
	std::string response = 
		"HTTP/1.1 " + code + " " + title + "\n"
		"Date: " + getTimeStamp() + "\n" +
		"Server: Webserv/1.0.0 (Linux)\n" +
		"Connection: keep-alive\n" +
		"Content-Type: text/html; charset=utf-8\n" +
		"Content-Length: " + intToString(customHTML.length()) + "\n\n" + customHTML;

	return response;
} */
