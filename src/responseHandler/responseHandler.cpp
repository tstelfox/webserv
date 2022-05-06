/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   responseHandler.cpp                                  :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/01 18:30:39 by tmullan       #+#    #+#                 */
/*   Updated: 2022/05/01 18:30:39 by tmullan       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "responseHandler.hpp"
#include "colours.hpp"
#include <iostream>
#include <map>
#include <fstream>
#include <sstream>
#include <sys/stat.h> // For checking if directory
#include <dirent.h> // For getting directory contents
#include <time.h> // time for time


responseHandler::responseHandler(std::string requestLine, WSERV::serverConfig const &configs,
                                 std::map <std::string, std::string> &fields)
        : _requestLine(requestLine), _config(configs), _requestFields(fields) {
//    std::cout << ITALIC << COLOR_NEON << "Brr I find out what to respond now" << FORMAT_RESET << RESET_COLOUR << std::endl;
}

responseHandler::responseHandler() {}

responseHandler::~responseHandler() {}

std::string responseHandler::parseAndRespond(int status, int method, std::string uri) {
    if (status != 200)
        return respondError(status);

    /* Actual normal parsing of a normal request */
    std::cout << "Request Line is: " << _requestLine << std::endl;
    switch (method) {
        case 1:
//            std::cout << "GET request" << std::endl;
            return getResponse(uri);
            break;
        case 2:
            std::cout << "POST request" << std::endl;
//            return postResponse(uri);
            break;
        case 3:
            std::cout << "DELETE request" << std::endl;
//            return deleteResponse(uri);
    }
    return "Placeholder"; // TODO
}

std::string responseHandler::getResponse(std::string uri) {
//    std::cout << "GET whatever is at " << uri << std::endl;



    /*Compare the uri against the locations
    in order to find the correct locations configs
    including the root.
    BUT for now just improvise something simple
     */

    std::string requestedFile = _config.get_Location_vec()[0].get_root(); // PLACEHOLDER TODO
    if (!uri.compare("/"))
        uri += "index.html";
    requestedFile += uri;
    std::ifstream myFile;
    std::cout << "Path to be opened: " << requestedFile << std::endl;

    // If a Directory is requested
    struct stat s;
    if (lstat(requestedFile.c_str(), &s) == 0) {
        if (S_ISDIR(s.st_mode)) {
            if (!_config.get_Location_vec()[0].get_autoindex()) // TODO when locations work
                return respondError(403);
            else {
                return buildDirectoryListing(requestedFile);
            }
        }
    }

    myFile.open(requestedFile);
    if (myFile.fail()) { // Check if it is a directory and then if autoindex is set on or off
        return respondError(404);
    }
    std::ostringstream fileContent;
    fileContent << myFile.rdbuf();
    std::string responseBody = fileContent.str();
    myFile.close();

//    std::cout << "Opened content: " << fileContent << std::endl;

    std::string responseHeader;
    responseHeader = buildHttpLine(200);
    responseHeader += buildDateLine();
    responseHeader += "Content-type: text/html; charset=UTF-8\nContent-Length:";
    responseHeader.append(std::to_string(responseBody.size()));
    responseHeader.append("\n\n");
    responseHeader.append(responseBody + "\n");
    std::cout << "RESPONSE: " << responseHeader << std::endl;

    /*Check that the method in question is an allowed method*/

    /* Max file Size will only be important for POST I believe */

    /* Default file when a directory is requested is just the index - Include in parsing */

    /* Autoindex off means that accessing a directory gives a 403
     * OTHERWISE I'll have to make a html file which lists directory contents
     * _WITH_ links? hmmmmmm
     */



    return responseHeader;
}

std::string responseHandler::respondError(int status) {

    std::string response;
    response = buildHttpLine(status);
    response += buildDateLine();

    //Content
    response += "Content-type: text/html; charset=UTF-8\nContent-Length:";
    // Extract file
    std::string body = extractErrorFile(status);
    std::cout << "Bro" << body << std::endl;
    response.append(std::to_string(body.size()));
    //Error files
    response += "\nConnection: close";

    //End of Header
    response.append("\n\n");
    std::cout << RED << "<<<<-------- The response header ------->>>>\n" << RESET_COLOUR << response << std::endl;
    response.append(body + "\n");
    return response;
}

