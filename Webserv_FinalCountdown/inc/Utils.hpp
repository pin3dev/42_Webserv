/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pin3dev <pinedev@outlook.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/27 15:11:42 by pin3dev           #+#    #+#             */
/*   Updated: 2024/06/05 15:50:09 by pin3dev          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <string>
#include <ctime>
#include <vector>
#include <map>
#include <sstream>
#include <sys/stat.h>
#include <dirent.h> //to dir
#include <fstream>

class Utils
{
    private:
        Utils();
        ~Utils();
        
    public:
        static time_t _nowTime();
        static std::string _getTimeStamp(const char *format);
        static std::vector<std::string> _split(std::string const &str, char delim);
        static void _trim(std::string &s, char const *set);
        static std::string _decoder(const std::string &url);
        static bool _isAt(const std::string &str, const std::string arr[3]);
        static bool	isExtensionOf(std::string const &ext, std::string const &fullname);
        static bool isParentDirOf(const std::string& parent, const std::string& son);
        static bool defaultGetUrl(std::string const &url, std::string const &method);
        static bool _isRootFile(const std::string& url);
        static bool  _isRoot(std::string const &url);
        static std::string _itoa(int n);
        static bool fileExists(std::string const &fullpath);
        static bool directoryExists(std::string const &fullpath);
        static void print_invisibles(const std::string& text);

        static std::string generateResponseOK(std::string const &path);
        static std::string getFileContent(std::string const &path);
        static std::string getFileType(std::string const &file);



};
