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
#include <string>

class poller {

public:

    typedef std::vector<struct pollfd> socketVector;

    poller(std::vector<server> serverBlocks);

    ~poller();

    void pollConnections();

    int newConnection(int index);

    int connectionError(short revents);

    // serverSock		*getSocket() const;

    // int				respondToClient(int sock); // This shit is temporary bollocks
    // void			closeConnection(std::vector<struct pollfd>::iterator it);
private:

    // This is how we tie this shit in
    socketVector _sockets;
    std::vector<serverBlock> _serverConfigs; // Should have a socket in each

    std::map<int, clientConnecter> _clients;

};

