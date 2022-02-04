/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   socket.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/02/02 19:19:15 by tmullan       #+#    #+#                 */
/*   Updated: 2022/02/04 17:28:47 by tmullan       ########   odam.nl         */
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
	test_connection(sock);
	

}

socketMan::~socketMan() {}

void 	socketMan::test_connection(int to_test) {
	if (to_test < 0) {
		std::cerr << "Failed to connect";
		exit(EXIT_FAILURE);
	}

}

int		serverSock::connect_server(int sock, struct sockaddr_in address) {
	// int addrlen;
	return bind(sock, (struct sockaddr *)&address, sizeof(address));
}

void	serverSock::listen_server(int bcklg) {
	backlog = bcklg;
	connection = listen(sock, backlog);
	test_connection(connection);
}

// int	clientSock::connect_server(int sock, struct sockaddr_in address) {
// 	// int addrlen;
// 	return connect(sock, (struct sockaddr *)&address, (socklen_t)&addrlen);
// }
