/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   server.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/02/04 18:59:58 by tmullan       #+#    #+#                 */
/*   Updated: 2022/03/15 14:11:49 by tmullan       ########   odam.nl         */
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
	
	/* The following is an attempt at using poll() */

	struct pollfd poll_set[42]; // 42 is arbitrary - Just use a vector.push_back() fam of pointers to class
	int numfds = 1;
	int socket_fd = _socket->getSock();
	memset(poll_set, 0, sizeof(poll_set));
	int new_fd = -1;
	int i;

	int ret;
	std::cout << "Socket fd is: " << socket_fd << std::endl;

	_socket->listenServer(backlog);

	poll_set[0].fd = socket_fd;
	poll_set[0].events = POLLIN | POLLOUT;
	int timeout = (60 * 1000);

	char buffer[1024] = {0};
	int close_conn = 0;
	
	// socklen_t	addrlen;
	// new_fd = accept(socket_fd, (struct sockaddr *)&_socket->getAddr(), (socklen_t *)&addrlen);
	// if (new_fd < 0) {
	// 	if (errno != EWOULDBLOCK) {
	// 		perror("accept failed");
	// 		// close_conn = 1;
	// 	}
	// 	// perror("Here? ");
	// 	return;
	// }
	while (true) {
		std::cout << "Waiting on poll()..." << std::endl;
		ret = poll(poll_set, numfds, timeout);
		if (ret < 0) {
			perror("poll");
			break;
		}
		if (ret == 0) {
			std::cout << "Poll timed out. End" << std::endl;
			break;
		}
		int current_size = numfds;
		sleep(3); // Currently without this we not going anywhere but surely there must be a way without
		// std::cout << "number of fds: " << numfds << std::endl;
		for (i = 0; i < current_size; i++) {
			std::cout << "current size: " << current_size << " and iteration no. " << i << std::endl;
			if (poll_set[i].fd == -1) {
				std::cout << "This was closed" << std::endl;
				continue;
			}
			if (poll_set[i].revents == 0) {
				std::cout << "Nothing to report on " << i << std::endl;
				continue;
			}
			if (poll_set[i].revents & (POLLHUP|POLLNVAL)) {
				close(poll_set[i].fd);
				poll_set[i].fd = -1;
				std::cout << "Connection was hung up or invalid requested events" << std::endl;
				continue;
			}
			if (poll_set[i].revents != POLLIN && poll_set[i].revents != POLLOUT) {
				// perror("revents is te");
				std::cout << "Error: revents=" << std::hex << poll_set[i].revents << std::endl;
				break;
			}
			if (poll_set[i].fd == socket_fd) {
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
			}
			if (poll_set[i].revents & (POLLIN|POLLOUT)) {
				std::cout << "Listening socket is readable" << std::endl;
				// socklen_t addrlen;
				// if (i == 0)
				// 	new_fd = accept(poll_set[i].fd, (struct sockaddr *)&_socket->getAddr(), (socklen_t *)&addrlen);
				// if (new_fd < 0) {
				// 	if (errno != EWOULDBLOCK) {
				// 		perror("accept failed");
				// 		// close_conn = 1;
				// 	}
				// 	// perror("Here? ");
				// 	break;
				// }
				close_conn = 0;
				std::cout << "accepted fd: " << new_fd << std::endl;
				std::cout << "Current iteration: " << i << " and numfds (next struct being set): " << numfds << std::endl;
				// poll_set[i].fd = new_fd;
				poll_set[numfds].fd = new_fd;
				poll_set[numfds].events = POLLIN | POLLOUT;
				numfds++;
				
				std::cout << "Trying to read from fd " << new_fd << std::endl;
				// int valread = recv(poll_set[i].fd, &buffer, 1024, 0);
				ssize_t valread = recv(new_fd, &buffer, 1024, 0);
				if (valread < 0) {
					std::cout << "No bytes to read" << std::endl;
					close_conn = 1;
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

				std::cout << "You can write to the client on fd: " << poll_set[i].fd << std::endl;
				
				// Respond to client
				ret = first_response(new_fd);
				if (ret < 0) {
					perror ("   send() failed");
					close_conn = 1;
					break;
				}
			}
			else if (poll_set[i].revents & POLLERR) {
				std::cout << "DIO PORCO MAIALE GANE" << std::endl;
			}
		} // End of i loop
		if (close_conn) {
			std::cout << "Closing connection: " << poll_set[i].fd << std::endl;
			close(new_fd);
			poll_set[i].fd = -1;
			// numfds--;
		}
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
