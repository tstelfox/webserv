//
// Created by Turlough stelfox Mullan on 4/30/22.
//

#ifndef WEBSERV_CLIENT_HPP
#define WEBSERV_CLIENT_HPP


class client {
public:

    typedef std::vector <WSERV::serverConfig> configVector;

    client();

    ~client();

public:
    enum methodTypes {
        EMPTY, GET, POST, DELETE
    };

private:
    /* Variables needed for config routing */
    configVector _configs;
    std::string _hostIp;
    int _port;

    /* Buffer variables */
    char _buffer[1024]; // Maybe max client size can be enforced here?
    int _buffSize;
    bool _isBuffFull?

    /* Request line info */
    int _method;
    std::string _uri;
    std::string _http;

    /* Request Header Fields */
    std::string _requestedServer;
    bool        _keepAlive;

    /* Status Code and response info */
    int _status;
    std::string _name; // What is this?

    /* Final response string */
    std::string _response;
};


#endif //WEBSERV_CLIENT_HPP
