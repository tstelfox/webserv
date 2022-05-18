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
    int fullHeaderReceived(const char *buff);
    bool isBufferFull() const;

    /* Parsing Request Header and Config Routing */
    void parseRequestLine(std::string request);
    void requestedHost(std::map<std::string, std::string> &fields);
    void parseRequestHeader();

    void routeConfig(std::map<std::string, std::string> &fields);

    /* Getters */
    std::string getBuffer() const;
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
    std::string _buffer;
    bool _bodyPresent;
    bool _isBuffFull;
    size_t _bodySize;


    /* Body */
    std::string _body;

    /* Host:port and Socket */
    std::string _hostIp;
    int _port;
    int _socket;

    /* Request line info */
    int _method;
    int _status;
    std::string _uri;
    std::string _http;
    std::string _requestLine;

    /* Request Header Fields */
    std::string _requestedHost;
//    bool _keepAlive;

    /* Final response string */
    std::string _response;
};


#endif //WEBSERV_CLIENT_HPP
