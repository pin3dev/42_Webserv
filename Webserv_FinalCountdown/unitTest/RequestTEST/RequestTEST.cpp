/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestTEST.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pin3dev <pinedev@outlook.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 17:55:04 by pin3dev           #+#    #+#             */
/*   Updated: 2024/05/27 21:43:06 by pin3dev          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#define  ABSOLUTE_PATH_TO_TEST_FILE "./RequestTEST/"

#include <iostream>
#include <fstream>
#include "Request.hpp"

//RequestTEST1.txt

std::ostream &operator<<(std::ostream &os, const Request &request)
{
        os << "URL: " << request.getURL() << std::endl;
        os << "METHOD: " << request.getMethod() << std::endl;
        os << "HOST: " << request.getHost() << std::endl;
        os << "PAYLOAD: " << request.getPayload() << std::endl;
        os << "BODY LENGTH: " << request.getBodyLength() << std::endl;
        os << "MAX LENGTH: " << request.getMaxLength() << std::endl;
        return (os);
}

int main()
{
	try
	{
                std::cout << "Wich file do you want to test? <1, 2 or 3>\n";
                std::string numb;
                
                std::cin >> numb;
                std::string filename = "RequestTEST";
                filename += numb;
                filename += ".txt";
                
                std::cout << "TRYING ACESS TO " << ABSOLUTE_PATH_TO_TEST_FILE << filename << " file..." << std::endl;
                
                std::string testFileName = ABSOLUTE_PATH_TO_TEST_FILE + filename;
                std::ifstream	testFile(testFileName.c_str());
                if (!testFile.is_open())
                        return (std::cerr << testFileName << " not avalaible\n", 1);
                
                std::stringstream HTTPrequest;
                HTTPrequest << testFile.rdbuf();
                testFile.close();
                std::string strRequest = HTTPrequest.str();
                size_t buffer = strRequest.size();
                
                Request _myRequest;
                _myRequest.toAppend(strRequest.c_str(), buffer);
		_myRequest.checkStatusRequest();
                std::cout << _myRequest;
	}
	catch(const std::exception& e)
	{
		std::cerr << "RESPONSE GENERATOR ERROR PAGE:" << e.what() << '\n';
	}
        
}
