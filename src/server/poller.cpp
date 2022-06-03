/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   poller.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/02/04 18:59:58 by tmullan       #+#    #+#                 */
/*   Updated: 2022/06/02 18:42:42 by ask           ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "poller.hpp"

poller::poller(configVector const& configVector) : _serverConfigs(configVector) 
{
    pollConnections();
}

poller::~poller() {}

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
