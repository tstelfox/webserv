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

    responseHandler(WSERV::serverConfig const &configs, std::map<std::string, std::string> &request);

    ~responseHandler();

    std::string parseAndRespond(int status);


private:
    responseHandler();

    std::string respondError(int status);

    /* Configs to sort through OR could do this in client and just
     * send the right config straight to the parser
     */
    WSERV::serverConfig _config;

    std::map<std::string, std::string> _requestFields;


    /* Status code */
//    int _status;

    /* Ultimate response to return */
    std::string _finalResponse;

};


#endif //WEBSERV_RESPONSEHANDLER_HPP
