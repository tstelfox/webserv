/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   poller_utils.cpp                                   :+:    :+:            */
/*                                                     +:+                    */
/*   By: ask <ask@student.codam.nl>                   +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/06/02 18:38:44 by ask           #+#    #+#                 */
/*   Updated: 2022/06/02 18:42:35 by ask           ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "poller.hpp"

void poller::deleteConnection(int fd) 
{
    std::map<int, client>::iterator clientIt = _clients.find(fd);
    
    if (clientIt != _clients.end())
    {
        _clients.erase(clientIt);
        close(fd);
    }
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

int poller::respondToClient(int socket, std::string response)
{

    char toSend[response.length() + 1];
    std::strcpy(toSend, response.c_str());

    int ret = send(socket, toSend, strlen(toSend), 0);
    return ret;
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