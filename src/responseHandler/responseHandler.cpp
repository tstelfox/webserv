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
    switch(method) {
        case 1:
            std::cout << "GET request" << std::endl;
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
    std::cout << "GET whatever is at " << uri << std::endl;
    /*Compare the uri against the locations
    in order to find the correct details*/

    /*Check that the method in question is an allowed method*/

//    if (!uri.compare("/")) {
//    }
    return uri;
}

std::string responseHandler::respondError(int status) {
    std::map<int, std::string> statusCodes;
//    statusCodes[200] = "OK";
    statusCodes[400] = "Bad Request";
    statusCodes[404] = "Not Found";
    statusCodes[405] = "Method Not Allowed";
    statusCodes[505] = "HTTP Version Not Supported";

    std::string response = "HTTP/1.1 ";
    response += std::to_string(status) + " "; // C++11 oops fix this later
    response += statusCodes[status] + "\n";

    // Server name
    response += "Server: Flamenco \033[31m Flame \033[37m Server\n";
    // Date
    time_t now = time(0);
    char *date = ctime(&now);
    std::string stringDate = date;
    stringDate.insert(3, ",");
    stringDate.resize(stringDate.size() - 1);
    response += "Date: " + stringDate + " GMT\n";

    //Content
    response += "Content-type: text/html; charset=UTF-8\nContent-Length:";
    // Extract file
    std::string body = extractErrorFile(status);
    response.append(std::to_string(body.size()));
    //Error files
    response += "\nConnection: close";

    //End of Header
    response.append("\n\n");
    std::cout << RED << "<<<<-------- The response header ------->>>>\n" << RESET_COLOUR << response << std::endl;
    response.append(body + "\n");
    return response;
}

std::string responseHandler::extractErrorFile(int status) {
    std::ifstream errFile;
    std::string path = _config.get_error_page();
    path += std::to_string(status) + ".html";
//    std::cout << "File path: " << path << std::endl;
    errFile.open(path);
    if (errFile.fail()) {
        // panic
    }
    std::ostringstream fileContent;
    fileContent << errFile.rdbuf();
    return fileContent.str();
}
