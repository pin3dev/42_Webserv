#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <stdexcept>
#include <sys/stat.h>
#include <unistd.h> // For unlink and rmdir
#include <dirent.h> //to dir


// Definição da estrutura location_s
typedef struct location_s {
    std::string root;
    std::string redirect;
    std::string tryFile;
    std::string cgiPath;
    std::string cgiExtension;
    std::string uploadTo;
    bool autoindex;
    bool hasCGI;
    std::vector<std::string> methods;
} location_t;




bool directoryExists(std::string const &fullpath)
{
	struct stat info;

	if (stat(fullpath.c_str(), &info) != 0)
		return ((std::cout << "O ARQUIVO: " << fullpath << " NÃO É UM DIRETORIO!\n"), false);
	else if (info.st_mode & S_IFDIR) 
		return ((std::cout << "O ARQUIVO: " << fullpath << " É UM DIRETORIO!\n"), true);
	return ((std::cout << "O ARQUIVO: " << fullpath << " NÃO É UM DIRETORIO!\n"), false);

}

bool fileExists(std::string const &fullpath)
{
	struct stat info;

	if (stat(fullpath.c_str(), &info) != 0)
		return ((std::cout << "O CAMINHO: " << fullpath << " NÃO É UM ARQUIVO!\n"), false);
	else if (info.st_mode & S_IFREG) 
		return ((std::cout << "O CAMINHO: " << fullpath << " É UM ARQUIVO!\n"), true);
	return ((std::cout << "O CAMINHO: " << fullpath << " NÃO É UM ARQUIVO!\n"), false);
}

void serveFile(const std::string &path) {
    std::cout << "Serving file: " << path << std::endl;
}

void serveError(int code, const std::string &message) {
    std::cout << "Error " << code << ": " << message << std::endl;
}

bool isMethodAllowed(const std::vector<std::string> &methods, const std::string &method) {
    bool isAllowed = std::find(methods.begin(), methods.end(), method) != methods.end();
    if (isAllowed)
       	return ((std::cout << "O METODO: " << method << " É PERMITIDO NO LOCATION!\n"), true);
    return ((std::cout << "O METODO: " << method << " NÃO É PERMITIDO NO LOCATION!\n"), false);
}

// Função para processar POST
void processPost(const std::string &path) {
    // Suponha que estamos recebendo dados para salvar em um arquivo
    // Esta parte deve lidar com a leitura do corpo da requisição e salvar o conteúdo no path
    std::cout << "Processing POST request, saving data to: " << path << std::endl;
    // Implementação fictícia para salvar dados
    // Aqui você colocaria a lógica real para ler o corpo do POST e salvar o conteúdo
}

// Função para processar DELETE
void processDelete(const std::string &path) {
    std::cout << "Processing DELETE request, deleting data from: " << path << std::endl;
}

void serveAutoindex(const std::string &path)
{
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(path.c_str())) != NULL)
	{
        std::cout << "Index of " << path << ":\n";
        while ((ent = readdir(dir)) != NULL)
            std::cout << ent->d_name << "\n";
        closedir(dir);
    }
	else
        throw std::runtime_error("500 Unable to open directory");
}

bool serveTryFile(const std::string &tryFilePath)
{
    if (fileExists(tryFilePath))
	{
        serveFile(tryFilePath);
        return true;
    }
    return false;
}

