/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   poller.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/02/04 18:59:58 by tmullan       #+#    #+#                 */
/*   Updated: 2022/05/16 14:27:57 by akramp        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

//#include "server.hpp"
//#include "serverConfig.hpp"
#include "client.hpp"
#include "poller.hpp"
#include "colours.hpp"
#include "socket.hpp"
#include <signal.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/fcntl.h>
#include <sys/types.h>
#include "sys/poll.h"
#include <netinet/in.h> // inet_addr()
#include <arpa/inet.h> // inet_addr()
#include <utility>

poller::poller(configVector const& configVector) : _serverConfigs(configVector) {}

poller::~poller() {}

socketVector::iterator poller::setPollFd(int fd, short events) {
    struct pollfd newPollFd;
    newPollFd.fd = fd;
    newPollFd.events = events;
    _sockets.push_back(newPollFd);
}

int poller::connectionError(short revents) const {
    return revents & (POLLERR | POLLNVAL) ||
           (!(revents & (POLLIN)) && revents & POLLHUP);
};

void poller::deleteConnection(int fd) {
    std::map<int, client>::iterator clientIt = _clients.find(fd);
//    std::cout << "CLient size before: " << _clients.size() << std::endl;
//    _client.erase()
    if (clientIt != _clients.end()) {
        _clients.erase(clientIt);
        close(fd);
    }
//    std::cout << "CLient size after: " << _clients.size() << std::endl;
}

int poller::newConnection(int fd) {
    /* TODO cleanup and split this function after cgi */
    socklen_t addrLen;
    struct sockaddr_in addr;
    bzero(&addr, sizeof(struct sockaddr_in));
    std::cout << RED << "fd: " << fd << RESET_COLOUR << std::endl;
    int newConnection = accept(fd, (struct sockaddr *) &addr, (socklen_t * ) &addrLen);

    if (newConnection < 0) {
        if (errno != EWOULDBLOCK) {
            perror("accept failed");
        }
        return 0;
    }
    int on = 1;
//    if ((setsockopt(newConnection, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int)) < 0)) {
    if ((setsockopt(newConnection, SOL_SOCKET, SO_NOSIGPIPE, &on, sizeof(int)) < 0)) {
        std::cout << "sockoptions got fucked" << std::endl;
        return 0;
    }
    if ((fcntl(newConnection, F_SETFL, O_NONBLOCK)) < 0) {
        std::cout << "fcntl got fucked" << std::endl;
        return 0;
    }
    setPollFd(newConnection, (POLLIN | POLLOUT));
    /* Retrieving the host:port this connection is coming from
     * This could Perhaps even be its own function
    */
    struct sockaddr_in sin;
    socklen_t len = sizeof(sin);
    int port;
    char *host = NULL;

    if (getsockname(newConnection, (struct sockaddr *)&sin, &len))
        perror("getsockname");
    else {
        port = ntohs(sin.sin_port);
        host = inet_ntoa(sin.sin_addr);
        std::cout << "New connection from port: " << port << \
            " on host: " << host << std::endl;
    }
    /*
        Make a vector of only the configs relevant to the host:port combination
        And pass that into the client class for server block routing
    */
    configVector relevant;
    std::string hostIp = host;
    for (configVector::iterator it = _serverConfigs.begin(); it != _serverConfigs.end(); it++) {
        if (it->get_port() == port && (it->get_host() == hostIp || \
            ((it->get_host()) == "localhost" && !hostIp.compare("127.0.0.1"))))
            relevant.push_back(*it);
    }
//    std::cout << "Relevant vectors are:" << std::endl;
//    for (configVector::iterator iter = relevant.begin(); iter != relevant.end(); iter++) {
//        std::cout << "Port: " << iter->get_port() << " host: " << iter->get_host() << \
//            " and server_name: " << iter->get_server_name() << std::endl;
//    }

    std::cout << RED << "New accepted client connection: " << newConnection << RESET_COLOUR << std::endl;

    client  newClient(hostIp, port, relevant, newConnection);
    _clients.insert(std::make_pair(newConnection, newClient));

    return 1;
}

int poller::newCgiConnection(int fd) {


}

