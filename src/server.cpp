/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   server.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/02/04 18:59:58 by tmullan       #+#    #+#                 */
/*   Updated: 2022/02/04 19:44:02 by tmullan       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

serverBoy::serverBoy(serverSock &sock) : socket(&sock), ready_socket(-1) {
	// socket = &sock;
}

serverBoy::~serverBoy() {}

void	serverBoy::run_server(int backlog) {
	
	socket->listen_server(backlog);
	while (1) {
		socklen_t	addrlen;
		ready_socket = accept(socket->getSock(), (struct sockaddr *)&socket->getAddr(), (socklen_t *)&addrlen);
		char buffer[1024] = {0};
		int valread = read(ready_socket, buffer, 1024);
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

		write(ready_socket, hey, strlen(hey));
		close(ready_socket);
	}
}