// Função principal para processar a requisição
void processRequest(const std::string &url, const std::string &method, const std::map<std::string, location_t> &locations, const std::string &root) {
    this->_urlLoopCount++;
    std::string effectiveRoot = root;
    std::string effectiveUrl = url;
    std::cout << "EFETUANDO PROCESSAMENTO DE REQUEST PARA ROOT: " << effectiveRoot << " E URL: " << effectiveUrl << std::endl;

    std::map<std::string, location_t>::const_iterator it = locations.begin();
    for (; it != locations.end(); ++it) {
        if (url.find(it->first) == 0) {
            std::cout << "LOCATION: " << it->first << " ACHADO PARA: " << url << std::endl;
            const location_t &loc = it->second;
            if (!loc.redirect.empty()) {
                effectiveUrl = loc.redirect;
                std::cout << "REDIRECIONANDO DE: " << url << " PARA: " << effectiveUrl << std::endl;
                processRequest(effectiveUrl, method, locations, root);
                return;
            }
            if (!loc.root.empty()) {
                std::cout << "ROTEANDO DE: " << root << " PARA: " << effectiveRoot << std::endl;
                effectiveRoot = loc.root;
            }
            if (!isMethodAllowed(loc.methods, method)) {
                throw std::runtime_error("405 Method Not Allowed");
                return;
            }
        }
    }

    std::string fullPath = effectiveRoot + effectiveUrl;

    if (method == "GET") {
        std::cout << "EFETUANDO METODO GET PARA PATH: " << fullPath << std::endl; 
        if (directoryExists(fullPath)) {
            if (fileExists(fullPath + "/index.html")) {
                serveFile(fullPath + "/index.html");
            } else if (!locations.at("/").tryFile.empty() && serveTryFile(effectiveRoot + locations.at("/").tryFile)) { //guardar o location no objeto e verificar se há um location
                return;
            } else if (locations.at("/").autoindex) { //guardar o location no objeto e verificar se há um location
                serveAutoindex(fullPath);
            } else {
                throw std::runtime_error("403 Forbidden: No index file in directory");
            }
        } else if (fileExists(fullPath)) {
            serveFile(fullPath);
        } else if (!locations.at("/").tryFile.empty() && serveTryFile(effectiveRoot + locations.at("/").tryFile)) {
            return;
        } else {
            throw std::runtime_error("404 Not Found: File or directory does not exist");
        }
    } else if (method == "POST") {
        std::cout << "EFETUANDO METODO POST PARA PATH: " << fullPath << std::endl; 
        processPost(fullPath);
    } else if (method == "DELETE") {
        std::cout << "EFETUANDO METODO DELETE PARA PATH: " << fullPath << std::endl; 
        processDelete(fullPath);
    } else {
        throw std::runtime_error("405 Method Not Allowed");
    }
}


void printMapLocation(std::map<std::string, location_t> &servLoc)
{
    for (std::map<std::string, location_t>::iterator it = servLoc.begin(); it != servLoc.end(); ++it)
    {
        std::cout << "-----------------------------------\n";
        std::cout << "Location: " << it->first << std::endl;
        std::cout << "Root: " << it->second.root << std::endl;
        std::cout << "Redirect: " << it->second.redirect << std::endl;
        std::cout << "TryFile: " << it->second.tryFile << std::endl;
        std::cout << "CgiPath: " << it->second.cgiPath << std::endl;
        std::cout << "CgiExtension: " << it->second.cgiExtension << std::endl;
        std::cout << "UploadTo: " << it->second.uploadTo << std::endl;
        std::cout << "Autoindex: " << it->second.autoindex << std::endl;
        std::cout << "HasCGI: " << it->second.hasCGI << std::endl;
        std::cout << "Methods: ";
        for (std::vector<std::string>::iterator it2 = it->second.methods.begin(); it2 != it->second.methods.end(); ++it2)
            std::cout << *it2 << " ";
        std::cout << std::endl;
        std::cout << "-----------------------------------\n";
    }
}

void fillMapLocation(std::map<std::string, location_t> &servLoc)
{
    location_t loc = {};
    std::vector<std::string> curMethods;
    curMethods.clear();

/*     std::string locName1;
    locName1 = "/";
    curMethods .push_back("GET");
    loc.root = "";
    loc.redirect = "";
    loc.tryFile = "";
    loc.cgiPath = "";
    loc.cgiExtension = "";
    loc.uploadTo = "";
    loc.autoindex = true;
    loc.hasCGI = false;
    loc.methods = curMethods;
    servLoc.insert(std::make_pair(locName1, loc)); */
        
    std::string locName2;
    locName2 = "/upload";
    curMethods.push_back("DELETE");
    loc.root = "";
    loc.redirect = "";
    loc.tryFile = "file.html";
    loc.cgiPath = "";
    loc.cgiExtension = "";
    loc.uploadTo = "";
    loc.autoindex = false;
    loc.hasCGI = false;
    loc.methods = curMethods;
    servLoc.insert(std::make_pair(locName2, loc));

    curMethods.clear();

    std::string locName3;
    locName3 = ".py";
    curMethods.push_back("GET");
    curMethods.push_back("POST");
    loc.root = "";
    loc.redirect = "";
    loc.tryFile = "file.html";
    loc.cgiPath = "/cgi";
    loc.cgiExtension = ".py";
    loc.uploadTo = "/upload";
    loc.autoindex = false;
    loc.hasCGI = true;
    loc.methods = curMethods;
    servLoc.insert(std::make_pair(locName3, loc));

    curMethods.clear();

    std::string locName4;
    locName4 = "/favicon.ico";
    curMethods.push_back("GET");
    loc.root = "";
    loc.redirect = "";
    loc.tryFile = "";
    loc.cgiPath = "";
    loc.cgiExtension = "";
    loc.uploadTo = "";
    loc.autoindex = false;
    loc.hasCGI = false;
    loc.methods = curMethods;
    servLoc.insert(std::make_pair(locName4, loc));

    std::string locName5;
    locName5 = "/redirect";
    loc.root = "";
    loc.redirect = "/index.html";
    loc.tryFile = "";
    loc.cgiPath = "";
    loc.cgiExtension = "";
    loc.uploadTo = "";
    loc.autoindex = false;
    loc.hasCGI = false;
    loc.methods = curMethods;
    servLoc.insert(std::make_pair(locName5, loc));
    
    printMapLocation(servLoc);
    std::string resource = "";
    std::string method = "";
    std::string root = "";
}

