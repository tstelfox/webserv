/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   socket.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/02/02 19:19:15 by tmullan       #+#    #+#                 */
/*   Updated: 2022/02/11 21:14:05 by tmullan       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "socket.hpp"

socketMan::socketMan(int domain, int service, int protocol, int port,
		u_long interface) {
	
	// Set up address structure
	address.sin_family = domain;
	address.sin_addr.s_addr = htonl(interface);
	address.sin_port = htons(port);
	// Create the socket
	sock = socket(domain, service, protocol);
	testConnection(sock);
	
}

socketMan::~socketMan() {}

void 	socketMan::testConnection(int to_test) {
	if (to_test < 0) {
		std::cerr << "Failed to connect";
		exit(EXIT_FAILURE);
	}

}

int		socketMan::getSock() { return sock; }

struct sockaddr_in&	socketMan::getAddr() { return address; }

int		serverSock::connectServer(int sock, struct sockaddr_in address) {
	return bind(sock, (struct sockaddr *)&address, sizeof(address));
}

void	serverSock::listenServer(int bcklg) {
	backlog = bcklg;
	int connection = listen(sock, backlog);
	testConnection(connection);
}

// int	clientSock::connect_server(int sock, struct sockaddr_in address) {
// 	// int addrlen;
// 	return connect(sock, (struct sockaddr *)&address, (socklen_t)&addrlen);
// }
