
#include "CGI.hpp"

CGI::CGI(std::string fullPath, std::string uploadTo, std::vector<char> &payload, size_t requestLength) : _outFile(0), _status(0), _fullPath(fullPath),
_uploadTo(uploadTo), _requestLength(requestLength), _payload(payload), _environ()
{}

CGI::~CGI()
{
	for (size_t i = 0; i < this->_environ.size(); ++i)
        delete[] _environ[i];
}

void _handle_timeout(pid_t pid)
{
    kill(pid, SIGKILL);
}

bool _hasTimeOut(time_t start_time)
{
	const int timeout = 1;
	return (Utils::_nowTime() - start_time > timeout);
}

void CGI::_exportEnvPath()
{
	this->_environ.push_back(NULL);
	this->_args.push_back(const_cast<char*>("python3"));
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
	std::string tmp = ".inputFile";
	FILE* inputFilePtr = NULL;
	int	 inputFileFD;
	
    this->_exportEnvPath();
    this->_setOutFile();
    if ((inputFileFD = this->_writeRequestToCGI(tmp, inputFilePtr)) == -1)
		throw std::runtime_error(Utils::_defaultErrorPages(500, "O servidor nao foi capaz de escrever o request para o CGI"));

	pid_t pid = fork();

	if (pid == -1)
		throw std::runtime_error(Utils::_defaultErrorPages(500, "O servidor nao foi capaz de abrir um processo filho para o CGI"));

	if (pid == 0)
	{
		dup2(inputFileFD, STDIN_FILENO);
		close(inputFileFD);

		dup2(this->_outFile, STDOUT_FILENO);
		close(this->_outFile);

		std::string python3 = "/usr/bin/python3";

		if (execve(python3.c_str(), &(this->_args[0]), &(this->_environ[0])) == -1)
			throw std::runtime_error(Utils::_defaultErrorPages(500, "O servidor nao conseguiu executar o CGI"));
		exit (1);
	}	
	else
	{
		close(inputFileFD);
		close(this->_outFile);
		//waitpid(pid, &(this->_status), 0);
 		time_t start_time = Utils::_nowTime();
		while (true)
		{
			pid_t result = waitpid(pid, &(this->_status), WNOHANG);
			if (result < 0)
				throw std::runtime_error(Utils::_defaultErrorPages(500, "O servidor detectou um erro ao executar o CGI"));
			else if (result == 0)
			{
				if (_hasTimeOut(start_time))
				{
					_handle_timeout(pid);
					throw std::runtime_error(Utils::_defaultErrorPages(500, "O cgi entrou em timeout"));
					break;
				}
			}
			else
				break;
		}
	}
}

void	CGI::_setOutFile()
{
	if ((this->_outFile = open("cgi.html", O_CREAT | O_WRONLY | O_TRUNC, 0644)) == -1)
		throw std::runtime_error(Utils::_defaultErrorPages(500, "O servidor nao foi capaz de criar saida para o CGI escrever"));
}

int CGI::_writeRequestToCGI(std::string& fname, FILE*& filePtr)
{
	std::ofstream inputFile(fname.c_str());
	if (!inputFile.is_open())
		return (-1);
	
	for (size_t i = 0; i < _payload.size(); i++)
		inputFile << _payload[i];
	inputFile.close();

	filePtr = std::fopen(fname.c_str(), "r");
	if (filePtr == NULL)
		return (-1);
	return(fileno(filePtr));
} 