int main()
{
    std::map<std::string, location_t> servLoc;
    servLoc.clear();

    fillMapLocation(servLoc);
    std::string root = "www/site1";
    
    try 
    {
        //TESTE 1 - TEM QUE EXECUTAR A FUNÇÃO 
        std::string url = "/";
        std::string method = "GET";
        
        std::cout << "\n---------------------------------------------- TESTE 1 ----------------------------------------------\n";
        std::cout << "Root: " << root << std::endl;
        std::cout << "URL: " << url << std::endl;
        std::cout << "Method: " << method << std::endl << std::endl;
        
        processRequest(url, method, servLoc, root);
    } 
    catch (const std::exception &e){ std::cerr << e.what() << std::endl;}
    try 
    {
        //TESTE 2 - VAI DAR ERRO
        std::string url = "/upload";
        std::string method = "POST";
        
        std::cout << "\n---------------------------------------------- TESTE 2 ----------------------------------------------\n";
        std::cout << "Root: " << root << std::endl;
        std::cout << "URL: " << url << std::endl;
        std::cout << "Method: " << method << std::endl << std::endl;
        
        processRequest(url, method, servLoc, root);
    } 
    catch (const std::exception &e){ std::cerr << e.what() << std::endl;}        
    try 
    {
        //TESTE 3 - VAI DAR CERTO
        std::string url = "/OI.py";
        std::string method = "POST";

        std::cout << "\n---------------------------------------------- TESTE 3 ----------------------------------------------\n";
        std::cout << "Root: " << root << std::endl;
        std::cout << "URL: " << url << std::endl;
        std::cout << "Method: " << method << std::endl << std::endl;
        
        processRequest(url, method, servLoc, root);
    } 
    catch (const std::exception &e){ std::cerr << e.what() << std::endl;}  
    try 
    {
        //TESTE 4 - VAI DAR ERRO
        std::string url = "/favicon.ico";
        std::string method = "POST";
        
        std::cout << "\n---------------------------------------------- TESTE 4 ----------------------------------------------\n";
        std::cout << "Root: " << root << std::endl;
        std::cout << "URL: " << url << std::endl;
        std::cout << "Method: " << method << std::endl << std::endl;
        
        processRequest(url, method, servLoc, root);
    } 
    catch (const std::exception &e){ std::cerr << e.what() << std::endl;}  
    try 
    {
        //TESTE 5 - VAI PERMITIR MAS DEPOIS DEVE DAR ERRO
        std::string url = "/redirect";
        std::string method = "DELETE";
        
        std::cout << "\n---------------------------------------------- TESTE 5 ----------------------------------------------\n";
        std::cout << "Root: " << root << std::endl;
        std::cout << "URL: " << url << std::endl;
        std::cout << "Method: " << method << std::endl << std::endl;
        

        processRequest(url, method, servLoc, root);
    } 
    catch (const std::exception &e){ std::cerr << e.what() << std::endl;}  
    try 
    {
        //TESTE 6 - VAI DAR ERRO
        std::string url = "/redirect/arquivo.txt";
        std::string method = "DELETE";
        
        std::cout << "\n---------------------------------------------- TESTE 6 ----------------------------------------------\n";
        std::cout << "Root: " << root << std::endl;
        std::cout << "URL: " << url << std::endl;
        std::cout << "Method: " << method << std::endl << std::endl;
        
        processRequest(url, method, servLoc, root);
    } 
    catch (const std::exception &e){ std::cerr << e.what() << std::endl;}
    try 
    {
        //TESTE 7 - VAI DAR CERTO
        std::string url = "/";
        std::string method = "POST";
        
        std::cout << "\n---------------------------------------------- TESTE 7 ----------------------------------------------\n";
        std::cout << "Root: " << root << std::endl;
        std::cout << "URL: " << url << std::endl;
        std::cout << "Method: " << method << std::endl << std::endl;
        
        processRequest(url, method, servLoc, root);
    } 
    catch (const std::exception &e){ std::cerr << e.what() << std::endl;} 

    return 0;
} 
