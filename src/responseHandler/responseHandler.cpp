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


responseHandler::responseHandler(WSERV::serverConfig const &configs, std::map<std::string, std::string> &fields)
: _config(configs), _requestFields(fields) {
    std::cout << ITALIC << COLOR_NEON << "Brr I find out what to respond now" << FORMAT_RESET << RESET_COLOUR << std::endl;
}

responseHandler::responseHandler() {}

responseHandler::~responseHandler() {}

std::string responseHandler::parseAndRespond(int status) {
    if (status != 200)
        return respondError(status);

    // Or the normal sheet
    return "Placeholder"; // TODO
}

std::string responseHandler::respondError(int status) {
    std::map<int, std::string> statusCodes;
//    statusCodes[200] = "OK";
    statusCodes[400] = "Bad Request";
    statusCodes[404] = "Not Found";
    statusCodes[405] = "Method Not Allowed";
    statusCodes[505] = "HTTP Version Not Supported";
//    std::cout << STATUSCODES[400] << std::endl;

    std::string header = "HTTP/1.1 ";
    header += std::to_string(status) + " "; // C++ 11 oops fix this later
    header += statusCodes[status] + "\n";

    // Server name
    header += "Server: Flamenco \033[31m Flame \033[37m Server\n";
    // Date
    time_t now = time(0);
    char *date = ctime(&now);
    std::string stringDate = date;
    stringDate.insert(3, ",");
    stringDate.resize(stringDate.size() - 1);
    header += "Date: " + stringDate + " GMT\n";

    //Content
    header += "Content-type: text/html; charset=UTF-8\nContent-Length:";
    // Extract file
//    std::cout << "Error pages directory: " << _config.get_error_page() << std::endl;
//    std::cout << "So far we have: " << header << std::endl;

//    std::string response;




    return header; // Placeholder
}

