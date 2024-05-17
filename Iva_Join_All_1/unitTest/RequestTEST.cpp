/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestTEST.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pin3dev <pinedev@outlook.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 17:55:04 by pin3dev           #+#    #+#             */
/*   Updated: 2024/05/17 19:34:58 by pin3dev          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#define  PATH_TO_TEST_FILE "RequestTEST3.txt"

#include <iostream>
#include <fstream>
#include "Request.hpp"

std::ostream &operator<<(std::ostream &os, const Request &request)
{
        os << "URL: " << request.getURL() << std::endl;
        os << "METHOD: " << request.getMethod() << std::endl;
        os << "PAYLOAD: " << request.getPayload() << std::endl;
        os << "BODY LENGTH: " << request.getBodyLength() << std::endl;
        os << "MAX LENGTH: " << request.getMaxLength() << std::endl;
        return (os);
}

int main()
{
	try
	{
                std::string testFileName = PATH_TO_TEST_FILE;
                std::ifstream	testFile(testFileName.c_str());
                if (!testFile.is_open())
                        return (std::cerr <<  "testREQUEST.txt not avalaible\n", 1);
                
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
