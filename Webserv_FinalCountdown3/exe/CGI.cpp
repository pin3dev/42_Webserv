/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pin3dev <pinedev@outlook.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/06 12:22:17 by pin3dev           #+#    #+#             */
/*   Updated: 2024/06/07 19:59:11 by pin3dev          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CGI.hpp"

void	printVec(std::vector<char*> &vec)
{
	std::vector<char*>::iterator it = vec.begin();
	for (; it != vec.end(); it++)
		std::cout << *it << std::endl;
}

CGI::CGI(std::string fullPath, std::string uploadTo, std::string request, size_t requestLength) : _outFile(0), _status(0), _fullPath(fullPath),
_uploadTo(uploadTo), _request(request), _requestLength(requestLength), _environ()
{
	std::cout << "\nCGI CRIADO:\n"
		<< "outFile: " << _outFile << std::endl
		<< "status: " << _status << std::endl
		<< "fullPath: " << _fullPath << std::endl
		<< "uploadTo: " << _uploadTo << std::endl
		<< "request: " << _request << std::endl
		<< "requestLength: " << _requestLength << std::endl;
}

CGI::~CGI()
{
	std::cout << "STATUS DE OPERACAO: " <<  this->_status << std::endl;
}

void CGI::_exportEnvPath()
{
	this->_environ.push_back(NULL);
	//this->_args.push_back(const_cast<char*>("python3"));
	this->_args.push_back(const_cast<char*>(this->_fullPath.c_str()));
	this->_args.push_back(const_cast<char*>(this->_uploadTo.c_str()));
	this->_args.push_back(NULL);
}
void CGI::setNewEnv(std::string key, std::string value)
{
    if (!value.empty() && !key.empty())
    {
        std::string env = key + "=" + value;
        char* env_heap = new char[env.size() + 1];
        std::copy(env.begin(), env.end(), env_heap);
        env_heap[env.size()] = '\0'; 
        this->_environ.push_back(env_heap);
    }
}
 void	CGI::execute()
{
	//std::cout << "trying to execute...\n";
    this->_exportEnvPath();
	printVec(this->_args);
	printVec(this->_environ);
    this->_setOutFile();
    this->_writeRequestToCGI();
	pid_t pid = fork();

	if (pid == -1)
		throw std::runtime_error("500 Internal Server Error");

	if (pid == 0)
		this->_runCGIandWriteHTML();
	else
	{
		close(this->_pipefd[READ_END]);
		close(this->_outFile);
		waitpid(pid, &(this->_status), 0);
	}
}
void	CGI::_setOutFile()
{
	if ((this->_outFile = open("cgi.html", O_CREAT | O_WRONLY | O_TRUNC, 0644)) == -1)
		throw std::runtime_error("500 Internal Server Error");
}


ssize_t write_all(int fd, const void* buf, size_t count)
{
    size_t bytes_left = count;
    const char* buffer = (const char*)buf;
    while (bytes_left > 0) {
        ssize_t bytes_written = write(fd, buffer, bytes_left);
        if (bytes_written <= 0) {
            return -1; // erro ao escrever
        }
        bytes_left -= bytes_written;
        buffer += bytes_written;
    }
    return count;
}
//**FIQUEI SEM ENTENDER SE ISSO EH CHAMADO 2 VEZES PARA IMG
void	CGI::_writeRequestToCGI()
{
	if (pipe(this->_pipefd) == -1)
		throw std::runtime_error("500 Internal Server Error");
	
	if (this->_requestLength > 0)
		fcntl(this->_pipefd[WRITE_END], F_SETPIPE_SZ, this->_requestLength);
	
	//write_all(this->_pipefd[WRITE_END], this->_request.data(), this->_request.size());
	write(this->_pipefd[WRITE_END], this->_request.data(), this->_request.size());
	close(this->_pipefd[WRITE_END]);
} 

void	CGI::_runCGIandWriteHTML()
{
/* 	for(size_t i = 0; this->_args[i]; i++)
		std::cout << this->_args[i] << std::endl;
	for(size_t i = 0; this->_environ[i]; i++)
		std::cout << this->_environ[i] << std::endl; */
	dup2(this->_pipefd[READ_END], STDIN_FILENO);
	close(this->_pipefd[READ_END]);
	dup2(this->_outFile, STDOUT_FILENO);
	close(this->_outFile);

	std::string python3 = "/usr/bin/python3";


	if (execve(python3.c_str(), &(this->_args[0]), &(this->_environ[0])) == -1)
		throw std::runtime_error("500 Internal Server Error");
}



