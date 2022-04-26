/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   serverBlock.cpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/04/22 14:05:39 by tmullan       #+#    #+#                 */
/*   Updated: 2022/04/22 14:05:39 by tmullan       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "serverBlock.hpp"
#include <iostream>

serverBlock::serverBlock() {}

serverBlock::serverBlock(int port) : _port(port) {
    serverSock *newSock = new serverSock(AF_INET, SOCK_STREAM, 0, _port, INADDR_ANY);
    _socket = newSock;
    _socketFd = _socket->getSock();
    std::cout << "New socket created: " << _socketFd << std::endl;
}

serverBlock::~serverBlock() {}

int serverBlock::getSockFd() const {
    return _socketFd;
}
