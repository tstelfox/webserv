/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   socket.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/02/02 15:17:36 by tmullan       #+#    #+#                 */
/*   Updated: 2022/02/04 17:21:52 by tmullan       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>

class socketMan
{
	public:
		socketMan(int domain, int service, int protocol,
			int port, u_long interface);
		virtual ~socketMan(); // This means inherited classes don't need to have a destructor

		virtual int connect_server(int sock, struct sockaddr_in	address) = 0;
		void		test_connection(int);

		int	getSock() { return sock; }
		int	getConnection() { return connection; }
		struct sockaddr_in&	getAddr() { return address; }

	protected:
		int sock;
		int connection;
		struct sockaddr_in	address;
};

class	serverSock : public socketMan {
	
	public:
		serverSock(int domain, int service, int protocol,
			int port, u_long interface) : socketMan(domain, service, protocol, port, interface) , backlog(0) {
				// Bind/connect the socket
				connection = connect_server(sock, address);
				test_connection(connection);

			}

		virtual int	connect_server(int sock, struct sockaddr_in address);
		void		listen_server(int bcklg);

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
