/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   poller.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/02/04 18:38:07 by tmullan       #+#    #+#                 */
/*   Updated: 2022/06/02 11:57:21 by ask           ########   odam.nl         */
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
       
        /* newConnection functions*/
        int             newConnection(int fd);
        void            make_relevant_config_vec(int &newConnection, configVector &relevant, \
                                                    int &port, std::string &hostIp);

        int             newCgiConnection(int fd);
        std::set<int>   openPorts( void );
        void            pair_host_and_port(std::set<std::pair<std::string, int> >  &ports);
        void            create_vector_of_pollfd_sockets(std::set<int> &listenSockets, std::set<std::pair<std::string, int> >  &ports);
        int             respondToClient(int socket, std::string response);
        void            deleteConnection(int fd);

        /* Poll loop functions */
        void            pollConnections( void );
        bool            check_fds_with_poll( void );
        bool            check_if_revents_errors (socketVector::iterator &it);
        int             read_from_fd(std::set<int> &portSockets, std::map<int, client*> &cgiSockets, \
                                    char *buffer, socketVector::iterator &it, client &currentClient);
        int             listening_socket(std::set<int> &portSockets, socketVector::iterator &it);
        int             cgi_socket(std::map<int, client*> &cgiSockets, char *buffer, socketVector::iterator &it);
        int             handle_incoming_message(socketVector::iterator &it, char *buffer, client &currentClient);
        
        typedef enum e_num
        {
            CONTINUE = 0,
            BREAK = 1,
            DO_NOTHING = 2
        }           t_num;

    private:

        /* Vector of the poll structs and events */
        socketVector            _sockets;

       /* Vector of all of the server configurations */
        configVector            _serverConfigs;

        /* Map of all the clients sorted by socket */
        std::map<int, client>   _clients;
};

