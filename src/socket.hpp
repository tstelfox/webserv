/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   socket.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/02/02 15:17:36 by tmullan       #+#    #+#                 */
/*   Updated: 2022/02/03 12:13:29 by tmullan       ########   odam.nl         */
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
	private:
		int sock;
		int connection;
		struct sockaddr_in	address;
};

class	serverSock : public socketMan {
	
	public:
		serverSock(int domain, int service, int protocol,
			int port, u_long interface) : socketMan(domain, service, protocol, port, interface);

		virtual int	connect_server(int sock, struct sockaddr_in address);
	private:
		serverSock();
		
};
