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
#include "responseHandler.hpp"

client::client(std::string hostIp, int port, configVector const& configs, int socket)
    : _configs(configs), _hostIp(hostIp), _port(port), _socket(socket) {

    _bodyPresent = false;
    _isBuffFull = false;
    _status = 200;
    _method = 0;
    _bodySize = 0;
}

client::client() {
    (void)_port;
    (void)_socket;
}

client::~client() {}

/* < ---------- Manage Incoming Request Buffer ----------- > */


/* Questions still hang over this implementation when there is a body */
void client::fillBuffer(const char *buff, ssize_t valRead) {
    (void)valRead;

    std::string buffRead(buff);
    _buffer.append(buffRead);
    if (fullHeaderReceived()) {
        std::cout << CYAN << "Even with the body received" << RESET_COLOUR << std::endl;
        _isBuffFull = true; // Check if there's a body or nah
    }
    std::cout << YELLOW << "Buffer:\n" << _buffer << RESET_COLOUR << std::endl;
}

int client::fullHeaderReceived() {
    std::string request(_buffer);
    std::istringstream ss(request);
    std::string line;
//    bool bodyPresent = false;

    if (!_bodyPresent) {
        while (std::getline(ss, line)) {
            std::stringstream stream(line);
            std::string headerElement;
            stream >> headerElement;
            if (!headerElement.compare("Content-Length:")) {
                std::cout << MAGENTA << "The fucker has a body: [" << headerElement << "]" << RESET_COLOUR << std::endl;
                stream >> _bodySize;
                std::cout << MAGENTA << "Body size is: " << _bodySize << RESET_COLOUR << std::endl;
                _bodyPresent = true;
            }

            if (!line.compare("\r")) {
                // _isBuffFull = true   ; Request might have a body so not ready for this
                if (_bodyPresent)
                    break;
                std::cout << MAGENTA << "FULL HEADER SET" << RESET_COLOUR << std::endl;
                return 1;
            }
        }
    }
    while (std::getline(ss, line)) {
        _body.append(line + "\n");
        std::cout << MAGENTA << "Size: " << _body.size() << RESET_COLOUR << std::endl;
        if (_body.size() == (_bodySize + 1)) {
            std::cout << MAGENTA << "Request is completed and it has a body" << RESET_COLOUR << std::endl;
            return 1;
        }

//        if (!line.compare("\n\n")) {
//        }
    }
    return 0;
}

void client::resetClient() {


//    _response.clear();
//    bzero(&_buffer, sizeof(_buffer));
    _buffer.clear();
    _isBuffFull = false;
    _bodyPresent = false;
    _status = 200;
    _bodySize = 0;
}

/* < --------- Request Parsing and Config Routing ------ > */

void client::parseRequestLine(std::string request) {
    std::stringstream ss(request);
    std::string field;

    ss >> field;
    /* 405 Method not allowed */
    if (!field.compare("POST")) {
        _method = POST;
//        std::cout << "Poche seghe, la richiesta POST gl'é: " << _buffer << std::endl;
    }
    else if (!field.compare("DELETE"))
        _method = DELETE;
    else if (!field.compare("GET"))
        _method = GET;
    ss >> _uri;
    if (!_uri.empty() && _uri[0] != '/') {
        // Consider also the possibility of an absolute uri e.g. http://github.com/tstelfox
        _status = 400;
    }
    ss >> _http;
    if (_http.empty() || (!_http.empty() && _http.compare("HTTP/1.1"))) {
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
//    std::cout << BLUE << "This is the culprit isn't it " << RESET_COLOUR << std::endl;
    // nginx demands the host be filled out or it's a 400 bad request
    std::string request(_buffer);
    std::istringstream ss(request);
    std::string line;

    std::getline(ss, line);
    parseRequestLine(line);
    _requestLine = line;

    std::map<std::string, std::string> fields;
//    std::cout << CYAN << "<-------Request line-------->\n" << RESET_COLOUR << \
//            _method << " " << _uri << " " << _http << std::endl << std::endl;

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
            value.resize(value.size() - 2); // Also could just str.replace(", ") or smoething cause this be abit retarded lol
        transform(key.begin(), key.end(), key.begin(), ::tolower);
        fields[key] = value;
    }
//    std::cout << MAGENTA << "<--------Optional Header requests------->" << RESET_COLOUR << std::endl;
//    for (std::map<std::string, std::string>::iterator it = fields.begin(); it != fields.end(); it++)
//        std::cout << "Field: [" << it->first << "] " << "- " << "Value [" << it->second << "]" << std::endl;
//    std::cout << std::endl;
    requestedHost(fields); // If something is invalid in the request line just respond immediately.
    routeConfig(fields);
}

void client::routeConfig(std::map<std::string, std::string> &fields) {
    std::cout << "Must find config matching " << _requestedHost << " in one of" << std::endl;
    for (configVector::iterator iter = _configs.begin(); iter != _configs.end(); iter++) {
        std::cout << "server_name: " << iter->get_server_name() << std::endl;
    }
    // What if there is no server_name? Probs just exit and fuck off. Ask Angie what she's doing when it's left empty
    WSERV::serverConfig  rightConfig;
    WSERV::serverConfig  *namelessConfig = nullptr;
    for (configVector::iterator iter = _configs.begin(); iter != _configs.end(); iter++) {
        if (!_requestedHost.compare(iter->get_server_name())) {
            rightConfig = *iter;
            break;
        }
        if (iter->get_server_name().empty())
            namelessConfig = &(*iter);
        if ((iter + 1) == _configs.end()) {
            if (namelessConfig == nullptr)
                rightConfig = *_configs.begin();
            else
                rightConfig = *namelessConfig;
        }
    }
    std::cout << "This was the right server after all: " << rightConfig.get_server_name() << std::endl;


    responseHandler response(_requestLine, rightConfig, fields);
    _response = response.parseAndRespond(_status, _method, _uri);

//    resetClient();
}

std::string client::getBuffer() const {
    return _buffer;
}

std::string client::getResponse() const {
    return _response;
}

bool client::isBufferFull() const {
    return _isBuffFull;
}