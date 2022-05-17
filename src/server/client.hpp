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

    /* Reset client */
    void resetClient();


public:
    enum methodTypes {
        EMPTY, GET, POST, DELETE
    };

private:
    client();

    /* Variables needed for config routing */
    configVector _configs;

    /* Buffer variables */
    char _buffer[1024]; // How am I gonna do that

    int _buffSize;
    bool _isBuffFull;

    /* Host:port and Socket */
    std::string _hostIp;
    int _port;
    int _socket;

    /* Request line info */
    int _method;
    std::string _uri;
    std::string _http;

    /* Request Header Fields */
    std::string _requestedHost;
//    bool _keepAlive;
//
    /* Request Line and Status Info */
    std::string _requestLine;
    int _status;
//    std::string _name;
//
////    bool _autoIndex;
////    set<int> _acceptedMethods;
//
//    /* Final response string */
    std::string _response;
};


#endif //WEBSERV_CLIENT_HPP
