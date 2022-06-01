/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   poller.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/02/04 18:59:58 by tmullan       #+#    #+#                 */
/*   Updated: 2022/05/16 14:27:57 by akramp        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

//#include "server.hpp"
//#include "serverConfig.hpp"
#include "client.hpp"
#include "poller.hpp"
#include "colours.hpp"
#include "socket.hpp"
#include <signal.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/fcntl.h>
#include <sys/types.h>
#include "sys/poll.h"
#include <netinet/in.h> // inet_addr()
#include <arpa/inet.h> // inet_addr()
#include <utility>

poller::poller(configVector const& configVector) : _serverConfigs(configVector) 
{
    pollConnections();
}

poller::~poller() {}

void poller::setPollFd(int fd, short events) {
    struct pollfd newPollFd;
    newPollFd.fd = fd;
    newPollFd.events = events;
    _sockets.push_back(newPollFd);
}

void poller::deleteConnection(int fd) {
    std::map<int, client>::iterator clientIt = _clients.find(fd);
//    std::cout << "CLient size before: " << _clients.size() << std::endl;
//    _client.erase()
    if (clientIt != _clients.end()) {
        _clients.erase(clientIt);
        close(fd);
    }
//    std::cout << "CLient size after: " << _clients.size() << std::endl;
}

int poller::newConnection(int fd) {
    /* TODO cleanup and split this function after cgi */
    socklen_t addrLen;
    struct sockaddr_in addr;
    bzero(&addr, sizeof(struct sockaddr_in));
    std::cout << RED << "fd: " << fd << RESET_COLOUR << std::endl;
    int newConnection = accept(fd, (struct sockaddr *) &addr, (socklen_t * ) &addrLen);

    if (newConnection < 0) {
        if (errno != EWOULDBLOCK) {
            perror("accept failed");
        }
        return 0;
    }
    int on = 1;
//    if ((setsockopt(newConnection, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int)) < 0)) {
    if ((setsockopt(newConnection, SOL_SOCKET, SO_NOSIGPIPE, &on, sizeof(int)) < 0)) {
        std::cout << "sockoptions got fucked" << std::endl;
        return 0;
    }
    if ((fcntl(newConnection, F_SETFL, O_NONBLOCK)) < 0) {
        std::cout << "fcntl got fucked" << std::endl;
        return 0;
    }
    setPollFd(newConnection, (POLLIN | POLLOUT));
    /* Retrieving the host:port this connection is coming from
     * This could Perhaps even be its own function
    */
    struct sockaddr_in sin;
    socklen_t len = sizeof(sin);
    int port;
    char *host = NULL;

    if (getsockname(newConnection, (struct sockaddr *)&sin, &len))
        perror("getsockname");
    else {
        port = ntohs(sin.sin_port);
        host = inet_ntoa(sin.sin_addr);
        std::cout << "New connection from port: " << port << \
            " on host: " << host << std::endl;
    }
    /*
        Make a vector of only the configs relevant to the host:port combination
        And pass that into the client class for server block routing
    */
    configVector relevant;
    std::string hostIp = host;
    for (configVector::iterator it = _serverConfigs.begin(); it != _serverConfigs.end(); it++) {
        if (it->get_port() == port && (it->get_host() == hostIp || \
            ((it->get_host()) == "localhost" && !hostIp.compare("127.0.0.1"))))
            relevant.push_back(*it);
    }
//    std::cout << "Relevant vectors are:" << std::endl;
//    for (configVector::iterator iter = relevant.begin(); iter != relevant.end(); iter++) {
//        std::cout << "Port: " << iter->get_port() << " host: " << iter->get_host() << \
//            " and server_name: " << iter->get_server_name() << std::endl;
//    }

    std::cout << RED << "New accepted client connection: " << newConnection << RESET_COLOUR << std::endl;

    client  newClient(hostIp, port, relevant, newConnection);
    _clients.insert(std::make_pair(newConnection, newClient));

    return 1;
}

void    poller::pair_host_and_port(std::set<std::pair<std::string, int> >  &ports)
{
    for (configVector::iterator it = _serverConfigs.begin(); it != _serverConfigs.end(); it++)
        ports.insert(std::make_pair(it->get_host(), it->get_port()));
}

void    poller::create_vector_of_pollfd_sockets(std::set<int> &listenSockets, std::set<std::pair<std::string, int> >  &ports)
{
    for (std::set<std::pair<std::string, int> >::iterator i = ports.begin(); i != ports.end(); i++)
    {
        std::cout << "Host: " << i->first << " Port: " << i->second << std::endl; //create socket_fd
        
        std::string host = i->first;
        int         port = i->second;
        serverSock buildSocket(AF_INET, SOCK_STREAM, 0, port, host.c_str()); 

        int newSocket = buildSocket.getSock();
        listenSockets.insert(newSocket);

        setPollFd(newSocket, (POLLIN | POLLOUT));
    }
}

std::set<int> poller::openPorts( void )
{
    std::set<std::pair<std::string, int> >  ports;
    std::set<int>                           listenSockets;

    pair_host_and_port(ports);
    create_vector_of_pollfd_sockets(listenSockets, ports);
  
    return listenSockets;
}

