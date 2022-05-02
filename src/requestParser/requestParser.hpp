/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   requestParser.hpp                                  :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/01 18:30:43 by tmullan       #+#    #+#                 */
/*   Updated: 2022/05/01 18:30:43 by tmullan       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_REQUESTPARSER_HPP
#define WEBSERV_REQUESTPARSER_HPP

#include <string>
#include "serverConfig.hpp"


class requestParser {
public:
    typedef std::vector<WSERV::serverConfig> configVector;

    requestParser(WSERV::serverConfig const &configs, std::map<std::string, std::string> &request);

    ~requestParser();

private:
    requestParser();

    /* Configs to sort through OR could do this in client and just
     * send the right config straight to the parser
     */
    WSERV::serverConfig _config;

    std::map<std::string, std::string> _requestFields;


    /* Ultimate response to return */
    std::string _response;

};


#endif //WEBSERV_REQUESTPARSER_HPP
