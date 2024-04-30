/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket_vs01.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pin3dev <pinedev@outlook.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/29 11:18:33 by pin3dev           #+#    #+#             */
/*   Updated: 2024/04/30 14:25:17 by pin3dev          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

//VERSÃO COM HTML ESTÁTICO
int main()
{
    //[SOCKET]: VARIAVEIS DE CONFIGURAÇÃO DE SOCKET
    int sockfd, newsockfd, portno = 8080;
    socklen_t clilen;
    char tmp_buffer[2048];
    struct sockaddr_in serv_addr, cli_addr;

    //[SOCKET]: VARIAVEIS DE LEITURA E ESCRITA
    std::ifstream file;
    int n;

    //[SOCKET]: CRIAÇÃO DO SOCKET
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        std::cerr << "ERROR opening socket" << std::endl;
        return 1;
    }

    //[SOCKET]: CONFIGURAÇÃO DE REUTILIZAÇÃO DE ENDEREÇO
    int optval = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    //[SOCKET]: CONFIGURAÇÃO DO SOCKET
    memset((char *)&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    //[SOCKET]: VINCULAÇÃO DO SOCKET
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        std::cerr << "ERROR on binding" << std::endl;
        return 1;
    }

    //[SOCKET]: ESPERA POR CONEXÃO
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);

    //[SOCKET]: ACEITA CONEXÃO
    newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
    if (newsockfd < 0)
    {
        std::cerr << "ERROR on accept" << std::endl;
        return 1;
    }


    /* TODO: SUBSTITUIR LEITURA DO SOCKET POR STRING
    std::string buffer;
    char tmp_buffer[1024];
    ssize_t bytes_read;
    while ((bytes_read = read(newsockfd, tmp_buffer, sizeof(tmp_buffer))) > 0)
    {
        buffer.append(tmp_buffer, bytes_read);
    }

    if (buffer.find("GET / HTTP/1.1") != std::string::npos)
    {
        // ...
    }
    */
   
    //[REQUEST]: LEITURA DO SOCKET
    n = read(newsockfd, tmp_buffer, 2048);
    if (n < 0)
    {
        std::cerr << "ERROR reading from socket" << std::endl;
        return 1;
    }
    tmp_buffer[n] = '\0';

    //[ANSWER]: VERIFICAÇÃO DO CONTEÚDO DO VERBO HTTP
    if (strstr(tmp_buffer, "GET / HTTP/1.1"))
    {
        //[ANSWER]: ABRIR ARQUIVO HTML ESTÁTICO
        file.open("sitetest_vs01.html");
        if (!file.is_open())
        {
            std::cerr << "Failed to open script file" << std::endl;
            return 1;
        }

        //[ANSWER]: ENVIAR CABEÇALHO HTTP
        write(newsockfd, "HTTP/1.1 200 OK\nContent-type: text/html\n\n", 41);

        //[ANSWER]: ENVIAR CONTEÚDO DO ARQUIVO HTML
        while (!file.eof())
        {
            file.getline(tmp_buffer, sizeof(tmp_buffer));
            write(newsockfd, tmp_buffer, strlen(tmp_buffer));
            write(newsockfd, "\n", 1); // Adicionar nova linha após cada linha do arquivo
        }
        //[ANSWER]: FECHAR ARQUIVO
        file.close();
    }
    //[SOCKET]: FECHAR CONEXÃO
    close(newsockfd);
    //[SOCKET]: FECHAR SOCKET
    close(sockfd);
    //[PROCESS]: FINALIZAR PROGRAMA
    return 0;
} 


/* //COM SCRIPT PYTHON
int main() {
    int sockfd, newsockfd, portno = 8080;
    socklen_t clilen;
    char buffer[2048];
    struct sockaddr_in serv_addr, cli_addr;
    int n;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cerr << "ERROR opening socket" << std::endl;
        return 1;
    }

    memset((char *)&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "ERROR on binding" << std::endl;
        return 1;
    }

    listen(sockfd, 5);
    clilen = sizeof(cli_addr);

    newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
    if (newsockfd < 0) {
        std::cerr << "ERROR on accept" << std::endl;
        return 1;
    }

    n = read(newsockfd, buffer, 2047);
    if (n < 0) {
        std::cerr << "ERROR reading from socket" << std::endl;
        return 1;
    }
    buffer[n] = '\0';

    if (strstr(buffer, "GET / HTTP/1.1")) {
        // Redirecionar a saída do script Python para o socket
        dup2(newsockfd, STDOUT_FILENO);

        // Executar o script Python
        const char *argv[] = { "cgi_vs01.py", NULL };
        execve("./cgi_vs01.py", const_cast<char **>(argv), NULL);
        
        // Se execve() retornar, ocorreu um erro
        std::cerr << "Failed to execute script" << std::endl;
        return 1;
    }

    close(newsockfd);
    close(sockfd);
    return 0;
} */