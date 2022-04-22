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

#include "server.hpp"
#include "serverBlock.hpp"
#include "clientConnecter.hpp"

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/fcntl.h>

poller::poller(std::vector<serverBlock> serverBlocks) : _serverConfigs(serverBlocks) {}

poller::~poller() {}

void poller::setPollFd(int fd, short events) {
    struct pollfd newPollFd;
    newPollFd.fd = fd;
    newPollFd.events = events;
    _sockets.push_back(newPollFd);
}

void poller::pollConnections() {
    // Set up the sockets for each port
    for (int i = 0; i < _serverConfigs.size(); i++) {
        // Create the pollfd structs and then push them into the vector
        int newSocket = _serverConfigs[i].getSocket();
        setPollFd(newSocket, (POLLIN | POLLOUT));
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







    int listening_socket = _socket->getSock(); // Should create an openPort class
    int ret;
    ssize_t valread = -1;

    poller.setPollFd(listening_socket, (POLLIN | POLLOUT));
    char buffer[999] = {0};

    while (true) {
        ret = poll(&(*poller.getConnections().begin()), poller.getConnections().size(),
                   -1); // Could use std::vector::data() but that's c++11
        if (ret < 0) {
            perror("poll");
            break;
        }
        // std::cout << "Well? " << poller.getConnections().size() << std::endl;
        for (std::vector<struct pollfd>::iterator it = poller.getConnections().begin();
             it != poller.getConnections().end(); it++) {
            // std::cout << "Diocane " << std::hex << it->revents << std::endl;
            if (connectionError(it->revents)) {
                std::cout << "Connection was hung up or invalid requested events: " << std::hex << it->revents
                          << std::endl;
                break;
            }
            if (it->revents & POLLIN) {
                // std::cout << "Listening socket is readable on fd: " << it->fd << std::endl;
                if (it->fd == listening_socket) {
                    newConnection();
                    break;
                }

                valread = recv(it->fd, buffer, 999, 0);
                // std::cout << "valread contains: [" << valread << "]" << std::endl;
                if (valread > 0) {
                    poller.getRequests()[it->fd].fillBuffer(buffer, valread);
                    // std::cout << poller.getRequests()[it->fd].getFd() << std::endl;
                    // std::cout << "From fd: " << it->fd << "\n" << buffer << std::endl;
                    // if (!poller.getRequests()[it->fd].getFullState())
                    poller.getRequests()[it->fd].parseRequest(); // Why is this empty on the second call?
                    memset(buffer, 0, sizeof(buffer));
                }
                if (!valread) {

                    /* Leave the connections open for now */
                    // std::cout << "Connection ended by client" << std::endl;
                    // closeConnection(it);
                    continue;
                }
                if (valread < 0) {
                    std::cout << "No bytes to read" << std::endl;
                    perror("What ");
                    break;
                }
            }
            if (it->revents & POLLOUT) {

                if (poller.getRequests()[it->fd].getFullState()) { // Change this to something more readable
                    ret = respondToClient(it->fd);
                    if (ret < 0) {
                        perror("   send() failed");
                        break;
                    }
                    poller.getRequests()[it->fd].resetHandler();
                }
            }
        } // End of loop through pollable connections
    }
}