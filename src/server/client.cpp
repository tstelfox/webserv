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

    _isChunked = false;
    _chunkSize = 0;
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

void client::fillBuffer(const char *buff, ssize_t valRead) {
    (void)valRead;

    std::string buffRead(buff);
    _buffer.append(buffRead);
//    std::cout << "Lellety lollety" << std::endl;
    if (fullHeaderReceived(buff)) {
        std::cout << CYAN << "Request Received in full" << RESET_COLOUR << std::endl;
        _isBuffFull = true;
    }
//    std::cout << YELLOW << "Buffer:\n" << _buffer << RESET_COLOUR << std::endl;
}

int client::fullHeaderReceived(const char *buff) {
    std::string request(buff);
    std::istringstream ss(request);
    std::string line;

//    std::cout << RED << "Before we begin the buff is: " << buff << RESET_COLOUR << std::endl;

    if (!_bodyPresent && !_isChunked) {
        while (std::getline(ss, line)) {
            std::stringstream stream(line);
            std::string headerElement;
            stream >> headerElement;
            if(!headerElement.compare("Transfer-Encoding:")) {
                stream >> headerElement;
                if (!headerElement.compare("chunked")) {
                    std::cout << MAGENTA << "Chunked diobestia" << RESET_COLOUR << std::endl;
                    _isChunked = true;
                }
            }
            if (!headerElement.compare("Content-Length:")) {
//                std::cout << MAGENTA << "The fucker has a body: [" << headerElement << "]" << RESET_COLOUR << std::endl;
                stream >> _bodySize;
                std::cout << MAGENTA << "Body size is: " << _bodySize << RESET_COLOUR << std::endl;
                _bodyPresent = true;
            }
            if (!line.compare("\r")) {
                // _isBuffFull = true   ; Request might have a body so not ready for this
                if (_bodyPresent || _isChunked) {
                    break;
                }
                std::cout << MAGENTA << "FULL HEADER SET WITHOUT BODY" << RESET_COLOUR << std::endl;
                return 1;
            }
        }
    }
    if (_isChunked) {
//        return chunkedRequest(ss);
        if (_chunk.empty()) {
            std::getline(ss, line);
            std::cout << RED << "Content of the fucking line please: [" << line << "]" << RESET_COLOUR << std::endl;
            if (line.empty())
                return 0;
            std::cout << MAGENTA << "Chunked body bro and this should be the size of the chunk in Hex: " << line
                      << RESET_COLOUR << std::endl;
            std::stringstream sizeStream;
            sizeStream << std::hex << line;
            sizeStream >> _chunkSize;
            if (_chunkSize == 0) {
                std::cout << RED << "Full body built from chunks: " << _body << RESET_COLOUR << std::endl;
                return 1;
            }
            std::cout << MAGENTA << "Chunk size converted to int: " << _chunkSize << RESET_COLOUR << std::endl;
        }
        while (std::getline(ss, line)) {
            _chunk.append(line + "\n");
            if (ss.tellg() == -1) {
                std::cout << "Not in here?" << std::endl;
//                std::cout << GREEN << "Chunk before: [" << line +"\n" << "]" << RESET_COLOUR << std::endl;
                _chunk.resize(_chunk.size() - 1);
//                std::cout << RED << "Chunk after: [" << line << "]" << RESET_COLOUR << std::endl;
            }
            std::cout << "Chunk of size " << _chunkSize << " Line by line: " << line << std::endl;
            std::cout << "size of the chunk: " << _chunk.size() << std::endl;
//            if (_chunkSize < _chunk.size()) // todo debug
//            return 0;
            if (_chunk.size() == _chunkSize) {
//                std::cout << "FULL CHUNK OF SIZE:" << _chunkSize << " RECEIVED:\n" << _chunk << std::endl;
                _body.append(_chunk);
                _chunk.clear();
                std::getline(ss, line);
                std::getline(ss, line);
                if (!line.empty()) {
                    std::stringstream streamy;
                    streamy << line;
                    streamy >> _chunkSize;
                    if (_chunkSize == 0) {
                        std::cout << CYAN << "Reached the end of the chunked body" << RESET_COLOUR << std::endl;
                        return 1;
                    }
                }
//                std::cout << GREEN << "Line after full chunk: " << line << RESET_COLOUR << std::endl;
            }
        }
    }
    else {
        while (std::getline(ss, line)) {
            _body.append(line + "\n");
    //        std::cout << RED << line << RESET_COLOUR << std::endl;
            if (ss.tellg() == -1)
                _body.resize(_body.size() - 1);
    //        std::cout << MAGENTA << "Size of body registered so far: " << _body.size() << RESET_COLOUR << std::endl;
            if (_body.size() == _bodySize) {
    //            std::cout << MAGENTA << "Request is completed and it has a body: " << _body << RESET_COLOUR << std::endl;
                return 1;
            }
        }
//        std::cout << CYAN << _body << RESET_COLOUR << std::endl;
    }
    return 0;
}

