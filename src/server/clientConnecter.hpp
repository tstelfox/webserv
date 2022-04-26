/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   clientConnecter.hpp                               :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/03/16 17:24:04 by tmullan       #+#    #+#                 */
/*   Updated: 2022/03/16 19:04:58 by tmullan       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <vector>
#include <map>
#include "sys/poll.h"
#include "src/requestHandler/requestHandler.hpp"

class clientConnecter {
public:
    clientConnecter();

    ~clientConnecter();

    void setPollFd(int fd, short events);

    void newRequest(int fd); //Take as params anything needed for requestHandler class

    std::vector<struct pollfd> &getConnections();

    std::map<int, requestHandler> &getRequests();

private:
    // Vector of the poll structs and events
    std::vector<struct pollfd> _connections;
    // Map of the requestHandlers key-bound by the connection fd
    std::map<int, requestHandler> _requests;

};

