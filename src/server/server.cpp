/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   server.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/02/04 18:59:58 by tmullan       #+#    #+#                 */
/*   Updated: 2022/03/01 17:47:03 by tmullan       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

// #include "server.hpp"
#include "webserv.hpp"


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
	numfds++;
	// update_maxfd(socket_fd, &max_fd);
	int new_fd = -1;
	int i;
	// int on = 1;

	int ret;
	std::cout << "Socket fd is: " << socket_fd << std::endl;

	// int yonked = -1;

	_socket->listenServer(backlog);

	poll_set[0].fd = socket_fd;
	poll_set[0].events = POLLIN;
	int timeout = (60 * 1000);

	char buffer[1024] = {0};
	// // Make socket reusable
	// ret = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on)); // check for failure
	// if (ret < 0) {
	// 	std::cout << "setsock fucked up" << std::endl;
	// 	exit(-1);
	// }
	
	// //Set socket to be nonblocking
	// ret = ioctl(socket_fd, FIONBIO, (char *)&on);
	// if (ret < 0) {
	// 	std::cout << "ioctl failed" << std::endl;
	// 	exit(-1);
	// }
	// do {
	// }
	// all of this needs to be filtered through poll() 
	
	
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
		int current_size = numfds;
		for (i = 0; i < current_size; i++) {
			// std::cout << "i is " << i << " and numfds " << numfds << std::endl;
			if (poll_set[i].revents == 0) {
				continue;
			}
			if (poll_set[i].revents != POLLIN) {
				std::cout << "Error. revents = " << poll_set[i].revents << std::endl;
				break;
			}
			if (poll_set[i].fd == socket_fd) {
				std::cout << "Listening socket is readable" << std::endl;
				// do {
				// std::cout << "Is this gonna loop?" << std::endl;
				// socklen_t addrlen;
				// new_fd = accept(socket_fd, (struct sockaddr *)&_socket->getAddr(), (socklen_t *)&addrlen);
				new_fd = accept(socket_fd, NULL, NULL);
				// std::cout << "accepted fd: " << new_fd << std::endl;
				// std::cout << "gettin stuck?" << std::endl;
				if (new_fd < 0) {
					// std::cout << "what" << numfds << std::endl;
					// perror("Yonk why");
					if (errno != EWOULDBLOCK) {
						std::cout << "Accept balls" << std::endl;
						// yonked = 1;
					}
					break;
				}
				std::cout << "New incoming connection " << new_fd << std::endl;
				poll_set[numfds].fd = new_fd;
				poll_set[numfds].events = POLLIN;
				std::cout << "numfds " << numfds << std::endl;
				numfds++;
				// std::cout << "fd we're attempting to get at is " << poll_set[numfds].fd << " i is " << i << std::endl;
				// // char buffer[1024] = {0};
				// int valread = recv(poll_set[numfds].fd, &buffer, 1024, 0);
				// // int valread = read(poll_set[0].fd, buffer, 1024);
				// if (valread < 0) {
				// 	std::cout << "No bytes to read" << std::endl;
				// }
				// perror("What is errno?");
				// std::cout << buffer << std::endl;
				// } while (new_fd != -1);
			}
			// std::cout << "Well?" << std::endl;
			// else {

			// }
		}

		// if (yonked == 1)
		// 	break;
		// socklen_t	addrlen;
		// ready_socket = accept(_socket->getSock(), (struct sockaddr *)&_socket->getAddr(), (socklen_t *)&addrlen);
		
		
		//  std::cout << "fd we're attempting to get at is " << poll_set[i].fd << " i is " << i << std::endl;
		int valread = recv(poll_set[i].fd, &buffer, 1024, 0);
		// int valread = read(poll_set[0].fd, buffer, 1024);
		if (valread < 0) {
			std::cout << "No bytes to read" << std::endl;
		}
		// perror("What is errno?");
		std::cout << buffer << std::endl;
		// Parse the buffer for GET/POST/DELETE
		// Build header
		// Write it
		read_browser_request(buffer);
		// break ;
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

		write(new_fd, hey, strlen(hey));
		close(new_fd);
		delete[] hey;
	}
}

serverSock*	serverBoy::getSocket() { return _socket; }

std::string	serverBoy::read_browser_request(char *buffer) {
	
	std::string	buff(buffer, buffer + 1024);
	std::cout << buff << std::endl;

	return "fuck off";
}
