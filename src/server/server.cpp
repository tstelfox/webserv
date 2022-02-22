/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   server.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/02/04 18:59:58 by tmullan       #+#    #+#                 */
/*   Updated: 2022/02/22 18:44:40 by tmullan       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

serverBoy::serverBoy(serverSock &sock) : _socket(&sock), ready_socket(-1) {
	// _socket = &sock;
}

serverBoy::~serverBoy() {}

void	serverBoy::runServer(int backlog) {
	
	_socket->listenServer(backlog);
	while (true) {
		socklen_t	addrlen;
		ready_socket = accept(_socket->getSock(), (struct sockaddr *)&_socket->getAddr(), (socklen_t *)&addrlen);
		
		
		char buffer[1024] = {0};
		int valread = read(ready_socket, buffer, 1024);
		std::cout << buffer << std::endl;
		// Parse the buffer for GET/POST/DELETE
		// Build header
		// Write it

		if (valread < 0) {
			std::cout << "No bytes to read";
		}
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
	(void)buffer;
	return "fuck off";
}
