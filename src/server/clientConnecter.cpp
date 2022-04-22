/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   clientConnecter.cpp                               :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/03/16 17:24:25 by tmullan       #+#    #+#                 */
/*   Updated: 2022/03/16 19:01:04 by tmullan       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

// #include "clientConnecter.hpp"
#include "clientConnecter.hpp"
#include <iostream>

clientConnecter::clientConnecter() {}

clientConnecter::~clientConnecter() {}

void clientConnecter::setPollFd(int fd, short events) {
    struct pollfd newPollFd;
    newPollFd.fd = fd;
    newPollFd.events = events;
    _connections.push_back(newPollFd);
}

void clientConnecter::newRequest(int fd) {
    requestHandler new_request;
    _requests.insert(std::make_pair(fd, new_request));
}

std::map<int, requestHandler> &clientConnecter::getRequests() { return _requests; }

std::vector<struct pollfd> &clientConnecter::getConnections() { return _connections; }