std::set<int> poller::openPorts() {
    std::set<std::pair<std::string, int> > ports; // cmd/shift f6 select all instances in project
    std::set<int> listenSockets;
    for (configVector::iterator it = _serverConfigs.begin(); it != _serverConfigs.end(); it++) {
        ports.insert(std::make_pair(it->get_host(), it->get_port()));
    }
    for (std::set<std::pair<std::string, int> >::iterator i = ports.begin(); i != ports.end(); i++) {
        std::cout << "Host: " << i->first << " Port: " << i->second << std::endl;
//        serverSock buildSocket(AF_INET, SOCK_STREAM, 0, i->second, INADDR_ANY);
        std::string host;
        if (!i->first.compare("localhost"))
            host = "127.0.0.1";
        else
            host = i->first;
        serverSock buildSocket(AF_INET, SOCK_STREAM, 0, i->second, host.c_str());
        int newSocket = buildSocket.getSock();
        listenSockets.insert(newSocket);
        setPollFd(newSocket, (POLLIN | POLLOUT));
    }
    return listenSockets;
}

int poller::respondToClient(int socket, std::string response) {

    char toSend[response.length() + 1];
    std::strcpy(toSend, response.c_str());

    int ret = send(socket, toSend, strlen(toSend), 0);
    return ret;
}

void poller::pollConnections() {

    std::set<int> portSockets = openPorts();
    char buffer[BUFF_SIZE] = {0};
    while (true) {
        if (poll(&(*_sockets.begin()), _sockets.size(), -1) < 0) {
            perror("poll");
            break;
        }
        for (socketVector::iterator it = _sockets.begin(); it != _sockets.end(); it++) {
            client &currentClient = _clients.find(it->fd)->second;
            if (connectionError(it->revents)) {
                std::cout << "Connection Error: " << std::hex << it->revents << std::endl;
                deleteConnection(it->fd);
                _sockets.erase(it);
                break;
            }
            if (it->revents & POLLIN) {
                if (portSockets.count(it->fd)) { // This checks that it's one of the listening sockets
                    newConnection(it->fd);
                    break;
                }
//                std::cout << "Listening socket is readable on fd: " << it->fd << std::endl;
                int valRead = recv(it->fd, buffer, BUFF_SIZE - 2, 0);
                if (valRead) {
//                    std::cout << CYAN << "The ahhhhhhh: " << buffer << RESET_COLOUR << std::endl;
                    currentClient.fillBuffer(buffer, valRead);
                    memset(buffer, 0, sizeof(buffer));
                }
                if (!valRead) {
                    deleteConnection(it->fd);
                    _sockets.erase(it);
                    break;
//                    std::cout << GREEN << "Nothing more to read" << RESET_COLOUR << std::endl;
                }
                if (valRead < 0) {
                    std::cout << RED << "Error receiving from client" << RESET_COLOUR << std::endl;
                    deleteConnection(it->fd);
                    _sockets.erase(it);
                    break;
                }
            } else if (it->revents & POLLOUT) {
//                std::cout << MAGENTA << "FRIGGIN FRIG" << RESET_COLOUR << std::endl;
                if (currentClient.isBufferFull()) {
                    currentClient.parseRequestHeader();
                    if (currentClient.isCgi()) {
                        std::cout << "diocane che balle but here is the fd: " << currentClient.getCgiFd() << std::endl;

                        socketVector::iterator cgiIt = setPollFd(currentClient.getCgiFd(), POLLIN);
//                        return; // Temporary
                        break;
                    }
                    int valSent = respondToClient(it->fd, currentClient.getResponse());
                    if (valSent) { // TODO this was introduced to fix siege issues but hmmmm
                        deleteConnection(it->fd);
                        _sockets.erase(it);
                        break;
                    }
                    else if (!valSent) {
                        deleteConnection(it->fd);
                        _sockets.erase(it);
                        break;
                        std::cout << "Nothing more to send" << std::endl;
                    }
                    else if (valSent < 0) {
                        std::cout << RED << "Error sending to client" << RESET_COLOUR << std::endl;
                        deleteConnection(it->fd);
                        _sockets.erase(it);
                        break;
                    }
                    currentClient.resetClient();
                }
            }
        }
    }
}
