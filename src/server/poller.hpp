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
#include "clientConnecter.hpp"
#include "serverConfig.hpp"
#include <string>

class poller {

public:

    typedef std::vector<struct pollfd> socketVector;

    poller(std::vector<WSERV::serverConfig>const& serverBlocks);

    ~poller();

    void pollConnections();

    void setPollFd(int fd, short events);

    int newConnection(int fd, int index);

    int connectionError(short revents) const;

    // serverSock		*getSocket() const;

    // int				respondToClient(int sock); // This shit is temporary bollocks
    // void			closeConnection(std::vector<struct pollfd>::iterator it);
private:


    socketVector _sockets;

    /*So I need to loop through every port in this POS and open sockets
    Then later create a map of every host:port combination which I will find whenever
    I parse the request header*/
    std::vector<WSERV::serverConfig> _serverConfigs;

    std::map<int, clientConnecter> _clients;

};