int poller::respondToClient(int socket, std::string response) {

    char toSend[response.length() + 1];
    std::strcpy(toSend, response.c_str());

    int ret = send(socket, toSend, strlen(toSend), 0);
    return ret;
}

bool poller::check_fds_with_poll( void )
{
    /**
         * poll();
         * it waits for one of a set of file descriptors to become 
         * ready to perform I/O.
    **/

    if (poll(&(*_sockets.begin()), _sockets.size(), -1) < 0)
    {
        perror("poll");
        return false;
    }
    return true;
}

int poller::connectionError(short revents) const
{
    /*  
        connectionError() checks if poll has set the revents of that socket to error
        So it returns true if revents is set to POLLERR or POLLNVAL
        OR if it is set to POLLHUP but not to POLLIN

        from the poll man page:
        POLLERR, POLLHUP, or POLLNVAL.
        (These three bits are
        meaningless in the events field, and will be set in the revents
        field whenever the corresponding condition is true.
    */

    return revents & (POLLERR | POLLNVAL) ||
           (!(revents & (POLLIN)) && revents & POLLHUP);
}

bool poller::check_if_revents_errors (socketVector::iterator &it)
{
    if (connectionError(it->revents))
    {
        std::cout << "Connection Error: " << std::hex << it->revents << std::endl;
        deleteConnection(it->fd);
        _sockets.erase(it);
        return false;
    }
    return true;
}

// bool poller::there_is_data_to_read_from_the_fd()
// {}

void poller::pollConnections()
{

    std::set<int>               portSockets = openPorts();
    std::map<int, client*>      cgiSockets;
    char                        buffer[BUFF_SIZE] = {0};

    while (true)
    {
        if (check_fds_with_poll() == false)
            break ;
        
        for (socketVector::iterator it = _sockets.begin(); it != _sockets.end(); it++) // loop_over_open_fds
        {
            /* 
                Map of current client sorted by socket
                to find wich socket is mapped to which client
             */
            client &currentClient = _clients.find(it->fd)->second; 

            check_if_revents_errors(it);

            if (it->revents & POLLIN)
            {
                if (portSockets.count(it->fd)) { // This checks that it's one of the listening sockets
                    newConnection(it->fd);
                    break;
                }
                if (cgiSockets.count(it->fd)) {
                    char cgiBuffer[BUFF_SIZE];
                    int cgiRead = read(it->fd, cgiBuffer, BUFF_SIZE - 2);
                    std::cout << RED << "Cgi buffer" << cgiBuffer << RESET_COLOUR << std::endl;
                    if (cgiRead)
                        cgiSockets.find(it->fd)->second->saveCgiResponse(cgiBuffer);
                    memset(cgiBuffer, 0, sizeof(buffer));
                    close(it->fd);
                    cgiSockets.erase(cgiSockets.find(it->fd));
                    continue;
                }
                int valRead = recv(it->fd, buffer, BUFF_SIZE - 2, 0);
                if (valRead) {
//                    std::cout << CYAN << "The ahhhhhhh: " << buffer << RESET_COLOUR << std::endl;
                    currentClient.fillBuffer(buffer);
                    memset(buffer, 0, sizeof(buffer));
                }
                if (!valRead) {
                    deleteConnection(it->fd);
                    _sockets.erase(it);
                    break;
                }
                if (valRead < 0) {
                    std::cout << RED << "Error receiving from client" << RESET_COLOUR << std::endl;
                    deleteConnection(it->fd);
                    _sockets.erase(it);
                    break;
                }
            }
            else if (it->revents & POLLOUT) {
//                std::cout << MAGENTA << "FRIGGIN FRIG" << RESET_COLOUR << std::endl;
                if (currentClient.isBufferFull()) {
                    currentClient.parseRequestHeader();
                    std::string response = currentClient.getResponse();
                    if (currentClient.isCgi()) {
                        if (!currentClient.getCgiResponse().empty()) {
                            response = currentClient.getCgiResponse();
                        }
                        else {
                            int cgiFd = currentClient.getCgiFd();
                            struct pollfd newCgiFd;
                            newCgiFd.fd = cgiFd;
                            newCgiFd.events = POLLIN;

                            cgiSockets.insert(std::make_pair(cgiFd, &currentClient));
                            _sockets.insert(_sockets.begin(), newCgiFd);
                            break;
                        }
                    }
                    int valSent = respondToClient(it->fd, response);
                    if (valSent) {
                        deleteConnection(it->fd);
                        _sockets.erase(it);
                        break;
                    }
                    else if (!valSent) {
                        deleteConnection(it->fd);
                        _sockets.erase(it);
                        break;
                        std::cout << "Nothing more to send" << std::endl;
                    }
                    else if (valSent < 0) {
                        std::cout << RED << "Error sending to client" << RESET_COLOUR << std::endl;
                        deleteConnection(it->fd);
                        _sockets.erase(it);
                        break;
                    }
                    currentClient.resetClient();
                }
            }
        }
    }
}
