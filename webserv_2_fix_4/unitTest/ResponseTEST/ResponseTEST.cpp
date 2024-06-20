/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseTEST.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pin3dev <pinedev@outlook.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/20 16:09:18 by pin3dev           #+#    #+#             */
/*   Updated: 2024/05/23 15:28:15 by pin3dev          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include "Location.hpp"

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

    std::string locName1;
    locName1 = "/";
    curMethods .push_back("GET");
    curMethods.push_back("DELETE");

    loc.root = "";
    loc.redirect = "";
    loc.tryFile = "";
    loc.cgiPath = "";
    loc.cgiExtension = "";
    loc.uploadTo = "";
    loc.autoindex = true;
    loc.hasCGI = false;
    loc.methods = curMethods;
    servLoc.insert(std::make_pair(locName1, loc));
        
    std::string locName2;
    locName2 = "/upload";
    //curMethods.push_back("DELETE");
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
        
        Response _myResponse = Response();
        //_myResponse.setRoot(root);
        _myResponse.setServLoc(servLoc);
        _myResponse.processRequest(url, method, root);
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
        
        Response _myResponse = Response();
        //_myResponse.setRoot(root);
        _myResponse.setServLoc(servLoc);
        _myResponse.processRequest(url, method, root);
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
        
        Response _myResponse = Response();
        //_myResponse.setRoot(root);
        _myResponse.setServLoc(servLoc);
        _myResponse.processRequest(url, method, root);
    } 
    catch (const std::exception &e){ std::cerr << e.what() << std::endl;}  
    try 
    {
        //TESTE 4 - VAI DAR ERRO
        std::string url = "/favicon.ico";
        std::string method = "GET";
        
        std::cout << "\n---------------------------------------------- TESTE 4 ----------------------------------------------\n";
        std::cout << "Root: " << root << std::endl;
        std::cout << "URL: " << url << std::endl;
        std::cout << "Method: " << method << std::endl << std::endl;
        
        Response _myResponse = Response();
        //_myResponse.setRoot(root);
        _myResponse.setServLoc(servLoc);
        _myResponse.processRequest(url, method, root);
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
        
        Response _myResponse = Response();
        //_myResponse.setRoot(root);
        _myResponse.setServLoc(servLoc);
        _myResponse.processRequest(url, method, root);
    } 
    catch (const std::exception &e){ std::cerr << e.what() << std::endl;}  
    try 
    {
        //TESTE 6 - VAI DAR ERRO
        std::string url = "/upload";
        std::string method = "GET";
        
        std::cout << "\n---------------------------------------------- TESTE 6 ----------------------------------------------\n";
        std::cout << "Root: " << root << std::endl;
        std::cout << "URL: " << url << std::endl;
        std::cout << "Method: " << method << std::endl << std::endl;
        
        Response _myResponse = Response();
        //_myResponse.setRoot(root);
        _myResponse.setServLoc(servLoc);
        _myResponse.processRequest(url, method, root);
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
        
        Response _myResponse = Response();
        //_myResponse.setRoot(root);
        _myResponse.setServLoc(servLoc);
        _myResponse.processRequest(url, method, root);
    } 
    catch (const std::exception &e){ std::cerr << e.what() << std::endl;}  

    return 0;
}