std::string responseHandler::extractErrorFile(int status) { // So there is still some sheet here
    std::ifstream errFile;
    std::string path = _config.get_error_page();
//    std::cout << "de boia " << path << std::endl;
    path += std::to_string(status) + ".html";
//    std::cout << "File path: " << path << std::endl;
    errFile.open(path);
    if (errFile.fail()) {
        // panic hard
    }
    std::ostringstream fileContent;
    fileContent << errFile.rdbuf();
    return fileContent.str();
}

std::string responseHandler::buildDirectoryListing(std::string &directory) {
    std::cout << "Gotta make the directory page for: " << directory << std::endl;

    /* nginx lists directories first in alphabetical order then files in alphabetical order
     * Date and time of creation or modification for each file
     * And then size of file "-" for directories
     * */

    DIR *dh;
    struct dirent *contents;

    std::set <std::string> fileSet;
    std::set <std::string> directorySet;
    dh = opendir(directory.c_str());
    if (!dh)
        std::cout << "No such directory as " << directory << std::endl;
    else {
        while ((contents = readdir(dh)) != NULL) {
            std::string name = contents->d_name;
            if (!name.compare("."))
                continue;
            struct stat s;
            std::string path = directory + "/" + name;
            if (lstat(path.c_str(), &s) == 0) {
                if (name.compare("..")) {
                    struct tm *timeInfo = localtime(&s.st_ctime);
                    std::string date = std::to_string(timeInfo->tm_mday) + "-" + std::to_string(timeInfo->tm_mon) + "-" \
 + std::to_string(timeInfo->tm_year + 1900);
                    int justification = 68 - name.length() + date.length(); // 67 seems to be nginx's thing
                    name.append(justification, ' ');
                    name += date;
                    name.append(19, ' '); // 19 spaces'
                } else
                    name += "/";
                if (S_ISDIR(s.st_mode)) {
                    if (name.compare("../"))
                        name.append("-");
                    directorySet.insert(name);
                } else if (S_ISREG(s.st_mode)) {
                    name += std::to_string(s.st_size);
                    fileSet.insert(name);
                }
            }
        }
        closedir(dh);
    }

    /* Right here the things should be put into the html format */
    for (std::set<std::string>::iterator it = directorySet.begin(); it != directorySet.end(); it++)
        std::cout << *it << std::endl;
    for (std::set<std::string>::iterator it = fileSet.begin(); it != fileSet.end(); it++)
        std::cout << *it << std::endl;

    std::string directoryResponse = directoryListResponse(directorySet, fileSet);
    std::cout << "Building the header for the directory listing:\n" << directoryResponse << std::endl;
    return "placeholder";
}



// anotherlevel/                                      05-May-2022 13:21
/* < ---------- Response header building utils ---------- > */

std::string responseHandler::buildHttpLine(int status) {
    std::map<int, std::string> statusCodes;
    statusCodes[200] = "OK";
    statusCodes[400] = "Bad Request";
    statusCodes[403] = "Forbidden";
    statusCodes[404] = "Not Found";
    statusCodes[405] = "Method Not Allowed";
    statusCodes[505] = "HTTP Version Not Supported";

    std::string response = "HTTP/1.1 ";
    response += std::to_string(status) + " "; // C++11 oops fix this later
    response += statusCodes[status] + "\n";
    response += "Server: Flamenco \033[31m Flame \033[37m Server\n";
    return response;
}

std::string responseHandler::buildDateLine() {
//   std::string s;

    time_t now = time(0);
    char *date = ctime(&now);
    std::string stringDate = date;
    stringDate.insert(3, ",");
    stringDate.resize(stringDate.size() - 1);
    stringDate = "Date: " + stringDate + " GMT\n";
    return stringDate;
}

std::string responseHandler::directoryListResponse(std::set <std::string> &directories,
                                                   std::set <std::string> &files) {
    std::string header = "HTTP/1.1 200 OK\nServer: Flamenco \033[31m Flame \033[37m Server\n";
    header += buildDateLine();
    (void)directories;
    (void)files;




    return header;
}