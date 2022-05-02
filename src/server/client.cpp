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
#include "colours.hpp"

client::client(std::string hostIp, int port, configVector const& configs, int socket)
    : _configs(configs), _hostIp(hostIp), _port(port), _socket(socket) {

    memset(_buffer, 0, 1024);
    _buffSize = 0;
    _isBuffFull = false;
    _status = 200;
    _method = 0;
}

client::client() {
    (void)_port;
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

/* < --------- Request Parsing and Config Routing ------ > */

void client::parseRequestLine(std::string request) {
    std::stringstream ss(request);
    std::string field;

    ss >> field;
    /* 405 Method not allowed will be about the config allowed methods
    Otherwise all others are 400 Bad Request */
    if (!field.compare("POST"))
        _method = POST;
    else if (!field.compare("DELETE"))
        _method = DELETE;
    else if (!field.compare("GET"))
        _method = GET;
    else
        _status = 400; // BAD REQUEST
    // else if (std::isupper(field[0])) {
    // 	_status = 405; // Method Not Allowed
    // }
    ss >> _uri;
    if (!_uri.empty() && _uri[0] != '/') {
        // Consider also the possibility of an absolute uri e.g. http://github.com/tstelfox
        _status = 400;
    }
    ss >> _http;
    if (!_http.empty() && _http.compare("HTTP/1.1")) {
        _status = 505; // HTTP VERSION NOT SUPPORTED
    }
    if (_status != 200) {
        //Ya know the drill
    }
}

void client::requestedHost(std::map<std::string, std::string> &fields) {

    /* Check that there is a valid Host */
    std::map<std::string, std::string>::iterator it;
    it = fields.find("host");
    if (it == fields.end()) {
        _status = 400;
        return;
    }
    /* Test the following with no host
        It may be that if the Uri is an absolute path then
        host can actually be omitted (?) */
    if (fields["host"].empty() || !fields["host"].compare(" ")) {
        std::cout << "No host" << std::endl;
        _status = 400;
    } else
        _requestedHost = fields["host"];
}

void client::parseRequestHeader() {
    // nginx demands the host be filled out or it's a 400 bad request
    std::string request(_buffer);
    std::istringstream ss(request);
    std::string line;

    std::getline(ss, line);
    parseRequestLine(line); // If something is invalid in the request line just respond immediately.

    std::map<std::string, std::string> fields;
    std::cout << CYAN << "<-------Request line-------->\n" << RESET_COLOUR << \
            _method << " " << _uri << " " << _http << std::endl << std::endl;

    while (std::getline(ss, line)) {
        if (!line.compare("\r"))
            break;
        std::replace(line.begin(), line.end(), ':', ' ');
        std::stringstream stream(line);
        std::string key;
        stream >> key;
        std::string value;
        while (stream) {
            std::string rcodio;
            stream >> rcodio;
            value += rcodio + " ";
        }
        if (value.size() > 2)
            value.resize(
                    value.size() - 2); // Also could just str.replace(", ") or smoething cause this be abit retarded lol
        transform(key.begin(), key.end(), key.begin(), ::tolower);
        fields[key] = value;
    }
    std::cout << MAGENTA << "<--------Optional Header requests------->" << RESET_COLOUR << std::endl;
    for (std::map<std::string, std::string>::iterator it = fields.begin(); it != fields.end(); it++)
        std::cout << "Field: [" << it->first << "] " << "- " << "Value [" << it->second << "]" << std::endl;
    std::cout << std::endl;
    requestedHost(fields); // If something is invalid in the request line just respond immediately.
    routeConfig();
}

void client::routeConfig() {
    std::cout << "Must find config matching " << _requestedHost << " in one of" << std::endl;
    for (configVector::iterator iter = _configs.begin(); iter != _configs.end(); iter++) {
        std::cout << "server_name: " << iter->get_server_name() << std::endl;
    }

    // What if there is no server_name? Probs just exit and fuck off. Ask Angie what she's doing when it's left empty

    WSERV::serverConfig  rightConfig;
    for (configVector::iterator iter = _configs.begin(); iter != _configs.end(); iter++) {
        if (!_requestedHost.compare(iter->get_server_name())) {
            rightConfig = *iter;
            break;
        }
        if ((iter + 1) == _configs.end())
            rightConfig = *_configs.begin();
    }
    std::cout << "This was the right server after all: " << rightConfig.get_server_name() << std::endl;


}

char *client::getBuffer() {
    return _buffer;
}

bool client::isBufferFull() const {
    return _isBuffFull;
}