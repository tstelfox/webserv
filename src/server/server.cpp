/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   server.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/02/04 18:59:58 by tmullan       #+#    #+#                 */
/*   Updated: 2022/03/01 16:20:57 by tmullan       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

serverBoy::serverBoy(serverSock &sock) : _socket(&sock), ready_socket(-1) {
	// _socket = &sock;
}

serverBoy::~serverBoy() {}

void	serverBoy::runServer(int backlog) {
	
	/* The following is an attempt at using poll() */

	struct pollfd poll_set[42]; // 42 is arbitrary I believe
	int numfds = 0;
	// int max_fd = 0;
	int socket_fd = _socket->getSock();
	memset(poll_set, 0, sizeof(poll_set));
	poll_set[0].fd = socket_fd;
	poll_set[0].events = POLLIN;
	int timeout = (30 * 1000);
	numfds++;
	// update_maxfd(socket_fd, &max_fd);
	int new_fd = -1;

	int ret;
	std::cout << "Socket fd is: " << socket_fd << std::endl;
	// int yonked = -1;

	// do {
	// }
	// all of this needs to be filtered through poll() 
	
	_socket->listenServer(backlog);
	
	while (true) {
		std::cout << "Waiting on poll()..." << std::endl;
		ret = poll(poll_set, numfds, timeout);
		if (ret < 0) {
			std::cout << "Shit's fucked" << std::endl;
			break;
		}
		if (ret == 0) {
			std::cout << "Poll timed out. End" << std::endl;
			break;
		}
		for (int i = 0; i < numfds; i++) {
			if (poll_set[i].revents == 0)
				continue;
			if (poll_set[i].revents != POLLIN) {
				std::cout << "Error. revents = " << poll_set[i].revents << std::endl;
				break;
			}
			if (poll_set[i].fd == socket_fd) {
				std::cout << "Listening socket is readable" << std::endl;
				do {
					// std::cout << "Is this gonna loop?" << std::endl;
					new_fd = accept(socket_fd, NULL, NULL);
					if (new_fd < 0) {
						if (errno != EWOULDBLOCK) {
							std::cout << "Accept balls" << std::endl;
							// yonked = 1;
						}
						break;
					}
					std::cout << "New incoming connection " << new_fd << std::endl;
					poll_set[numfds].fd = new_fd;
					poll_set[numfds].events = POLLIN;
					numfds++;
				} while (new_fd != -1);
			}
			std::cout << "Well?" << std::endl;
			// else {

			// }
		}
		// if (yonked == 1)
		// 	break;
		// socklen_t	addrlen;
		// ready_socket = accept(_socket->getSock(), (struct sockaddr *)&_socket->getAddr(), (socklen_t *)&addrlen);
		
		
		char buffer[1024] = {0};
		int valread = recv(poll_set[0].fd, buffer, sizeof(buffer), 0);
		// int valread = read(ready_socket, buffer, 1024);
		std::cout << buffer << std::endl;
		// Parse the buffer for GET/POST/DELETE
		// Build header
		// Write it
		if (valread < 0) {
			std::cout << "No bytes to read";
		}
		read_browser_request(buffer);
		break ;
		std::ostringstream file_content;
		std::ifstream myfile;
		myfile.open("pages/other.html");
		file_content << myfile.rdbuf();
		std::string content = file_content.str();

		/* Have to create the header file here from the data of the file */
		std::string	header = "HTTP/1.1 200 OK\nContent-Type: text/html; charset=UTF-8\nContent-Length:";
		int len = file_content.str().size(); // Literally only got time to do this
		header.append(std::to_string(len));
        header.append("\n\n");
		header.append(content);
		char *hey = new char[header.length() + 1];
		std::strcpy(hey, header.c_str());

		write(ready_socket, hey, strlen(hey));
		close(ready_socket);
		delete[] hey;
	}
}

serverSock*	serverBoy::getSocket() { return _socket; }

std::string	serverBoy::read_browser_request(char *buffer) {
	
	std::string	buff(buffer, buffer + 1024);
	std::cout << buff << std::endl;

	return "fuck off";
}
