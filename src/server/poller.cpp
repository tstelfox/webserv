/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   poller.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/02/04 18:59:58 by tmullan       #+#    #+#                 */
/*   Updated: 2022/06/02 17:27:34 by ask           ########   odam.nl         */
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

void poller::setPollFd(int fd, short events)
{
    struct pollfd newPollFd;
    
    newPollFd.fd        = fd;
    newPollFd.events    = events;

    _sockets.push_back(newPollFd);
}

void poller::deleteConnection(int fd)
{
    std::map<int, client>::iterator clientIt = _clients.find(fd);
    
    if (clientIt != _clients.end())
    {
        _clients.erase(clientIt);
        close(fd);
    }
}

static bool    accept_error_check(int newConnection)
{
    if (newConnection < 0)
    {
        if (errno != EWOULDBLOCK)
            perror("accept failed");
        return false;
    }
    return true;
}

static bool set_socket_options(int &newConnection)
{
    int on = 1;
    if ((setsockopt(newConnection, SOL_SOCKET, SO_NOSIGPIPE, &on, sizeof(int)) < 0))
    {
        std::cout << "sockoptions got fucked" << std::endl;
        return false;
    }
    return true;
}

static bool make_fd_non_blocking(int &newConnection)
{
    if ((fcntl(newConnection, F_SETFL, O_NONBLOCK)) < 0)
    {
        std::cout << "fcntl got fucked" << std::endl;
        return false;
    }
    return true;
}
void    set_sockadr_struct(int &newConnection, struct sockaddr_in &sin, socklen_t &len, int &port, \
                            char *host)
{
    /**
     * getsockname() returns the current address to which the socket
     * sockfd is bound
     * Here we check if it's an empty connection, should be by now in the code
     **/
    if (getsockname(newConnection, (struct sockaddr *)&sin, &len))
        perror("getsockname");
    else
    {
        port = ntohs(sin.sin_port);     // ntohs: Convert multi-byte integer types \
                                        // from host byte order to network byte order
        host = inet_ntoa(sin.sin_addr); // inet_ntoa: Convert IP addresses from a \
                                        // dots-and-number string to a struct in_addr and back

        std::cout << "New connection from port: " << port << \
            " on host: " << host << std::endl;
    }
}

void    poller::make_relevant_config_vec(int &newConnection, configVector &relevant, int &port, std::string &hostIp)
{
    for (configVector::iterator it = _serverConfigs.begin(); it != _serverConfigs.end(); it++)
    {
        if (it->get_port() == port && (it->get_host() == hostIp || \
            ((it->get_host()) == "localhost" && !hostIp.compare("127.0.0.1"))))
            relevant.push_back(*it);
    }

    std::cout << RED << "New accepted client connection: " << newConnection << RESET_COLOUR << std::endl;
}