int client::chunkedRequest(std::istringstream &ss) {
    std::string line;

    if (_chunk.empty()) {
        std::getline(ss, line);
        std::cout << RED << "Content of the fucking line please: [" << line << "]" << RESET_COLOUR << std::endl;
        if (line.empty())
            return 0;
        std::cout << MAGENTA << "Chunked body bro and this should be the size of the chunk in Hex: " << line
                  << RESET_COLOUR << std::endl;
        std::stringstream sizeStream;
        sizeStream << std::hex << line;
        sizeStream >> _chunkSize;
        if (_chunkSize == 0) {
            std::cout << RED << "Full body built from chunks: " << _body << RESET_COLOUR << std::endl;
            return 1;
        }
        std::cout << MAGENTA << "Chunk size converted to int: " << _chunkSize << RESET_COLOUR << std::endl;
    }
    while (std::getline(ss, line)) {
//            std::cout << "Chunk of size " << _chunkSize << " Line by line: " << line << std::endl;
        _chunk.append(line + "\n");
        if (ss.tellg() == -1)
            _chunk.resize(_chunk.size() - 1);
        if (_chunk.size() == _chunkSize) {
            std::cout << "FULL CHUNK OF SIZE:" << _chunkSize << " RECEIVED:\n" << _chunk << std::endl;
            _body.append(_chunk);
            _chunk.clear();
            std::getline(ss, line);
            std::getline(ss, line);
            if (!line.empty()) {
                std::stringstream streamy;
                streamy << line;
                streamy >> _chunkSize;
                if (_chunkSize == 0) {
                    std::cout << CYAN << "Reached the end of the chunked body" << RESET_COLOUR << std::endl;
                    return 1;
                }
            }
//                std::cout << GREEN << "Line after full chunk: " << line << RESET_COLOUR << std::endl;
        }
    }
    return 0;
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
        _status = 400;
    }
    ss >> _http;
    if (_http.empty() || (!_http.empty() && _http.compare("HTTP/1.1"))) {
        _status = 505; // HTTP VERSION NOT SUPPORTED
    }
    if (_status != 200) {
        //Ya know the drill todo
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
    if (fields["host"].empty() || !fields["host"].compare(" ")) {
        std::cout << "No host" << std::endl;
        _status = 400;
    } else
        _requestedHost = fields["host"];

    if (fields.count("content-length"))
        if (fields["content-type"].compare("text/plain") && fields["content-type"].compare("text/html"))
            _status = 415;
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
    std::cout << std::endl;
    requestedHost(fields);
    routeConfig(fields);
}

void client::routeConfig(std::map<std::string, std::string> &fields) {
//    std::cout << "Must find config matching " << _requestedHost << " in one of" << std::endl;
//    for (configVector::iterator iter = _configs.begin(); iter != _configs.end(); iter++) {
////        std::cout << "server_name: " << iter->get_server_name() << std::endl;
//    }
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


    responseHandler response(_requestLine, rightConfig, fields, _body);
    _response = response.parseAndRespond(_status, _method, _uri);

}

/* < --------- GENERAL UTILS ------ > */

std::string client::getBuffer() const {
    return _buffer;
}

std::string client::getResponse() const {
    return _response;
}

bool client::isBufferFull() const {
    return _isBuffFull;
}

void client::resetClient() {


//    _response.clear();
//    bzero(&_buffer, sizeof(_buffer));
    _buffer.clear();
    _isChunked = false;
    _chunk.clear();
    _chunkSize = 0;
    _isBuffFull = false;
    _bodyPresent = false;
    _status = 200;
    _bodySize = 0;
}
