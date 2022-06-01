/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   poller.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/02/04 18:38:07 by tmullan       #+#    #+#                 */
/*   Updated: 2022/04/22 13:38:54 by tmullan       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "socket.hpp"
#include "client.hpp"
//#include "clientConnecter.hpp"
#include "serverConfig.hpp"
#include <string>
#include <set>

class poller
{

    public:

        typedef std::vector<struct pollfd> socketVector;
        typedef std::vector<WSERV::serverConfig> configVector;

        poller(configVector const &serverBlocks);
        ~poller();

        void            setPollFd(int fd, short events);
        int             connectionError(short revents) const;
        int             newConnection(int fd);
        int             newCgiConnection(int fd);
        std::set<int>   openPorts( void );
        void            pair_host_and_port(std::set<std::pair<std::string, int> >  &ports);
        void            create_vector_of_pollfd_sockets(std::set<int> &listenSockets, std::set<std::pair<std::string, int> >  &ports);
        int             respondToClient(int socket, std::string response);
        void            pollConnections( void );
        void            deleteConnection(int fd);

    private:

        /* Vector of the poll structs and events */
        socketVector            _sockets;

       /* Vector of all of the server configurations */
        configVector            _serverConfigs;

        /* Map of all the clients sorted by socket */
        std::map<int, client>   _clients;
};

