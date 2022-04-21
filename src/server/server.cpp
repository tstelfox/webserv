/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   server.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/02/04 18:59:58 by tmullan       #+#    #+#                 */
/*   Updated: 2022/04/21 17:06:22 by tmullan       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"
#include "clientConnecter.hpp"

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/fcntl.h>

serverBoy::serverBoy(serverSock &sock) : _socket(&sock) {}

serverBoy::~serverBoy() {}

void	serverBoy::runServer() {

	int listening_socket = _socket->getSock(); // Should create an openPort class
	int ret;
	ssize_t valread = -1;

	poller.setPollFd(listening_socket, (POLLIN|POLLOUT));
	char buffer[999] = {0};

	while (true) {
		ret = poll(&(*poller.getConnections().begin()), poller.getConnections().size(), -1); // Could use std::vector::data() but that's c++11
		if (ret < 0) {
			perror("poll");
			break;
		}
		// std::cout << "Well? " << poller.getConnections().size() << std::endl;
		for (std::vector<struct pollfd>::iterator it = poller.getConnections().begin(); it != poller.getConnections().end(); it++) {
			// std::cout << "Diocane " << std::hex << it->revents << std::endl;
			if (connectionError(it->revents)) {
				std::cout << "Connection was hung up or invalid requested events: " << std::hex << it->revents << std::endl;
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
						perror ("   send() failed");
						break;
					}
					poller.getRequests()[it->fd].resetHandler();
				}
			}
		} // End of loop through pollable connections
	}
}

serverSock*	serverBoy::getSocket() const { return _socket; }

int		serverBoy::connectionError(short revents) {
	return revents & (POLLERR|POLLNVAL) ||
			(!(revents & (POLLIN)) && revents & POLLHUP);
}

void	serverBoy::closeConnection(std::vector<struct pollfd>::iterator it) {
	std::cout << "Deleting connection [" << it->fd << "]" << std::endl;
	close(it->fd);
	// Also gotta delete the request class
	poller.getRequests().erase(it->fd);
	poller.getConnections().erase(it);
}

void		serverBoy::newConnection() {
	socklen_t	addrlen;
	int new_fd = accept(_socket->getSock(), (struct sockaddr *)&_socket->getAddr(), (socklen_t *)&addrlen);
	if (new_fd < 0) {
		if (errno != EWOULDBLOCK) {
			perror("accept failed");
		}
		return ;
	}
	int on = 1;
	if ((setsockopt(new_fd, SOL_SOCKET, SO_NOSIGPIPE, &on, sizeof(on)) < 0)) {
		std::cout << "sockoptions got fucked" << std::endl;
		return ;
	}
	if ((fcntl(new_fd, F_SETFL, O_NONBLOCK)) < 0) {
		std::cout << "fcntl got fucked" << std::endl;
		return ;
	}
	poller.setPollFd(new_fd, (POLLIN|POLLOUT));

	// Create a new Request class in the map container
	poller.newRequest(new_fd);
}

int		serverBoy::respondToClient(int sock_fd) {

	std::string response = poller.getRequests()[sock_fd].getResponse();

	char *hey = new char[response.length() + 1];
	std::strcpy(hey, response.c_str());

	int ret = send(sock_fd, hey, strlen(hey), 0);
	delete[] hey;
	return ret;
}

