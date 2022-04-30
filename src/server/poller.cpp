/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   poller.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/02/04 18:59:58 by tmullan       #+#    #+#                 */
/*   Updated: 2022/04/22 12:37:00 by tmullan       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

//#include "server.hpp"
//#include "serverConfig.hpp"
//#include "clientConnecter.hpp"
#include "poller.hpp"
#include "colours.hpp"
#include "socket.hpp"

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/fcntl.h>
#include <sys/types.h>
#include <netinet/in.h> // inet_addr()
#include <arpa/inet.h> // inet_addr()
#include <utility>

poller::poller(configVector const& configVector) : _serverConfigs(configVector) {}

poller::~poller() {}

void poller::setPollFd(int fd, short events) {
    struct pollfd newPollFd;
    newPollFd.fd = fd;
    newPollFd.events = events;
    _sockets.push_back(newPollFd);
}

int poller::connectionError(short revents) const {
    return revents & (POLLERR | POLLNVAL) ||
           (!(revents & (POLLIN)) && revents & POLLHUP);
};

int poller::newConnection(int fd) {
    socklen_t addrLen;
    int new_fd = accept(fd, (struct sockaddr *) &addrLen, (socklen_t * ) & addrLen);
    if (new_fd < 0) {
        if (errno != EWOULDBLOCK) {
            perror("accept failed");
        }
        return 0;
    }
    int on = 1;
    if ((setsockopt(new_fd, SOL_SOCKET, SO_NOSIGPIPE, &on, sizeof(on)) < 0)) {
        std::cout << "sockoptions got fucked" << std::endl;
        return 0;
    }
    if ((fcntl(new_fd, F_SETFL, O_NONBLOCK)) < 0) {
        std::cout << "fcntl got fucked" << std::endl;
        return 0;
    }
    setPollFd(new_fd, (POLLIN | POLLOUT));

    /* Retrieving the host:port this connection is coming from
     * This could Perhaps even be its own function
    */
    struct sockaddr_in sin;
    socklen_t len = sizeof(sin);
    int port; // This might not work?
    char *host = NULL;

    if (getsockname(new_fd, (struct sockaddr *)&sin, &len))
        perror("getsockname");
    else {
        port = ntohs(sin.sin_port);
        host = inet_ntoa(sin.sin_addr);
        std::cout << "New connection from port: " << port << \
            " on host: " << host << std::endl;
    }

    /*
        Make a vector of only the configs relevant to the host:port combination
        And pass that into the client class
    */
    configVector relevant;
    std::string hostIp = host;
    for (configVector::iterator it = _serverConfigs.begin(); it != _serverConfigs.end(); it++) {
        if (it->get_port() == port && (it->get_host() == hostIp || \
            ((it->get_host()) == "localhost" && !hostIp.compare("127.0.0.1"))))
            relevant.push_back(*it);
    }
    std::cout << "Relevant vectors are:" << std::endl;
    for (configVector::iterator iter = relevant.begin(); iter != relevant.end(); iter++) {
        std::cout << "Port: " << iter->get_port() << " host: " << iter->get_host() << \
            " and server_name: " << iter->get_server_name() << std::endl;
    }


    std::cout << RED << "New accepted client connection: " << new_fd << RESET_COLOUR << std::endl;

    /*
        Here is where I should just create an instance of a client connection that also contains

    */
    client  new_client(host, port, relevant);
    _clients[new_fd] = new_client;

    return 1;
}

