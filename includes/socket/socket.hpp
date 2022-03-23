/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   socket.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/02/02 15:17:36 by tmullan       #+#    #+#                 */
/*   Updated: 2022/03/23 13:43:03 by tmullan       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <sys/socket.h>
#include <netinet/in.h>
// #include <sys/fcntl.h>
// #include <iostream>
// #include "webserv.hpp"

class socketMan
{
	public:
		socketMan(int domain, int service, int protocol,
			int port, u_long interface);

		virtual ~socketMan(); // This means inherited classes don't need to have a destructor

		virtual int bindServer(int sock, struct sockaddr_in	address) = 0;
		void		testConnection(int);

		int	getSock();
		struct sockaddr_in&	getAddr();

	protected:
		int sock;
		struct sockaddr_in	address;
};

class	serverSock : public socketMan {
	
	public:
		serverSock(int domain, int service, int protocol,
			int port, u_long interface);
		virtual ~serverSock();
		// serverSock(serverSock const &x);

		virtual int	bindServer(int sock, struct sockaddr_in address);
		void		listenServer(int bcklg);

	private:
		socklen_t	addrlen;
		int			backlog;
		serverSock();
		
};


// For the client side o'sheet
/* class	clientSock : public socketMan {
	
	public:
		clientSock(int domain, int service, int protocol,
			int port, u_long interface) : socketMan(domain, service, protocol, port, interface) {
				// Bind/connect the socket
				connection = connect_server(sock, address);
				test_connection(connection);
			}

		virtual int	connect_server(int sock, struct sockaddr_in address);
	private:
		socklen_t	addrlen;
		clientSock();
		
}; */
