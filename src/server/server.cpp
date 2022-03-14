/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   server.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/02/04 18:59:58 by tmullan       #+#    #+#                 */
/*   Updated: 2022/03/14 15:22:45 by tmullan       ########   odam.nl         */
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

	struct pollfd poll_set[42]; // 42 is arbitrary - Don't know how many we should be preparing for
	int numfds = 1;
	// int max_fd = 0;
	int socket_fd = _socket->getSock();
	memset(poll_set, 0, sizeof(poll_set));
	// update_maxfd(socket_fd, &max_fd);
	int new_fd = -1;
	int i;

	int ret;
	std::cout << "Socket fd is: " << socket_fd << std::endl;

	_socket->listenServer(backlog);

	poll_set[0].fd = socket_fd;
	poll_set[0].events = POLLIN | POLLOUT;
	int timeout = (60 * 1000);

	char buffer[1024] = {0};
	// // Make socket reusable
	// ret = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on)); // check for failure
	// if (ret < 0) {
	// 	std::cout << "setsock fucked up" << std::endl;
	// 	exit(-1);
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
		std::cout << "number of fds: " << numfds << std::endl;
		for (i = 0; i < current_size; i++) {
			if (poll_set[i].revents == 0) {
				// std::cout << "Nothing to report" << std::endl;
				continue;
			}
			if (poll_set[i].revents != POLLIN && poll_set[i].revents != POLLOUT) {
				// perror("revents");
				// Questo dovrebbe stare in silenzio diocane
				std::cout << "Error: revents=" << std::hex << poll_set[i].revents << std::endl;
				break;
			}
			// if (poll_set[i].fd == socket_fd) {
			if (poll_set[i].revents & POLLIN) {
				std::cout << "Listening socket is readable" << std::endl;
				socklen_t addrlen;
				new_fd = accept(socket_fd, (struct sockaddr *)&_socket->getAddr(), (socklen_t *)&addrlen);
				// new_fd = accept(socket_fd, NULL, NULL);
				if (new_fd < 0) {
					if (errno != EWOULDBLOCK) {
						std::cout << "Accept balls" << std::endl;
					}
					// std::cout << "Should re-route to poll again" << std::endl;
					break;
				}
				std::cout << "accepted fd: " << new_fd << std::endl;
				poll_set[numfds].fd = new_fd;
				poll_set[numfds].events = POLLIN | POLLOUT;
				
				if (poll_set[i].revents & POLLIN) {
					std::cout << "Trying to read from fd " << new_fd << std::endl;
					// int valread = recv(poll_set[i].fd, &buffer, 1024, 0);
					ssize_t valread = recv(new_fd, &buffer, 1024, 0);
					if (valread < 0) {
						std::cout << "No bytes to read" << std::endl;
						break;
					}
					if (valread == 0) {
						std::cout << "Connection closed" << std::endl;
						break;
					}
					std::cout << buffer << std::endl;
					/* At this point should parse the request 
						from the browser and then
					send appropriate response back */
					// ----------------------------- //
				}
				// ----------------------------- //
				/* Here I am attempting to check if the socket is ready
				for me to send a response back to the client(browser) */
				// ----------------------------- //
				// if (poll_set[i].revents & POLLOUT) {
				// 	std::cout << "You can write to the client" << std::endl;
				// }
				// else if (poll_set[i].revents & POLLERR) {
				// 	std::cout << "DIO PORCO MAIALE GANE" << std::endl;
				// }
				

				// Reset
				memset(buffer, 0, sizeof(buffer));

				numfds++;

				// std::cout << "numfds " << numfds << std::endl;
				// } while (new_fd != -1);
			}
			if (poll_set[i].revents & POLLOUT) {
				// Write
				std::cout << "You can write to the client on fd: " << poll_set[i].fd << std::endl;
				// socklen_t addrlen;
				// new_fd = accept(socket_fd, (struct sockaddr *)&_socket->getAddr(), (socklen_t *)&addrlen);
				if (new_fd < 0) {
					if (errno != EWOULDBLOCK) {
						std::cout << "Accept balls" << std::endl;
					}
					// std::cout << "Should re-route to poll again" << std::endl;
					break;
				}
				
				first_response(poll_set[i].fd);
				/* std::ostringstream file_content;
				std::ifstream myfile;
				if (i % 2)
					myfile.open("pages/other.html");
				else
					myfile.open("pages/index.html");
				file_content << myfile.rdbuf();
				std::string content = file_content.str();

				std::string	header = "HTTP/1.1 200 OK\nContent-Type: text/html; charset=UTF-8\nContent-Length:";
				int len = file_content.str().size(); // Literally only got time to do this
				header.append(std::to_string(len));
				header.append("\n\n");
				header.append(content);
				char *hey = new char[header.length() + 1];
				std::strcpy(hey, header.c_str());

				// write(new_fd, hey, strlen(hey));
				send(new_fd, hey, strlen(hey), 0);
				// close(new_fd);
				// std::cout << "What" << std::endl;
				delete[] hey; */

			}
			else if (poll_set[i].revents & POLLERR) {
				std::cout << "DIO PORCO MAIALE GANE" << std::endl;
			}
		}
	}
}

serverSock*	serverBoy::getSocket() { return _socket; }

void		serverBoy::first_response(int sock_fd) {
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
	send(sock_fd, hey, strlen(hey), 0);
	close(sock_fd);
	// std::cout << "What" << std::endl;
	delete[] hey;
}

std::string	serverBoy::read_browser_request(char *buffer) {
	
	std::string	buff(buffer, buffer + 1024);
	std::cout << buff << std::endl;

	return "fuck off";
}
