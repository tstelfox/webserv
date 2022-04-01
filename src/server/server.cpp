/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   server.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/02/04 18:59:58 by tmullan       #+#    #+#                 */
/*   Updated: 2022/04/01 16:18:01 by tmullan       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"
#include "clientConnecter.hpp"
#include "responseHandler.hpp"

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

	int socket_fd = _socket->getSock();
	int ret;


	poller.setPollFd(socket_fd, (POLLIN|POLLOUT));
	char buffer[1024] = {0};

	while (true) {
		ret = poll(&(*poller.getConnections().begin()), poller.getConnections().size(), -1); // Could use std::vector::data() but that's c++11
		if (ret < 0) {
			perror("poll");
			break;
		}
		for (std::vector<struct pollfd>::iterator it = poller.getConnections().begin(); it != poller.getConnections().end(); it++) {
			if (connectionError(it->revents)) {
				std::cout << "Connection was hung up or invalid requested events: " << std::hex << it->revents << std::endl;
				break;
			}
			if (it->revents & POLLIN) {
				// std::cout << "Listening socket is readable and writeable on fd: " << it->fd << std::endl;
				if (it->fd == socket_fd) {
					newConnection();
					break;
				}
				
				// Create a new Request class in the map container
				// The below shouldn't overwrite any existing requests.
				// When deleting a connection I should also delete the request though
				// poller.newRequest(it->fd);

				ssize_t valread;
				valread = recv(it->fd, buffer, 1024, 0);
				if (valread > 0) {
					poller.getRequests()[it->fd].fillBuffer(buffer, valread);
					memset(buffer, 0, sizeof(buffer));
				}
				/* The full request has been registered ---
					proceed to parse it and construct the response */
				if (valread == 0 && !poller.getRequests()[it->fd].getFullState()) {
					poller.getRequests()[it->fd].bufferIsFull();
					poller.getRequests()[it->fd].parseRequest();

					/* Ok create a response but this might need to also be in a map */
					responseHandler	response(poller.getRequests()[it->fd]);

					/* Leave the connections open for now */
					// std::cout << "Connection closed by client" << std::endl;
					// closeConnection(it);
					break;
				}
				if (valread < 0) {
					std::cout << "No bytes to read" << std::endl;
					break;
				}
			}
			if (it->revents & POLLOUT) {
				/* So I want to make a response class which takes 
					a pointer to the requestHandler of the relevant
					client connection */
				ret = firstResponse(it->fd);
				if (ret < 0) {
					perror ("   send() failed");
					break;
				}
			}
		} // End of loop through pollable connections
	}
}

serverSock*	serverBoy::getSocket() { return _socket; }

int		serverBoy::connectionError(short revents) {
	return revents & (POLLERR|POLLNVAL) ||
			(!(revents & (POLLIN)) && revents & POLLHUP);
}

void	serverBoy::closeConnection(std::vector<struct pollfd>::iterator it) {
	std::cout << "Deleting connection [" << it->fd << "]" << std::endl;
	close(it->fd);
	// Also gotta delete the request class
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
		return ; // Could define these to ERR
	}
	poller.setPollFd(new_fd, (POLLIN|POLLOUT));
}

int		serverBoy::firstResponse(int sock_fd) {
	std::ostringstream file_content;
	std::ifstream myfile;
	if (sock_fd % 2)
		myfile.open("pages/other.html");
	else
		myfile.open("pages/index.html");
	if (!myfile) {
		std::cout << "ao" << std::endl;
	}

	file_content << myfile.rdbuf();
	std::string content = file_content.str();

	std::string	header = "HTTP/1.1 200 OK\nContent-Type: text/html; charset=UTF-8\nContent-Length:";
	int len = file_content.str().size();
	header.append(std::to_string(len));
	header.append("\n\n");
	header.append(content);
	char *hey = new char[header.length() + 1];
	std::strcpy(hey, header.c_str());

	int ret = send(sock_fd, hey, strlen(hey), 0);
	delete[] hey;
	return ret;
}

