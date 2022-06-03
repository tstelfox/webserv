/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   client.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/01 15:35:44 by tmullan       #+#    #+#                 */
/*   Updated: 2022/06/02 17:04:07 by ask           ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "client.hpp"
#include <iostream>
#include <sstream>
#include "colours.hpp"
#include "responseHandler.hpp"

client::client(std::string hostIp, int port, configVector const& configs, int socket)
    : _configs(configs), _hostIp(hostIp), _port(port), _socket(socket), _isCgi(false)
{
    _isChunked      = false;
    _chunkSize      = 0;
    _bodyPresent    = false;
    _isBuffFull     = false;
    _status         = 200;
    _method         = 0;
    _bodySize       = 0;
    _cgiFd          = -1;
}

client::client()
{
    (void)_port;
    (void)_socket;
}

client::~client() {}

/* < ---------- Manage Incoming Request Buffer ----------- > */

void client::fillBuffer(const char *buff)
{
    std::string buffRead(buff);

    _buffer.append(buffRead);
    if (fullHeaderReceived(buff))
    {
        std::cout << CYAN << "Request Received in full" << RESET_COLOUR << std::endl;
        _isBuffFull = true;
    }
}

int client::empty_body_header_handler(std::istringstream  &ss, std::string &line, bool &onlyBody)
{
    while (std::getline(ss, line))
    {
        std::stringstream   stream(line);
        std::string         headerElement;

        stream >> headerElement;

        if(!headerElement.compare("Transfer-Encoding:"))
        {
            stream >> headerElement;
            
            if (!headerElement.compare("chunked"))
            {
                std::cout << MAGENTA << "Chunked diobestia" << RESET_COLOUR << std::endl;
                _isChunked = true;
            }
        }
        if (!headerElement.compare("Content-Length:"))
        {
            stream >> _bodySize;

            std::cout << MAGENTA << "Body size is: " << _bodySize << RESET_COLOUR << std::endl;
            _bodyPresent = true;
        }
        if (!line.compare("\r"))
        {
            onlyBody = false;
            if (_bodyPresent || _isChunked)
                break;
            std::cout << MAGENTA << "FULL HEADER SET WITHOUT BODY" << RESET_COLOUR << std::endl;
            return 1;
        }
    }
    return 0;
}

int client::append_to_body(std::istringstream  &ss, std::string &line)
{
    while (std::getline(ss, line))
    {
        _body.append(line + "\n");
        if (ss.tellg() == -1)
            _body.resize(_body.size() - 1);
        if (_body.size() == _bodySize)
            return 1;
    }
    return 0;
}

int client::fullHeaderReceived(const char *buff)
{
    std::string         request(buff);
    std::istringstream  ss(request);
    std::string         line;
    bool                onlyBody = true;

    if (!_bodyPresent && !_isChunked)
    {
        if (empty_body_header_handler(ss, line, onlyBody) == 1)
            return 1;
    }
    if (_isChunked)
        return chunkedRequest(request, onlyBody);
    else 
    {
       if (append_to_body(ss, line) == 1)
        return 1;
    }
    return 0;
}

static int remove_header(std::string &buffer)
{
    size_t pos = buffer.find("\r\n\r\n"); //remove header

    buffer.erase(0, pos + 4);
    if (buffer.empty())
        return 0;
    std::cout << GREEN << "Correctly removed header? [" << buffer << "]" << RESET_COLOUR << std::endl;
    return 1;
}

void client::update_buffers(std::string &buffer, std::string &chunkLenStr)
{
    buffer.erase(0, chunkLenStr.size() + 2);        //erase from buffer
    int missingSize = _chunkSize - _chunk.size();   //update size
    _chunk.append(buffer, 0, missingSize);          //append to chunkbuffer
    _body.append(_chunk);                           //append chunk to body
    _chunk.clear();                                 //clear chunk buffer
    _chunkSize = 0;                                 //reset size
    buffer.erase(0, missingSize + 2);
}

