/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   serverBlock.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/02/04 18:38:07 by tmullan       #+#    #+#                 */
/*   Updated: 2022/04/21 17:50:28 by tmullan       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */


#ifndef SERVERBLOCK_HPP
#define SERVERBLOCK_HPP
#include "src/sockets/socket.hpp"

class serverBlock {

public:
    /*One of the following tbd*/
//    serverBlock(serverSock &listeningSocket);
    serverBlock(int port);

    ~serverBlock();

    int getSockFd() const;


private:
    serverBlock();

    /*Server Block specifications*/
    int _port; // And what about host?

//    std::string _serverName;
//    std::string _rootDirectory;
//
//    bool _isDirListOn;
//
//    std::vector <std::string> _acceptedMethods;

    /*Actual Socket or Socket fd*/
    serverSock *_socket;
    int _socketFd;


};

#endif //WEBSERV_SERVERBLOCK_HPP