/*  
 ///-------------------------------------------------------------------------WALTER
void CGI::setNewEnv(std::string key, std::string value)
{
	std::string env;
	
    if (!value.empty() && !key.empty())
    {
		env = key + "=" + value;
        this->_environ.push_back(env);
    }
}

void		CGI::_setArgvEnvp()
{

		this->_args = new char*[4]; //ALOCA MEMÓRIA PARA 4 ARGV**
		this->_args[0] = strdup("python3"); //DEFINE A VERSÃO DO PYTHON PARA 3
		this->_args[1] = strdup(this->_fullPath.c_str()); //DEFINE O CAMINHO DO SCRIPT
		this->_args[2] = strdup(this->_uploadTo.c_str()); //DEFINE O DIRETÓRIO DE UPLOAD
		this->_args[3] = NULL; //DEFINE NULL NO FINAL DE ARGV**

	this->_envp = new char*[this->_environ.size() + 1]; //ALOCA MEMÓRIA PARA O TAMANHO DO ENV + 1
	for (size_t i = 0; i < this->_environ.size(); i++) //RODA O ENV ATÉ O FINAL
		this->_envp[i] = strdup(this->_environ[i].c_str()); //COPIA O ENV PARA ENV**
	this->_envp[this->_environ.size()] = NULL; //DEFINE NULL NO FINAL DE ENV**
}

void	CGI::_run()
{
	this->_setArgvEnvp();
	int file, status, fd[2]; //CRIA VARIAVEIS PARA ARMAZENAR O FD DO ARQUIVO, STATUS DO PROCESSO, E O PIPE
    //this->_exportEnvPath();

	this->_setPipeAndFile(file, fd); //ABRE O PIPE DE COMUNICAÇÃO ENTRE DOIS PROCESSOS USANDO UM ARQUIVO TEMPORARIO COMO CANAL
	//AQUI EXISTE O PIPE DE LEITURA ABERTO COM O ARQUIVO TEMPORARIO COM CONTEUDO DO REQUEST NELE
	pid_t pid = fork(); //CRIA UM PROCESSO FILHO

	if (pid == -1) //VERIFICA SE O PROCESSO FILHO PODE SER CRIADO
		throw std::runtime_error("500 Internal Server Error");

	if (pid == 0) //SE O PROCESSO FOR O FILHO
		this->_executeBin(file, fd); //EXECUTA O SCRIPT E GUARDA O OUTPUT NO ARQUIVO TEMPORÁRIO
	else
	{
		close(fd[READ_END]); //O PAI FECHA O FD DE LEITURA DO PIPE
		close(file); //O PAI FECHA O ARQUIVO TEMPORÁRIO
		waitpid(pid, &status, 0); //O PAI ESPERA O PROCESSO FILHO TERMINAR
	}

}
//CRIA ARQUIVO TEMPORÁRIO PARA 
void	CGI::_setPipeAndFile(int &file, int (&fd)[2])
{
	if ((file = open("cgi.html", O_CREAT | O_WRONLY | O_TRUNC, 0644)) == -1) //CRIA UM ARQUIVO TEMPORÁRIO PARA ARMAZENAR O OUTPUT DO SCRIPT E VERIFICA SE O ARQUIVO PODE SER CRIADO
		throw std::runtime_error("500 Internal Server Error");

	if (pipe(fd) == -1) //CRIA UM PIPE PARA COMUNICAÇÃO ENTRE O PROCESSO PAI E O FILHO E VERIFICA SE O PIPE PODE SER CRIADO
		throw std::runtime_error("500 Internal Server Error");
	
	if (this->_requestLength > 0) //SE O TAMANHO DO REQUEST FOR MAIOR QUE 0
		fcntl(fd[WRITE_END], F_SETPIPE_SZ, this->_requestLength); //DEFINE O TAMANHO DO FD DE ESCRITA DO PIPE PARA O TAMANHO DO REQUEST
	
	write(fd[WRITE_END], this->_request.data(), this->_requestLength); //ESCREVE NO PIPE DE ESCRITA O REQUEST
	close(fd[WRITE_END]); //FECHA O PIPE DE ESCRITA
}

void	CGI::_executeBin(int &file, int (&fd)[2])
{
	//NO PROCESSO FILHO
	dup2(fd[READ_END], STDIN_FILENO); //REDIRECIONA O STDIN DO PROCESSO FILHO PARA LER O PIPE (A ENTRADA DO MEU PROCESSO FILHO, QUE EXECUTARÁ O SCRIPT, SERÁ O REQUEST)
	close(fd[READ_END]); //FECHA O FD DE LEITURA DO PIPE
	dup2(file, STDOUT_FILENO); //REDIRECIONA O STDOUT DO PROCESSO FILHO PARA ESCREVER NO ARQUIVO TEMPORÁRIO (A SAIDA DO MEU PROCESSO FILHO, QUE SERÁ O RESULTADO DA EXECUÇÃO DO MEU SCRIPT, SERÁ ESCRITA NO ARQUIVO TEMPORÁRIO)
	close(file); //FECHA O ARQUIVO TEMPORÁRIO

	std::string python3 = "/usr/bin/python3";

	//PASSA O CAMINHO DO INTERPRETADOR DE PYTHON, OS ARGV, E O ENV PARA EXECUTAR O SCRIPT
	if (execve(python3.c_str(), this->_args, this->_envp) == -1) //EXECUTA O SCRIPT E VERIFICA SE O SCRIPT PODE SER EXECUTADO
		throw std::runtime_error("500 Internal Server Error");
}
 */
