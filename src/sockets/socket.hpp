/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   socket.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/02/02 15:17:36 by tmullan       #+#    #+#                 */
/*   Updated: 2022/04/28 18:41:46 by ubuntu        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
// #include <sys/fcntl.h>
// #include <iostream>
// #include "webserv.hpp"

class Create_socket_fd
{
    public:
        Create_socket_fd(int domain, int service, int protocol,
                  int port, const char *host);
        virtual ~Create_socket_fd();   // This means inherited classes don't need to have a destructor

        void    testConnection(int, std::string);
        int     getSock();
        struct  sockaddr_in &getAddr();

    protected:
        int                 sock;
        struct sockaddr_in  address;
};

class serverSock : public Create_socket_fd
{
    public:
        serverSock(int domain, int service, int protocol,
                   int port, const char *host);
        virtual ~serverSock();

        void bindServer(int sock, struct sockaddr_in address);
        void set_socket_options( void );
        void listenServer(int bcklg);
        void make_socket_nonblocking( void );

    private:
        socklen_t           addrlen;
        int                 backlog;
        serverSock();
};


// For the client side o'sheet
/* class	clientSock : public Create_socket_fd {

	public:
		clientSock(int domain, int service, int protocol,
			int port, const char *host) : Create_socket_fd(domain, service, protocol, port,host) {
				// Bind/connect the socket
				connection = connect_server(sock, address);
				test_connection(connection);
			}

		virtual int	connect_server(int sock, struct sockaddr_in address);
	private:
		socklen_t	addrlen;
		clientSock();

}; */
