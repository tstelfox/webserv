/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   socket.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/02/02 19:19:15 by tmullan       #+#    #+#                 */
/*   Updated: 2022/03/23 14:05:28 by tmullan       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include <sys/fcntl.h>
#include <arpa/inet.h> // inet_addr()
#include <netinet/in.h> // inet_addr()
#include <iostream>
#include "socket.hpp"

// #include "webserv.hpp"

socketMan::socketMan(int domain, int service, int protocol, int port,
                     const char *host) {

    std::cout << "host: " << host << std::endl;
    // Set up address structure
    address.sin_family = domain;
    address.sin_addr.s_addr = inet_addr(host);
    address.sin_port = htons(port);
    // Create the socket
    sock = socket(domain, service, protocol);
    testConnection(sock, "Creation of Socket");

}

socketMan::~socketMan() {}

void socketMan::testConnection(int to_test, std::string str) {
    if (to_test < 0) {
        std::cerr << "Failed to connect: " << str << "\n";
        exit(EXIT_FAILURE);
    }

}

int socketMan::getSock() { return sock; }

struct sockaddr_in &socketMan::getAddr() { return address; }

serverSock::serverSock(int domain, int service, int protocol,
                       int port, const char *host) : socketMan(domain, service, protocol, port, host), backlog(0) {
    // Bind/connect the socket
    int ret;
    int on = 1;
    // Make socket reusable
    // (char *)&on
    ret = setsockopt(getSock(), SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    if (ret < 0) {
        std::cout << "setsock fucked up" << std::endl;
        exit(-1);
    }


    ret = bindServer(sock, address);
    testConnection(ret, "Binding failure");
    // ret = fcntl(getSock(), F_GETFL, 0);
    listenServer(5);
    /* The following is the fcntl version which must be used, as per subject */
    //Set socket to be nonblocking
    ret = fcntl(getSock(), F_SETFL, O_NONBLOCK);

    if (ret < 0) {
        std::cout << "ioctl failed" << std::endl;
        exit(-1);
    }
}

serverSock::~serverSock() {}

int serverSock::bindServer(int sock, struct sockaddr_in address) {
    return bind(sock, (struct sockaddr *) &address, sizeof(address));
}

void serverSock::listenServer(int bcklg) {
    backlog = bcklg;
    int connection = listen(sock, backlog);
    testConnection(connection, "Listen failure");
}

// int	clientSock::connect_server(int sock, struct sockaddr_in address) {
// 	// int addrlen;
// 	return connect(sock, (struct sockaddr *)&address, (socklen_t)&addrlen);
// }
