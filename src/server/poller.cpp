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

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/fcntl.h>

poller::poller(std::vector <WSERV::serverConfig>const& configVector) : _serverConfigs(configVector) {
    // Here I have to create that set of ports.
}

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

int poller::newConnection(int fd, int index) {
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
    // Create a new Request class that takes the server configs at the index in the map container
    (void)index;
    std::cout << RED << "New accepted client connection: " << new_fd << RESET_COLOUR << std::endl;
    return 1;
}

void poller::pollConnections() {
    // Set up the sockets for each port
//    std::map<int, int> indexToConfig;
//    for (size_t i = 0; i < _serverConfigs.size(); i++) {
//        // Create the pollfd structs and then push them into the vector
//        int newSocket = _serverConfigs[i].getSockFd();
//        indexToConfig[newSocket] = i;
//        setPollFd(newSocket, (POLLIN | POLLOUT));
//    }
////    char buffer[1024] = {0};
//    while (true) {
//        if (poll(&(*_sockets.begin()), _sockets.size(), -1) < 0) {
//            perror("poll");
//            break;
//        }
//        for (socketVector::iterator it = _sockets.begin(); it != _sockets.end(); it++) {
//            if (connectionError(it->revents)) {
//                std::cout << "Connection Error: " << std::hex << it->revents << std::endl;
//                break;
//            }
//            if (it->revents & POLLIN) {
//                 std::cout << "Listening socket is readable on fd: " << it->fd << std::endl;
//                if (indexToConfig.count(it->fd)) { // This should check that it's one of the listening sockets
//                    newConnection(it->fd, indexToConfig[it->fd]);
//                    break;
//                }
//                // Otherwise the actual reading
//            }
//        }
//    }

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
//    ssize_t valread = -1;
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
//                valread = recv(it->fd, buffer, 999, 0);
//                // std::cout << "valread contains: [" << valread << "]" << std::endl;
//                if (valread > 0) {
//                    poller.getRequests()[it->fd].fillBuffer(buffer, valread);
//                    // std::cout << poller.getRequests()[it->fd].getFd() << std::endl;
//                    // std::cout << "From fd: " << it->fd << "\n" << buffer << std::endl;
//                    // if (!poller.getRequests()[it->fd].getFullState())
//                    poller.getRequests()[it->fd].parseRequest(); // Why is this empty on the second call?
//                    memset(buffer, 0, sizeof(buffer));
//                }
//                if (!valread) {
//
//                    /* Leave the connections open for now */
//                    // std::cout << "Connection ended by client" << std::endl;
//                    // closeConnection(it);
//                    continue;
//                }
//                if (valread < 0) {
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