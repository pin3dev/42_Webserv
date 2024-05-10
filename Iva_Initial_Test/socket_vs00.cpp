/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pin3dev <pinedev@outlook.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/29 10:12:36 by pin3dev           #+#    #+#             */
/*   Updated: 2024/04/29 10:42:20 by pin3dev          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <cstring>      // Necessário para memset
#include <sys/socket.h> // Bibliotecas para sockets
#include <netinet/in.h> // Bibliotecas para endereços de internet
#include <unistd.h> // Bibliotecas para close, read, write

int main() 
{
    int sockfd, newsockfd, portno = 8080;
    socklen_t clilen;
    char buffer[5000];
    struct sockaddr_in serv_addr, cli_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        std::cerr << "ERROR opening socket" << std::endl;
        return 1;
    }

    memset((char *)&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
        std::cerr << "ERROR on binding" << std::endl;
        return 1;
    }

    listen(sockfd,5);
    clilen = sizeof(cli_addr);

    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0)
    {
        std::cerr << "ERROR on accept" << std::endl;
        return 1;
    }

    int n;
    std::string total_message;
    while ((n = read(newsockfd, buffer, 4999)) > 0)
    {
        buffer[n] = '\0'; // Null-terminate the buffer
        total_message += buffer;
    }
    if (n < 0)
    {
        std::cerr << "ERROR reading from socket" << std::endl;
        return 1;
    }

    std::cout << "Here is the message: " << total_message << std::endl;

    close(newsockfd);
    close(sockfd);
    return 0;
}
