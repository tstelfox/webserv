/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   socket.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/02/02 19:19:15 by tmullan       #+#    #+#                 */
/*   Updated: 2022/06/02 17:22:39 by ask           ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include <sys/fcntl.h>
#include <arpa/inet.h> // inet_addr()
#include <netinet/in.h> // inet_addr()
#include <iostream>
#include "socket.hpp"

Create_socket_fd::Create_socket_fd(int domain, int service, int protocol, int port,
                     const char *host)
{
    // Set up address structure
    address.sin_family =        domain;
    address.sin_addr.s_addr =   inet_addr(host);
    address.sin_port =          htons(port);

    // Create the socket
    /*  
        socket() simply returns to you a socket descriptor 
        that you can use in later system calls, or -1 on error
    */
    sock =                      socket(domain, service, protocol);

    // Test the socket
    testConnection(sock, "Creation of Socket");
}

Create_socket_fd::~Create_socket_fd() {}

void Create_socket_fd::testConnection(int to_test, std::string str)
{
    if (to_test < 0)
    {
        std::cerr << "Failed to connect: " << str << "\n";
        exit(EXIT_FAILURE);
    }
}

int Create_socket_fd::getSock() { return sock; }

struct sockaddr_in &Create_socket_fd::getAddr() { return address; }


/***
 * Once you have a socket, you might have to associate that socket with a port 
 * on your local machine. The port number is used by the kernel to match 
 * an incoming packet to a certain process’s socket descriptor.
 * When a remote machine wants to connect to your server program,
 * it needs two pieces of information: the IP address and the port number. 
 * The bind() call allows you to do just that, the IP address and port will be
 * bound to the socket.
***/

serverSock::serverSock(int domain, int service, int protocol,
                       int port, const char *host) : Create_socket_fd(domain, service, protocol, port, host), backlog(0)
{
    set_socket_options();
    bindServer(sock, address);
    listenServer(5);
    make_socket_nonblocking();
}

void serverSock::bindServer(int sock, struct sockaddr_in address)
{
    int ret = bind(sock, (struct sockaddr *) &address, sizeof(address));
    testConnection(ret, "Binding failure");
}

void serverSock::set_socket_options( void )
{
    int on = 1;
    int ret = setsockopt(getSock(), SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    if (ret < 0)
    {
        std::cout << "setsock fucked up" << std::endl;
        exit(-1);
    }
}

void serverSock::listenServer(int bcklg)
{
    backlog = bcklg;
    int connection = listen(sock, backlog);
    testConnection(connection, "Listen failure");
}

    /* 
        The following is the fcntl version which must be used, as per subject
        Set socket to be nonblocking.
    */
   
void serverSock::make_socket_nonblocking( void )
{
    int ret = fcntl(getSock(), F_SETFL, O_NONBLOCK);
    if (ret < 0)
    {
        std::cout << "ioctl failed" << std::endl;
        exit(-1);
    }
}

serverSock::~serverSock() {}