/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   client.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/01 15:35:37 by tmullan       #+#    #+#                 */
/*   Updated: 2022/05/01 15:35:37 by tmullan       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_CLIENT_HPP
#define WEBSERV_CLIENT_HPP

#include "serverConfig.hpp"


class client {
public:

    typedef std::vector <WSERV::serverConfig> configVector;

    client(std::string hostIp, int port, configVector const &configs,
           int socket);

    ~client();

    /* Storing incoming client request */
    void fillBuffer(const char *buff, ssize_t valRead);
    int fullHeaderReceived();
    bool isBufferFull() const;

    /* Parsing Request Header and Config Routing */
    void parseRequestLine(std::string request);
    void requestedHost(std::map<std::string, std::string> &fields);
    void parseRequestHeader();

    void routeConfig(std::map<std::string, std::string> &fields);

    /* Getters */
    char *getBuffer();
    std::string getResponse() const;

//    void resetClient();




public:
    enum methodTypes {
        EMPTY, GET, POST, DELETE
    };

private:
    client();

    /* Initially wanted to put configs in a map but nah
     * Actually a map would be a bad idea cause I have to use the
     * first in the case there is no server_name match*/

    /* Variables needed for config routing */
    configVector _configs;

    /* Buffer variables */
    char _buffer[1024]; // Maybe max client size can be enforced here?

    int _buffSize;
    bool _isBuffFull;
    /* Perhaps client should contain a requestHandler */


    /* Following things may be needed but am not 100% sure */
    std::string _hostIp;
    int _port;
    int _socket;


//    /* Request line info */
    int _method;
    std::string _uri;
    std::string _http;
//
//    /* Request Header Fields */
    std::string _requestedHost;
//    bool _keepAlive;
//
//    /* Request Line and Status Info */
    std::string _requestLine;
    int _status;
//    std::string _name;
//
//    /* Truly should consider making a request class TODO*/
////    bool _autoIndex;
////    set<int> _acceptedMethods;
//
//    /* Final response string */
    std::string _response;
};


#endif //WEBSERV_CLIENT_HPP
