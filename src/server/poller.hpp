/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   poller.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/02/04 18:38:07 by tmullan       #+#    #+#                 */
/*   Updated: 2022/04/22 13:38:54 by tmullan       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "socket.hpp"
#include "client.hpp"
#include "serverConfig.hpp"
#include <string>
#include <set>

class poller {

public:

    typedef std::vector<struct pollfd> socketVector;
    typedef std::vector<WSERV::serverConfig> configVector;

    poller(configVector const &serverBlocks);

    ~poller();

    void setPollFd(int fd, short events);

    int connectionError(short revents) const;

    int newConnection(int fd);

    std::set<int> openPorts();

    void pollConnections();

    // serverSock		*getSocket() const;

    // int				respondToClient(int sock); // This shit is temporary bollocks
    // void			closeConnection(std::vector<struct pollfd>::iterator it);
private:

    /* Vector of the poll structs and events */
    socketVector _sockets;

    /* Create a map of every host:port combination which I will find whenever
    I parse the request header */

   /* Vector of all of the server configurations */
    configVector _serverConfigs;

    /* Map of all the clients sorted by socket */
    std::map<int, client> _clients;

};

