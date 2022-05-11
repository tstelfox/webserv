/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   server.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/02/04 18:38:07 by tmullan       #+#    #+#                 */
/*   Updated: 2022/04/28 18:37:20 by ubuntu        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../sockets/socket.hpp"
#include "clientConnecter.hpp"
#include <string>

/* Fixing the issues of the following older comment */
/* The structure is incorrect.
    - Polling cannot occur within the server object
        because if there are multiple servers there must be a single poll()
        to handle all of the listening sockets on the various ports.
    - A poller general function will probably have to be called and have
        a vector or map of servers passed to it for it to loop through the
        listening socket for each of them. */

class serverBoy {

public:
    serverBoy(serverSock &sock);
    // serverBoy(std::vector<openPort>	serverBlocks);

    ~serverBoy();

    void runServer();

    serverSock *getSocket() const;

    int respondToClient(int sock); // This shit is temporary bollocks
    int connectionError(short revents);

    void newConnection();

    void closeConnection(std::vector<struct pollfd>::iterator it);

private:
    serverBoy();

    serverSock *_socket;
    clientConnecter poller;

    // This is how we tie this shit in
    // std::vector<openPort>	_openPorts;
};

