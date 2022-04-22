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

class serverBlock {

public:
    serverBlock(serverSock &listeningSocket);

    ~serverBlock();


private:
    serverBlock();

    /*Server Block specifications*/
    int _port;
    std::string _serverName;

    /*Actual Socket or Socket fd*/
//    serverSock *_socket;
//    int _socketFd;


};

#endif //WEBSERV_SERVERBLOCK_HPP
