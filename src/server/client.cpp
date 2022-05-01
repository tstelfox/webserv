/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   client.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/01 15:35:44 by tmullan       #+#    #+#                 */
/*   Updated: 2022/05/01 15:35:44 by tmullan       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "client.hpp"
#include <iostream>

client::client(std::string hostIp, int port, configVector const& configs, int socket)
    : _configs(configs), _hostIp(hostIp), _port(port), _socket(socket) {
    std::cout << "Client Class created with config:\n" << std::endl;
    for (configVector::iterator it = _configs.begin(); it != _configs.end(); it++)
        std::cout << "Servername: " << it->get_server_name() << std::endl;
}

client::client() {
    /* Just for now */
    (void)_port;
    (void)_buffer;
    (void)_buffSize;
    (void)_isBuffFull;
    (void)_method;
    (void)_keepAlive;
    (void)_status;
    (void)_socket;
}

client::~client() {}

void client::fillBuffer(const char *buff, ssize_t valRead) {
    int temp = _buffSize;
    _buffSize += valRead;
    for (int i = 0; i < valRead; i++) {
        _buffer[temp] = buff[i];
        temp++;
    }
    _buffer[temp] = '\0';
    std::cout << _buffer << std::endl;
}

char *client::getBuffer() {
    return _buffer;
}