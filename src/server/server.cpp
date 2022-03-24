/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   server.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/02/04 18:59:58 by tmullan       #+#    #+#                 */
/*   Updated: 2022/03/24 18:21:12 by tmullan       ########   odam.nl         */
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


	/*--------- Should just use a vector for every client connection which contains:
					-socket
					-buffer of shit to read
					-something else?
				And then close it by deleting the item -----------*/ 
void	serverBoy::runServer() {
	
	// clientConnecter		poller;
	int socket_fd = _socket->getSock();
	int new_fd = -1;
	int i;

	int ret;
	std::cout << "Socket fd is: " << socket_fd << std::endl;


	poller.setPollFd(socket_fd, (POLLIN|POLLOUT));


	char buffer[1024] = {0};
	int current_size;

	while (true) {
		current_size = poller.getConnections().size();
		for (i = 0; i < current_size; i++) {
			// std::cout << "<<------Waiting on poll()...------>>" << std::endl;
			// std::cout << "Amount of connections: " << poller.getConnections().size() << std::endl;
			ret = poll(&poller.getConnections()[0], poller.getConnections().size(), -1); // Could use std::vector::data() but that's c++11
			if (ret < 0) {
				perror("poll");
				break;
			}
			if (ret == 0) {
				std::cout << "Poll timed out. End" << std::endl;
				break;
			}
			if (poller.getConnections()[i].revents == 0) {
				continue;
			}
			if (connectionErr(poller.getConnections()[i].revents)) {
				std::cout << "Connection was hung up or invalid requested events: " << std::hex << poller.getConnections()[i].revents << std::endl;
				break;
			}
			if (poller.getConnections()[i].revents & POLLIN) {
				std::cout << "Listening socket is readable and writeable on fd: " << new_fd << std::endl;
				if (poller.getConnections()[i].fd == socket_fd) { // So try to move this into POLLIN
					socklen_t	addrlen;
					new_fd = accept(socket_fd, (struct sockaddr *)&_socket->getAddr(), (socklen_t *)&addrlen);
					if (new_fd < 0) {
						if (errno != EWOULDBLOCK) {
							perror("accept failed");
						}
						break;
					}
					int on = 1;
					if ((setsockopt(new_fd, SOL_SOCKET, SO_NOSIGPIPE, &on, sizeof(on)) < 0)) {
						std::cout << "sockoptions got fucked" << std::endl;
						break;
					}
					if ((fcntl(new_fd, F_SETFL, O_NONBLOCK)) < 0) {
						std::cout << "fcntl got fucked" << std::endl;
						break;
					}
					poller.setPollFd(new_fd, (POLLIN|POLLOUT));
				}
				
				
				ssize_t valread;
				
				valread = recv(new_fd, buffer, 1024, 0);						
				// std::cout << "Recv returned: " << valread << std::endl;
				if (valread > 0) {
					std::cout << buffer << std::endl;
					memset(buffer, 0, sizeof(buffer));
				}
				if (valread == 0) {
					std::cout << "Connection closed" << std::endl;
					if (i != 0) {
						std::cout << "Deleting connection" << std::endl;
						close(poller.getConnections()[i].fd);
						poller.getConnections().erase(poller.getConnections().begin() + i);
					}
					break; // Had this as break but maybe let's see if this way we can wait on favicon
				}
				if (valread < 0) {
					std::cout << "No bytes to read" << std::endl;
					if (EAGAIN) {
						// perror("Recv failed: ");
						// std::cout << "After on: " << i << std::endl;
						break;
					}
					// close_conn = 1;
					break;
				}
				// std::cout << buffer << std::endl;
				/* At this point should parse the request 
					from the browser and then
				send appropriate response back */

				// Respond to client
				// Reset
				// }
			}
			if (poller.getConnections()[i].revents & POLLOUT) {
				ret = firstResponse(new_fd);
				if (ret < 0) {
					perror ("   send() failed");
					break;
				}
			}
		} // End of loop through pollable connections
	}
}

serverSock*	serverBoy::getSocket() { return _socket; }

int		serverBoy::connectionErr(short revents) {
	return revents & (POLLERR|POLLNVAL) ||
			(!(revents & (POLLIN)) && revents & POLLHUP);
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

std::string	serverBoy::readBrowserRequest(char *buffer) {
	
	std::string	buff(buffer, buffer + 1024);
	std::cout << buff << std::endl;

	return "fuck off";
}
