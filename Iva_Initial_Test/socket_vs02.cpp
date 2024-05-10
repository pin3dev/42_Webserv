/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket_vs02.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pin3dev <pinedev@outlook.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/29 11:18:33 by pin3dev           #+#    #+#             */
/*   Updated: 2024/05/01 12:49:27 by pin3dev          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <iomanip>

extern char **environ;


#define RESET    "\x1b[0m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define RED    "\x1b[31m"
#define GRAY  "\x1b[37m"
#define MAGENTA "\x1b[35m"
#define BKGRAY	"\x1b[47;90m"
#define CYAN    "\x1b[36m"

//COM SCRIPT PYTHON
int main() 
{
    //--------------------------------------------------SOCKET--------------------------------------------------
    //[SOCKET]: VARIAVEIS DE CONFIGURAÇÃO DE SOCKET
    int sockfd, newsockfd, portno = 8080;
    socklen_t clilen;
    char tmp_buffer[2048];
    struct sockaddr_in serv_addr, cli_addr;
    int n;

    //[SOCKET]: CRIAÇÃO DO SOCKET
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
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

    //[SOCKET]: VINCULAÇÃO DO SOCKET A ENDEREÇO LOCAL
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "ERROR on binding" << std::endl;
        return 1;
    }

    //[SOCKET]: ESPERA POR CONEXÃO
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);

    //--------------------------------------------------REQUEST--------------------------------------------------

    //[SOCKET]: ACEITA CONEXÃO
    newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
    if (newsockfd < 0) {
        std::cerr << "ERROR on accept" << std::endl;
        return 1;
    }

    //[REQUEST]: LEITURA DO SOCKET
    n = read(newsockfd, tmp_buffer, 2047);
    if (n < 0) {
        std::cerr << "ERROR reading from socket" << std::endl;
        return 1;
    }
    tmp_buffer[n] = '\0';

    //[REQUEST]: IMPRESSÃO DO CONTEÚDO DO VERBO HTTP
    std::cout << std::setw(60) << MAGENTA "<<<<< REQUEST MESSAGE >>>>>\n" RESET 
    << tmp_buffer << std::endl 
    << std::setw(60) << MAGENTA "<<<<< END OF REQUEST >>>>>\n" RESET;

    //[REQUEST]: IMPRESSÃO DO CONTEÚDO DA VARIÁVEL DE AMBIENTE NO SERVIDOR
    std::cout << std::setw(60) << CYAN "<<<<< ENV VARIBLES >>>>>\n" RESET;
    for (char **env = environ; *env != 0; env++)
    {
        char *thisEnv = *env;
        std::cout << thisEnv << std::endl;
    }
    std::cout << std::setw(60) << CYAN "<<<<< END OF ENV VARIBLES >>>>>\n" RESET;

    //--------------------------------------------------ANSWER--------------------------------------------------

    //[ANSWER]: VERIFICAÇÃO DO CONTEÚDO DO VERBO HTTP
    if (strstr(tmp_buffer, "GET / HTTP/1.1"))
    {
        //[ANSWER]: ENVIAR CONTEÚDO DO ARQUIVO HTML
        int pipefd[2];
        pipe(pipefd);

        // Criação de processo filho
        pid_t pid = fork();
        if (pid == 0) { // Processo filho
            // Redireciona a saída padrão para o pipe
            dup2(pipefd[1], STDOUT_FILENO);
            close(pipefd[0]);
            close(pipefd[1]);

            // Execução do script CGI
            execl("./cgi_vs02.py", "cgi_vs02.py", NULL);
        } 
        else
        { // Processo pai
            // Fecha a extremidade de escrita do pipe
            close(pipefd[1]);

            // Leitura da saída do script CGI
            char buffer[2048];
            ssize_t bytes_read;
            //[ANSWER]: ENVIAR CONTEÚDO DO ARQUIVO HTML
            while ((bytes_read = read(pipefd[0], buffer, sizeof(buffer))) > 0) {
                // Escrever a saída do script CGI no socket
                write(newsockfd, buffer, bytes_read);
            }
            if (bytes_read < 0) {
                std::cerr << "ERROR reading from pipe" << std::endl;
                return 1;
            }
        }
        // Espera pelo término do processo filho
        wait(NULL);
    }
    //[SOCKET]: FECHAR CONEXÃO
    close(newsockfd);
    //[SOCKET]: FECHAR SOCKET
    close(sockfd);
    //[PROCESS]: FINALIZAR PROGRAMA
    return 0;
}