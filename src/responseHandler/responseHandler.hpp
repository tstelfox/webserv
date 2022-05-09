/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   responseHandler.hpp                                  :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/01 18:30:43 by tmullan       #+#    #+#                 */
/*   Updated: 2022/05/01 18:30:43 by tmullan       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_RESPONSEHANDLER_HPP
#define WEBSERV_RESPONSEHANDLER_HPP

#include <string>
#include "serverConfig.hpp"


class responseHandler {
public:

    responseHandler(std::string requestLine, WSERV::serverConfig const &configs,
                    std::map <std::string, std::string> &request);

    ~responseHandler();

    std::string parseAndRespond(int status, int method, std::string uri);


private:
    responseHandler();

    /* Error Responses */
    std::string respondError(int status);

    std::string extractErrorFile(int status);

    /* GET request*/
    std::string getResponse(std::string uri);

    /* POST request */
    std::string getPost(std::string uri);

    /* Response header building utils */
    std::string buildHttpLine(int status);

    std::string buildDateLine();

    /* Create html for Directory listing */
    std::string buildDirectoryListing(std::string &directory, std::string uri);

    std::string directoryListResponse(std::set <std::vector<std::string> > &directories,
                                      std::set <std::vector<std::string> > &files, std::string uri);

    /* Should probably send in the request line tbh */
    std::string _requestLine;
    WSERV::serverConfig _config;
    std::map <std::string, std::string> _requestFields;


    /* Status code */
//    int _status;

    /* Ultimate response to return */
    std::string _finalResponse;

};


#endif //WEBSERV_RESPONSEHANDLER_HPP