int poller::newConnection(int fd)
{
    socklen_t           addrLen;
    struct sockaddr_in  addr;

    bzero(&addr, sizeof(struct sockaddr_in));
    std::cout << RED << "fd: " << fd << RESET_COLOUR << std::endl;
    int newConnection = accept(fd, (struct sockaddr *) &addr, (socklen_t * ) &addrLen);

    if (accept_error_check(newConnection) == false)
        return 0;
    if (set_socket_options(newConnection) == false)
        return 0;
    if (make_fd_non_blocking(newConnection) == false)
        return 0;
    setPollFd(newConnection, (POLLIN | POLLOUT));

    struct sockaddr_in  sin;
    socklen_t           len = sizeof(sin);
    int                 port;
    char                *host = NULL;
    
    /**
     * getsockname() returns the current address to which the socket
     * sockfd is bound
     * Here we check if it's an empty connection, should be by now in the code
     **/
    if (getsockname(newConnection, (struct sockaddr *)&sin, &len))
        perror("getsockname");
    else
    {
        port = ntohs(sin.sin_port);     // ntohs: Convert multi-byte integer types \
                                        // from host byte order to network byte order
        host = inet_ntoa(sin.sin_addr); // inet_ntoa: Convert IP addresses from a \
                                        // dots-and-number string to a struct in_addr and back

        std::cout << "New connection from port: " << port << \
            " on host: " << host << std::endl;
    }
    /*
        Make a vector of only the configs relevant to the host:port combination
        And pass that into the client class for server block routing
    */
    configVector relevant;
    std::string hostIp = host;
    make_relevant_config_vec(newConnection, relevant, port, hostIp);

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

int poller::listening_socket(std::set<int> &portSockets, socketVector::iterator &it)
{
    if (portSockets.count(it->fd)) // This checks that it's one of the listening sockets
    { 
        newConnection(it->fd);
        return BREAK;
    }
    return DO_NOTHING;
}

int poller::cgi_socket(std::map<int, client*> &cgiSockets, char *buffer, socketVector::iterator &it)
{
    if (cgiSockets.count(it->fd))
    {
        char    cgiBuffer[BUFF_SIZE];
        int     cgiRead = read(it->fd, cgiBuffer, BUFF_SIZE - 2);

        std::cout << RED << "Cgi buffer" << cgiBuffer << RESET_COLOUR << std::endl;

        if (cgiRead)
            cgiSockets.find(it->fd)->second->saveCgiResponse(cgiBuffer);
        memset(cgiBuffer, 0, sizeof(buffer));
        close(it->fd);
        cgiSockets.erase(cgiSockets.find(it->fd));

        return CONTINUE;
    }
    return DO_NOTHING;
}

int poller::handle_incoming_message(socketVector::iterator &it, char *buffer, client &currentClient)
{
    int valRead = recv(it->fd, buffer, BUFF_SIZE - 2, 0); //see if thers an incoming message, or none returning -1
    if (valRead)
    {
        currentClient.fillBuffer(buffer);
        memset(buffer, 0, BUFF_SIZE);
    }
    if (!valRead)
    {
        deleteConnection(it->fd);
        _sockets.erase(it);
        return BREAK;
    }
    if (valRead < 0)
    {
        std::cout << RED << "Error receiving from client" << RESET_COLOUR << std::endl;
        deleteConnection(it->fd);
        _sockets.erase(it);
        return BREAK;
    }
    return DO_NOTHING;
}

int poller::read_from_fd(std::set<int> &portSockets, \
                        std::map<int, client*> &cgiSockets, char *buffer, \
                        socketVector::iterator &it, client &currentClient)
{
    if (listening_socket(portSockets, it) == BREAK)
        return BREAK;
    if (cgi_socket(cgiSockets, buffer, it) == CONTINUE)
        return CONTINUE;
    
    int valRead = recv(it->fd, buffer, BUFF_SIZE - 2, 0);
    if (valRead)
    {
        currentClient.fillBuffer(buffer);
        memset(buffer, 0, BUFF_SIZE);
    }
    if (!valRead)
    {
        deleteConnection(it->fd);
        _sockets.erase(it);
        return BREAK;
    }
    if (valRead < 0)
    {
        std::cout << RED << "Error receiving from client" << RESET_COLOUR << std::endl;
        deleteConnection(it->fd);
        _sockets.erase(it);
        return BREAK;
    }
    return DO_NOTHING;
}

int poller::send_to_cgi_socket(client &currentClient, std::string &response, std::map<int, client*> &cgiSockets)
{
    if (!currentClient.getCgiResponse().empty())
        response = currentClient.getCgiResponse();
    else 
    {
        int             cgiFd = currentClient.getCgiFd();
        struct pollfd   newCgiFd;
        
        newCgiFd.fd     = cgiFd;
        newCgiFd.events = POLLIN;
        cgiSockets.insert(std::make_pair(cgiFd, &currentClient));
        _sockets.insert(_sockets.begin(), newCgiFd);
        return BREAK;
    }
    return DO_NOTHING;
}

int poller::write_to_fd(std::map<int, client*> &cgiSockets, socketVector::iterator &it, client &currentClient)
{
    if (currentClient.isBufferFull())
    {
        currentClient.parseRequestHeader();
        std::string response = currentClient.getResponse();
        if (currentClient.isCgi())
        {
            if (send_to_cgi_socket(currentClient, response, cgiSockets) == BREAK)
                return BREAK;
        }
        int valSent = respondToClient(it->fd, response);
        if (valSent)
        {
            deleteConnection(it->fd);
            _sockets.erase(it);
            return BREAK;
        }
        else if (!valSent)
        {
            deleteConnection(it->fd);
            _sockets.erase(it);
            std::cout << "Nothing more to send" << std::endl;
            return BREAK;
        }
        else if (valSent < 0)
        {
            std::cout << RED << "Error sending to client" << RESET_COLOUR << std::endl;
            deleteConnection(it->fd);
            _sockets.erase(it);
            return BREAK;
        }
        currentClient.resetClient();
    }
    return DO_NOTHING;
}

void poller::pollConnections()
{

    std::set<int>               portSockets = openPorts();
    std::map<int, client*>      cgiSockets;
    char                        *buffer = new char[BUFF_SIZE];
    bzero(buffer, BUFF_SIZE); 
    
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
                int ret = read_from_fd(portSockets, cgiSockets, buffer, it, currentClient);
                if (ret == BREAK)
                    break ;
                else if (ret == CONTINUE)
                    continue;
            }
            else if (it->revents & POLLOUT)
            {
                if (write_to_fd(cgiSockets, it, currentClient) == BREAK)
                    break ;
            }
        }
    }
    delete[] buffer;
}
