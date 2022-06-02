/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   poller_newConnection.cpp                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: ask <ask@student.codam.nl>                   +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/06/02 18:33:32 by ask           #+#    #+#                 */
/*   Updated: 2022/06/02 18:37:05 by ask           ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "poller.hpp"

void poller::setPollFd(int fd, short events)
{
    struct pollfd newPollFd;
    
    newPollFd.fd        = fd;
    newPollFd.events    = events;

    _sockets.push_back(newPollFd);
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