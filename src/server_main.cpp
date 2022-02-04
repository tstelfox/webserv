/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   server_main.cpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/02/03 15:40:57 by tmullan       #+#    #+#                 */
/*   Updated: 2022/02/04 17:21:56 by tmullan       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "socket.hpp"

#include <iostream>
#include <unistd.h>
#include <sstream>
#include <fstream>

int		main() {

	serverSock	hello(AF_INET, SOCK_STREAM, 0, 8080, INADDR_ANY);
	
	hello.listen_server(2);
	// listen(hello.getSock(), 3);

	while (1) {
		socklen_t	addrlen;
		int new_socket = accept(hello.getSock(), (struct sockaddr *)&hello.getAddr(), (socklen_t*)&addrlen);
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

		// This is the creation of the header which will need to be worked out
		std::string	header = "HTTP/1.1 200 OK\nContent-Type: text/html; charset=UTF-8\nContent-Length:535\n\n";
		header.append(content);
		char *hey = new char[header.length() + 1];
		std::strcpy(hey, header.c_str());


		// std::string	header = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length:9\n\nPorcoddio";

		//Here I actually write to the socket
		write(new_socket, hey, strlen(hey));
		close(new_socket);
	}
	return 0;
}