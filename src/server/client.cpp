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
#include <sstream>

client::client(std::string hostIp, int port, configVector const& configs, int socket)
    : _configs(configs), _hostIp(hostIp), _port(port), _socket(socket) {

    memset(_buffer, 0, 1024);
    _buffSize = 0;
    _isBuffFull = false;
//    _status = 200;
//    _method = 0;
}

client::client() {
    /* Just for now */
    (void)_port;
//    (void)_buffer;
//    (void)_buffSize;
//    (void)_isBuffFull;
//    (void)_method;
//    (void)_keepAlive;
//    (void)_status;
    (void)_socket;
}

client::~client() {}

/* < ---------- Manage Incoming Request Buffer ----------- > */


/* Questions still hang over this implementation when there is a body */
void client::fillBuffer(const char *buff, ssize_t valRead) {
    int temp = _buffSize;
    _buffSize += valRead;
    for (int i = 0; i < valRead; i++) {
        _buffer[temp] = buff[i];
        temp++;
    }
    _buffer[temp] = '\0';
    if (fullHeaderReceived())
        _isBuffFull = true; // Check if there's a body or nah
}

int client::fullHeaderReceived() {
    std::string request(_buffer);
    std::istringstream ss(request);
    std::string line;
    while (std::getline(ss, line)) {
        if (!line.compare("\r")) {
            // _isBuffFull = true   ; Request might have a body so not ready for this
            return 1;
        }
    }
    return 0;
}

//void client::resetClient() { // TODO
//    /* Is it necessary to wipe it this way
//     * or can I just delete the client entirely?
//     * Perhaps this is in order to receive the favicon too
//     */
//}

/* < --------- Parse Received Request ------ > */




char *client::getBuffer() {
    return _buffer;
}

bool client::isBufferFull() const {
    return _isBuffFull;
}