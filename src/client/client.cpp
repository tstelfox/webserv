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
    bool onlyBody = true;

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
                onlyBody = false;
                if (_bodyPresent || _isChunked) {
                    break;
                }
                std::cout << MAGENTA << "FULL HEADER SET WITHOUT BODY" << RESET_COLOUR << std::endl;
                return 1;
            }
        }
    }
    if (_isChunked)
        return chunkedRequest(request, onlyBody);
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

int client::chunkedRequest(std::string buffer, bool onlyBody) {
    std::string chunkLenStr;
    std::stringstream sizeStream;
    std::stringstream ss;

    if (!onlyBody) {
        size_t pos = buffer.find("\r\n\r\n");
        buffer.erase(0, pos + 4);
        if (buffer.empty())
            return 0;
        std::cout << GREEN << "Correctly removed header? [" << buffer << "]" << RESET_COLOUR << std::endl;
    }
    if (!_chunkSize) {
        ss.str(buffer);
        ss >> chunkLenStr;
        sizeStream << std::hex << chunkLenStr;
        sizeStream >> _chunkSize;
        if (_chunkSize == 0){
            std::cout << "All chunks received" << std::endl;
            return 1;
        }
        if (_chunkSize > BUFF_SIZE) {
            std::cout << "Invalid chunked request larger than buffer size" << std::endl;
            _status = 400;
            return 1;
        }
        buffer.erase(0, chunkLenStr.size() + 2);
//        std::cout << YELLOW << "The size of this new chunk is: " << _chunkSize << RESET_COLOUR << std::endl;
        int missingSize = _chunkSize - _chunk.size();
//        std::cout << MAGENTA << "Missing size is: " << missingSize << RESET_COLOUR << std::endl;
        _chunk.append(buffer, 0, missingSize);
        _body.append(_chunk);
        _chunk.clear();
        _chunkSize = 0;
        buffer.erase(0, missingSize + 2);
        if (buffer == "0\r\n\r\n") {
            std::cout << "All chunks received" << std::endl;
            std::cout << COLOR_PINK << "Full body de-chunked:\n" << _body << std::endl;
            return 1;
        }
//        std::cout << "Chunk at end" << _chunk << std::endl;
//        std::cout << "Buffer at the end [" << buffer << "]"<< std::endl;
    }
    return 0;
}

/* < --------- Request Parsing and Config Routing ------ > */

void client::parseRequestLine(std::string request) {
    std::stringstream ss(request);
    std::string field;

    ss >> field;
    /* 405 Method not allowed */
    if (!field.compare("POST"))
        _method = POST;
    else if (!field.compare("DELETE"))
        _method = DELETE;
    else if (!field.compare("GET"))
        _method = GET;
    ss >> _uri;
    if (!_uri.empty() && _uri[0] != '/')
        _status = 400;
    ss >> _http;
    if (_http.empty() || (!_http.empty() && _http.compare("HTTP/1.1"))) {
        _status = 505; // HTTP VERSION NOT SUPPORTED
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
//    std::cout << std::endl;
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

int     client::getSocket() const {
    return _socket;
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