int client::chunked_request_handler(std::string &buffer, std::string &chunkLenStr, std::stringstream &sizeStream, \
                                    std::stringstream &ss)
{
    ss.str(buffer);
        
    ss >> chunkLenStr;
    sizeStream << std::hex << chunkLenStr;
    sizeStream >> _chunkSize;

    if (_chunkSize == 0) //all_chunks_received
    {
        std::cout << "All chunks received" << std::endl;
        return 1;
    }
    if (_chunkSize > BUFF_SIZE) //invalid chunked request
    {
        std::cout << "Invalid chunked request larger than buffer size" << std::endl;
        _status = 400;
        return 1;
    }
    update_buffers(buffer, chunkLenStr);
    if (buffer == "0\r\n\r\n") //at the end of chunked request
    {
        std::cout << "All chunks received" << std::endl;
        std::cout << COLOR_PINK << "Full body de-chunked:\n" << _body << std::endl;
        return 1;
    }
    return 0;
}

int client::chunkedRequest(std::string buffer, bool onlyBody)
{
    std::string         chunkLenStr;
    std::stringstream   sizeStream;
    std::stringstream   ss;

    if (!onlyBody)
    {
        if (remove_header(buffer) == 0)
            return 0;
    }
    if (!_chunkSize)
    {
        if (chunked_request_handler(buffer, chunkLenStr, sizeStream, ss) == 1)
            return 1;
    }
    return 0;
}

/* < --------- Request Parsing and Config Routing ------ > */

void client::parseRequestLine(std::string request)
{
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

    if (_http.empty() || (!_http.empty() && _http.compare("HTTP/1.1")))
        _status = 505; // HTTP VERSION NOT SUPPORTED
}

void client::requestedHost(std::map<std::string, std::string> &fields)
{
    /* Check that there is a valid Host */
    std::map<std::string, std::string>::iterator it;
    it = fields.find("host");

    if (it == fields.end())
    {
        _status = 400;
        return;
    }
    if (fields["host"].empty() || !fields["host"].compare(" "))
    {
        std::cout << "No host" << std::endl;
        _status = 400;
    } 
    else
        _requestedHost = fields["host"];

    if (fields.count("content-length"))
        if (fields["content-type"].compare("text/plain") && fields["content-type"].compare("text/html"))
            _status = 415;
}

void client::parseRequestHeader()
{
    std::string request(_buffer);
    std::istringstream ss(request);
    std::string line;

    std::getline(ss, line);
    parseRequestLine(line);
    _requestLine = line;

    std::map<std::string, std::string> fields;

    while (std::getline(ss, line))
    {
        if (!line.compare("\r"))
            break;
        std::replace(line.begin(), line.end(), ':', ' ');
        std::stringstream stream(line);
        std::string key;

        stream >> key;

        std::string value;
        while (stream)
        {
            std::string rcodio;
            stream >> rcodio;
            value += rcodio + " ";
        }
        if (value.size() > 2)
            value.resize(value.size() - 2);
        transform(key.begin(), key.end(), key.begin(), ::tolower);
        fields[key] = value;
    }
    requestedHost(fields);
    routeConfig(fields);
}

void client::routeConfig(std::map<std::string, std::string> &fields)
{
    WSERV::serverConfig  rightConfig;
    WSERV::serverConfig  *namelessConfig = nullptr;
    
    for (configVector::iterator iter = _configs.begin(); iter != _configs.end(); iter++)
    {
        if (!_requestedHost.compare(iter->get_server_name()))
        {
            rightConfig = *iter;
            break;
        }
        if (iter->get_server_name().empty())
            namelessConfig = &(*iter);
        if ((iter + 1) == _configs.end())
        {
            if (namelessConfig == nullptr)
                rightConfig = *_configs.begin();
            else
                rightConfig = *namelessConfig;
        }
    }
    
    std::cout << "This was the right server after all: " << rightConfig.get_server_name() << std::endl;

    responseHandler response(_requestLine, rightConfig, fields, _body);
    _response = response.parseAndRespond(_status, _method, _uri);
    if (response.isCgiResponse())
    {
        _isCgi = true;
        _cgiFd = response.getCgiFd();
        std::cout << CYAN << "So cgi fd now in client is: " << _cgiFd << RESET_COLOUR << std::endl;
    }

}

/* < --------- GENERAL UTILS ------ > */

void client::resetClient()
{
    _cgiResponse.clear();
    _isCgi          = false;
    _cgiFd          = -1;
    _buffer.clear();
    _isChunked      = false;
    _chunk.clear();
    _chunkSize      = 0;
    _isBuffFull     = false;
    _bodyPresent    = false;
    _status         = 200;
    _bodySize       = 0;
}

void    client::saveCgiResponse(char *buffer)
{
    std::string str(buffer);
    _cgiResponse = str;
}