std::set<int> poller::openPorts() {
    std::set<std::pair<std::string, int> > ports; // cmd/shift f6 select all instances in project
    std::set<int> listenSockets;
    for (configVector::iterator it = _serverConfigs.begin(); it != _serverConfigs.end(); it++) {
        ports.insert(std::make_pair(it->get_host(), it->get_port()));
        /*It's still here that I should be managing to keep track of the configs as well or is it???*/
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

//std::map<std::pair<std::string, int>, WSERV::serverConfig>  poller::getUniqueConnections() {
//
//}

void poller::pollConnections() {

    std::set<int> portSockets = openPorts();
//    std::map<std::pair<std::string, int>, WSERV::serverConfig> uniqueConnections = getUniqueConnections();
    char buffer[1024] = {0};
    while (true) {
        if (poll(&(*_sockets.begin()), _sockets.size(), -1) < 0) {
            perror("poll");
            break;
        }
        for (socketVector::iterator it = _sockets.begin(); it != _sockets.end(); it++) {
            if (connectionError(it->revents)) {
                std::cout << "Connection Error: " << std::hex << it->revents << std::endl;
                break;
            }
            if (it->revents & POLLIN) {
                if (portSockets.count(it->fd)) { // This should check that it's one of the listening sockets
                    newConnection(it->fd);
                    break;
                }
                std::cout << "Listening socket is readable on fd: " << it->fd << std::endl;
                size_t valRead = recv(it->fd, buffer, 1024, 0);
                if (valRead) {
                    // I should start filling in a client connecter here
                    // Should probably have already created the map<host:port, *config>
                    std::cout << "Received message\n" << buffer << std::endl;
                }
                if (!valRead) {
                    // Do nothing for now - Maybe close connection if required.
                }
                if (valRead < 0) {
                    std::cout << "No bytes to read" << std::endl;
                    perror("What ");
                    break;
                }
            }
            else if (it->revents & POLLOUT) {
                // You _should_ know the drill
            }
        }
    }

    /*
        while true
        poll through all these connections
        while loop through the vector of pollfds structs
            if the connection matches one of those in the server _serverConfigs
                accept the connection and save the serverconfig to it. - But how?
            if POLLIN
                stuff
            if POLLOUT
                you know it

    When accepting a new connection create a Client class and save to it the index of the serverConfigs
    Contents of the Client class? Pass it the appropriate serverConfig and the fd to read/write to.
    Maybe keep a map of Client classes in the poller and then can keep the polling loop similar in functioning
     */


//    int ret;
//    ssize_t valRead = -1;
//
//    poller.setPollFd(listening_socket, (POLLIN | POLLOUT));
//    char buffer[999] = {0};
//
//    while (true) {
//        ret = poll(&(*poller.getConnections().begin()), poller.getConnections().size(),
//                   -1); // Could use std::vector::data() but that's c++11
//        if (ret < 0) {
//            perror("poll");
//            break;
//        }
//        // std::cout << "Well? " << poller.getConnections().size() << std::endl;
//        for (std::vector<struct pollfd>::iterator it = poller.getConnections().begin();
//             it != poller.getConnections().end(); it++) {
//            // std::cout << "Diocane " << std::hex << it->revents << std::endl;
//            if (connectionError(it->revents)) {
//                std::cout << "Connection was hung up or invalid requested events: " << std::hex << it->revents
//                          << std::endl;
//                break;
//            }
//            if (it->revents & POLLIN) {
//                // std::cout << "Listening socket is readable on fd: " << it->fd << std::endl;
//                if (it->fd == listening_socket) {
//                    newConnection();
//                    break;
//                }
//
//                valRead = recv(it->fd, buffer, 999, 0);
//                // std::cout << "valRead contains: [" << valRead << "]" << std::endl;
//                if (valRead > 0) {
//                    poller.getRequests()[it->fd].fillBuffer(buffer, valRead);
//                    // std::cout << poller.getRequests()[it->fd].getFd() << std::endl;
//                    // std::cout << "From fd: " << it->fd << "\n" << buffer << std::endl;
//                    // if (!poller.getRequests()[it->fd].getFullState())
//                    poller.getRequests()[it->fd].parseRequest(); // Why is this empty on the second call?
//                    memset(buffer, 0, sizeof(buffer));
//                }
//                if (!valRead) {
//
//                    /* Leave the connections open for now */
//                    // std::cout << "Connection ended by client" << std::endl;
//                    // closeConnection(it);
//                    continue;
//                }
//                if (valRead < 0) {
//                    std::cout << "No bytes to read" << std::endl;
//                    perror("What ");
//                    break;
//                }
//            }
//            if (it->revents & POLLOUT) {
//
//                if (poller.getRequests()[it->fd].getFullState()) { // Change this to something more readable
//                    ret = respondToClient(it->fd);
//                    if (ret < 0) {
//                        perror("   send() failed");
//                        break;
//                    }
//                    poller.getRequests()[it->fd].resetHandler();
//                }
//            }
//        } // End of loop through pollable connections
//    }
}