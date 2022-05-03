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
    in order to find the correct locations configs
    including the root.
    BUT for now just improvise something simple
     */

    std::string requestedFile = _config.get_Location_vec()[0].get_root(); // PLACEHOLDER TODO
    if (!uri.compare("/"))
        uri += "index.html";
    requestedFile += uri;
    std::ifstream myFile;
    myFile.open(requestedFile);
    if (myFile.fail()) {
        return respondError(404);
    }
    std::ostringstream fileContent;
    fileContent << myFile.rdbuf();
    std::string responseBody = fileContent.str();
    myFile.close();
    std::cout << "Diocane: " << requestedFile << std::endl;

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
    // Server name
//    response += "Server: Flamenco \033[31m Flame \033[37m Server\n";
    // Date
//    time_t now = time(0);
//    char *date = ctime(&now);
//    std::string stringDate = date;
//    stringDate.insert(3, ",");
//    stringDate.resize(stringDate.size() - 1);
//    response += "Date: " + stringDate + " GMT\n";

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
    std::cout << "de boia " << path << std::endl;
    path += std::to_string(status) + ".html";
    std::cout << "File path: " << path << std::endl;
    errFile.open(path);
    if (errFile.fail()) {
        // panic
    }
    std::ostringstream fileContent;
    fileContent << errFile.rdbuf();
    return fileContent.str();
}

/* < ---------- Response header building utils ---------- > */

std::string responseHandler::buildHttpLine(int status) {
    std::map<int, std::string> statusCodes;
    statusCodes[200] = "OK";
    statusCodes[400] = "Bad Request";
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
