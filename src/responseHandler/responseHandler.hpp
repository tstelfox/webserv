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

    /* Match location */
    int matchLocation(std::string uri);

    /* Error Responses */
    std::string respondError(int status);

    std::string extractErrorFile(int status);

    /* GET request*/
    std::string getResponse(std::string const& uri);

    /* POST request */
    std::string postResponse(std::string const& uri);

    /* DELETE request */
    std::string deleteResponse(std::string uri);

    /* Redirection Response */
    std::string redirectionResponse(std::string redirectionUri);

    /* Response header building utils */
    std::string buildHttpLine(int status);

    std::string buildDateLine();

    /* Create html for Directory listing */
    std::string buildDirectoryListing(std::string &directory);

    std::string directoryListResponse(std::set <std::vector<std::string> > &directories,
                                      std::set <std::vector<std::string> > &files, std::string directory);


    /* General utils */
    bool    isDirectory(std::string const& path);
    std::string rootResolution(std::string const& uri);

    /* Should probably send in the request line tbh */
    std::string _requestLine;
    WSERV::serverConfig _config;
    std::map <std::string, std::string> _requestFields;

    /* Correct Location */
    WSERV::Location _location;
    /* Status code */
//    int _status;

    /* Ultimate response to return */
    std::string _finalResponse;

};


#endif //WEBSERV_RESPONSEHANDLER_HPP
