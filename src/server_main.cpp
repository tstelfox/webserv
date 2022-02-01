/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   server_main.cpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/01/28 11:37:35 by tmullan       #+#    #+#                 */
/*   Updated: 2022/02/01 16:25:36 by tmullan       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <fstream>
#include <sstream>

int		main() {
	
	/* Part 1: Create the Socket
	socket(domain, type, protocol); */
	int server_fd = socket(AF_INET, SOCK_STREAM, 0); // Protect against failure

	/* Part 2: Identify (name) a socket
	bind(int socket, const struct sockaddr *address, socklen_t address_len);
	struct sockaddr_in
	{
		__uint8_t		sin_len;
		sa_family_t		sin_family; ----> domain from socket()
		in_port_t		sin_port; ----> transport address. Can assign or allow operating system to
		struct in_addr	sin_addr; ----> Address 4 socket. Machine's IP address.
		char			sin_zero[8];
	}; */
	struct sockaddr_in address;
	const int PORT = 8080; // Where clients reach at.
	/* htonl converts a long int or a short int to a network representation */
	
	memset((char *)&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	address.sin_port = htons(PORT);
	
	bind(server_fd, (struct sockaddr *)&address, sizeof(address)); // Protect here as well

	/* Part 3: On server side, wait for incoming connection
	int listen(int socket, int backlog); // backlog is maximum number of pending connections.
	int accept(int socket, struct sockaddr *restrict address, socklen_t *restrict address_len);*/

	listen(server_fd, 3);

	while (1) {
		socklen_t addrlen;
		int new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);

		/* Part 4: send and receieve messages */
		char buffer[1024] = {0};
		int valread = read(new_socket, buffer, 1024);
		std::cout << buffer << std::endl;
		if (valread < 0) {
			std::cout << "No bytes to read";
		}

		std::ostringstream file_content;
		std::ifstream myfile;
		myfile.open("pages/index.html");
		file_content << myfile.rdbuf();
		std::string content = file_content.str();
		// std::cout << "EPPORCA LA MADONNA " << content << " diobe";
		// std::string	header = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length:9\n\nPorcoddio";
		std::string	header = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length:535\n\n";
		header.append(content);
		char *hey = new char[header.length() + 1];
		std::strcpy(hey, header.c_str());
		// char *hey = strdup(myfile.read()); // Will change this to c++
		write(new_socket, hey, strlen(hey));
		close(new_socket);
		// break;

				/* How the shit do you do this part in c++? */
				// std::ifstream new_socket;
				// // new_socket.open();
				// std::cout << new_socket << std::endl;

				// // char *hello = "Hello from the server";
				// std::ofstream new_socket;
				// new_socket << "Hello from the server";
				// close(new_socket);
	}
}