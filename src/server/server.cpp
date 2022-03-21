/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   server.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/02/04 18:59:58 by tmullan       #+#    #+#                 */
/*   Updated: 2022/03/21 15:37:28 by tmullan       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

// #include "server.hpp"
#include "webserv.hpp"


serverBoy::serverBoy(serverSock &sock) : _socket(&sock), ready_socket(-1) {
	// _socket = &sock;
}

serverBoy::~serverBoy() {}


	/*--------- Should just use a vector for every client connection which contains:
					-socket
					-buffer of shit to read
					-something else?
				And then close it by deleting the item -----------*/ 
void	serverBoy::runServer(int backlog) {
	
	clientConnecter		poller;
	// struct pollfd poll_set[42]; // 42 is arbitrary - Just use a vector.push_back() fam of pointers to class
	int numfds = 1;
	int socket_fd = _socket->getSock();
	// memset(poll_set, 0, sizeof(poll_set));
	int new_fd = -1;
	int i;

	int ret;
	std::cout << "Socket fd is: " << socket_fd << std::endl;

	_socket->listenServer(backlog);

	poller.setPollFd(socket_fd, (POLLIN|POLLOUT));

	// poll_set[0].fd = socket_fd;
	// poll_set[0].events = POLLIN | POLLOUT;
	int timeout = (60 * 1000);

	char buffer[1024] = {0};
	int close_conn = 0;
	int current_size;

	while (true) {
		std::cout << "<<------Waiting on poll()...------>>" << std::endl;
		ret = poll(&poller.getConnections()[0], numfds, timeout); // Could use std::vector::data() but that's c++11
		if (ret < 0) {
			perror("poll");
			break;
		}
		if (ret == 0) {
			std::cout << "Poll timed out. End" << std::endl;
			break;
		}
		current_size = poller.getConnections().size();
		// sleep(1); // Currently without this we not going anywhere but surely there must be a way without
		// std::cout << "number of fds: " << numfds << std::endl;
		std::cout << "Size of the connections: " << current_size << std::endl;
		for (i = 0; i < current_size; i++) {
			// std::cout << "current size: " << current_size << " and iteration no. " << i << std::endl;
			// if (poller.getConnections()[i].fd == -1) { // Surely there is a better way to actually loop through this shit and discard connections
			// 	// std::cout << "This was closed" << std::endl;
			// 	continue;
			// }
			if (poller.getConnections()[i].revents == 0) {
				// std::cout << "Nothing to report on " << i << std::endl;
				continue;
			}
			if (poller.getConnections()[i].revents & (POLLHUP|POLLNVAL)) {
				std::cout << "Connection was hung up or invalid requested events" << std::endl;
				close(poller.getConnections()[i].fd);
				poller.getConnections().erase(poller.getConnections().begin() + i);
				continue;
			}
			if (poller.getConnections()[i].revents != POLLIN && poller.getConnections()[i].revents != POLLOUT) {
				// perror("revents is te");
				std::cout << "Error: revents=" << std::hex << poller.getConnections()[i].revents << std::endl;
				close(poller.getConnections()[i].fd);
				poller.getConnections().erase(poller.getConnections().begin() + i);
				break;
			}
			// else {
			if (poller.getConnections()[i].revents & (POLLIN|POLLOUT)) {
				if (poller.getConnections()[i].fd == socket_fd) {
					socklen_t	addrlen;
					new_fd = accept(socket_fd, (struct sockaddr *)&_socket->getAddr(), (socklen_t *)&addrlen);
					if (new_fd < 0) {
						if (errno != EWOULDBLOCK) {
							perror("accept failed");
							// close_conn = 1;
						}
						// perror("Here? ");
						break;
					}
					poller.setPollFd(new_fd, (POLLIN|POLLOUT));
				}
				std::cout << "Listening socket is readable and writeable on fd: " << new_fd << std::endl;
				close_conn = 0;
				// std::cout << "accepted fd: " << new_fd << std::endl;
				// std::cout << "Current iteration: " << i << " and numfds (next struct being set): " << numfds << std::endl;
				// poller.setPollFd(new_fd, (POLLIN|POLLOUT));
				// poll_set[numfds].fd = new_fd;
				// poll_set[numfds].events = POLLIN | POLLOUT;
				
				/* REALLY CAN USE THE VECTOR'S SIZE() FUNCTION */
				// numfds++; // Should
				
				// std::cout << "Trying to read from fd " << new_fd << std::endl;
				// int valread = recv(poller.getConnections()[i].fd, &buffer, 1024, 0);
				while (true) {
					ssize_t valread = recv(new_fd, &buffer, 200, 0);
					if (valread < 0) {
						std::cout << "No bytes to read" << std::endl;
						if (EAGAIN) {
							perror("Recv failed: ");
							// break;
						}
						// close_conn = 1;
						break;
					}
					if (valread == 0) {
						std::cout << "Connection closed" << std::endl;
						close_conn = 1;
						break;
					}
					std::cout << buffer << std::endl;
					/* At this point should parse the request 
						from the browser and then
					send appropriate response back */
					

					// Reset
					memset(buffer, 0, sizeof(buffer));

					// std::cout << "You can write to the client on fd: " << new_fd << std::endl;
					
					// Respond to client
					ret = first_response(new_fd);
					if (ret < 0) {
						perror ("   send() failed");
						close_conn = 1;
						break;
					}
				}
			}
			else if (poller.getConnections()[i].revents & POLLERR) {
				std::cout << "DIO PORCO MAIALE GANE" << std::endl;
			}
			if (close_conn) {
				std::cout << "Closing connection: " << poller.getConnections()[i].fd << std::endl;
				close(poller.getConnections()[i].fd);
				// std::vector<struct pollfd>::iterator to_close = poller.getConnections().begin();
				poller.getConnections().erase(poller.getConnections().begin() + i);
				// numfds--;
			}
			// } // End of current connection
		} // End of loop through pollable connections
	}
}

serverSock*	serverBoy::getSocket() { return _socket; }

int		serverBoy::first_response(int sock_fd) {
	std::ostringstream file_content;
	std::ifstream myfile;
	if (sock_fd % 2)
		myfile.open("pages/other.html");
	else
		myfile.open("pages/index.html");
	file_content << myfile.rdbuf();
	std::string content = file_content.str();

	std::string	header = "HTTP/1.1 200 OK\nContent-Type: text/html; charset=UTF-8\nContent-Length:";
	int len = file_content.str().size();
	header.append(std::to_string(len));
	header.append("\n\n");
	header.append(content);
	char *hey = new char[header.length() + 1];
	std::strcpy(hey, header.c_str());

	// write(sock_fd, hey, strlen(hey));
	int ret = send(sock_fd, hey, strlen(hey), 0);
	// close(sock_fd);
	// std::cout << "What" << std::endl;
	delete[] hey;
	return ret;
}

std::string	serverBoy::read_browser_request(char *buffer) {
	
	std::string	buff(buffer, buffer + 1024);
	std::cout << buff << std::endl;

	return "fuck off";
}
