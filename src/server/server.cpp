/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   server.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/02/04 18:59:58 by tmullan       #+#    #+#                 */
/*   Updated: 2022/04/07 15:44:25 by tmullan       ########   odam.nl         */
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
	ssize_t valread = -1;

	poller.setPollFd(socket_fd, (POLLIN|POLLOUT));
	char buffer[1024] = {0};

	while (true) {
		ret = poll(&(*poller.getConnections().begin()), poller.getConnections().size(), -1); // Could use std::vector::data() but that's c++11
		if (ret < 0) {
			perror("poll");
			break;
		}
		// std::cout << "Well?" << std::endl;
		for (std::vector<struct pollfd>::iterator it = poller.getConnections().begin(); it != poller.getConnections().end(); it++) {
			// std::cout << "Diocane " << std::hex << it->revents << std::endl;
			if (connectionError(it->revents)) {
				std::cout << "Connection was hung up or invalid requested events: " << std::hex << it->revents << std::endl;
				break;
			}
			if (it->revents & POLLIN) {
				std::cout << "Listening socket is readable on fd: " << it->fd << std::endl;
				if (it->fd == socket_fd) {
					newConnection();
					break;
				}
				
				valread = recv(it->fd, buffer, 1024, 0);
				// std::cout << "valread contains: [" << valread << "]" << std::endl;
				if (valread > 0) {
					poller.getRequests()[it->fd].fillBuffer(buffer, valread);
					// std::cout << poller.getRequests()[it->fd].getFd() << std::endl;
					memset(buffer, 0, sizeof(buffer));
				}
				if (!valread) {
					// std::cout << "In here?" << std::endl;
					/* Realistically Don't actually need this here */
					// poller.getRequests()[it->fd].bufferIsFull();
					// poller.getRequests()[it->fd].parseRequest();
					// std::cout << "jesus" << " " << poller.getRequests()[it->fd].getFullState() << std::boolalpha << std::endl;

					/* Leave the connections open for now */
					// std::cout << "Connection ended by client" << std::endl;
					// closeConnection(it);
					break;
				}
				if (valread < 0) {
					std::cout << "No bytes to read" << std::endl;
					perror("What ");
					break;
				}
			}
			if (it->revents & POLLOUT) {

				// std::cout << "Socket is writeable on fd: " << it->fd << std::endl;
				/* This may not always work. Loop may need a wee bit more work
				--- it presumes that we'll never have an incomplete request
				--- by the time poll gives us POLLOUT */
				// poller.getRequests()[it->fd].parseRequest();
				if (!(it->revents & POLLIN) && !poller.getRequests()[it->fd].getFullState()) {
					poller.getRequests()[it->fd].bufferIsFull();
					std::cout << "Parsing the following:\n" << poller.getRequests()[it->fd].getBuffer() << std::endl;
					poller.getRequests()[it->fd].parseRequest();
				}

				// std::cout << "Bro, fd is: " << it->fd << "\nAnd the response is: " << poller.getRequests()[it->fd].getResponse() << std::endl;
				if (poller.getRequests()[it->fd].getFullState()) {
					ret = firstResponse(it->fd);
					if (ret < 0) {
						perror ("   send() failed");
						break;
					}
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

int		serverBoy::firstResponse(int sock_fd) {
	// std::ostringstream file_content;
	// std::ifstream myfile;
	// if (sock_fd % 2)
	// 	myfile.open("pages/other.html");
	// else
	// 	myfile.open("pages/index.html");
	// if (!myfile) {
	// 	std::cout << "ao" << std::endl;
	// }

	// file_content << myfile.rdbuf();
	// std::string content = file_content.str();

	// std::string	header = "HTTP/1.1 200 OK\nContent-Type: text/html; charset=UTF-8\nContent-Length:";
	// int len = file_content.str().size();
	// header.append(std::to_string(len));
	// header.append("\n\n");
	// header.append(content);
	// char *hey = new char[header.length() + 1];
	// std::strcpy(hey, header.c_str());

	// std::cout << "the friggin fd is: " << std::endl;
	std::string response = poller.getRequests()[sock_fd].getResponse();
	if (response.size() == 0) {
		std::cout << "Response not ready yet" << std::endl;
		return 0;
	}
	// std::cout << "Respones gonna be [" << response << "]" << std::endl;
	
	char *hey = new char[response.length() + 1];
	std::strcpy(hey, response.c_str());

	int ret = send(sock_fd, hey, strlen(hey), 0);
	delete[] hey;
	// ret = -1;
	return ret;
}

