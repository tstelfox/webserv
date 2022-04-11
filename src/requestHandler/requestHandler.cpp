/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   requestHandler.cpp                                 :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/03/25 19:06:20 by tmullan       #+#    #+#                 */
/*   Updated: 2022/04/11 16:55:11 by tmullan       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "requestHandler.hpp"
#include <iostream>
#include <sstream>
#include <fstream>
#include <map>
#include <algorithm>

requestHandler::requestHandler(int fd) : _clientFd(fd), _buffSize(0), _fullBuffer(false), _method(0), 
				_keepAlive(true), _status(200) {
	memset(_buffer, 0, 1024);
	// std::cout << "Request for client no fd: [" << _clientFd << "] created." << std::endl;
	(void)_keepAlive;
	(void)_method;
}

requestHandler::requestHandler() {}

// void	requestHandler::setFd(int fd) {
// 	_clientFd = fd;
// }

int		requestHandler::getFd() const {
	return this->_clientFd;
}

std::string	requestHandler::getResponse() const {
	return this->_response;
}

void	requestHandler::fillBuffer(char *buff, int valread) {
	int temp = _buffSize;
	_buffSize += valread;
	for (int i = 0; i < valread; i++) {
		_buffer[temp] = buff[i];
		temp++;
	}
	_buffer[temp] = '\0';
}

	/* Quickly check for the double \n\n delimiter of header
	and then parse it after that */

void	requestHandler::requestLine(std::string request) {
	std::stringstream	ss(request);
	std::string			field;
	
	ss >> field;
	if (!field.compare("POST"))
		_method = POST;
	else if (!field.compare("DELETE"))
		_method = DELETE;
	else if (!field.compare("GET"))
		_method = GET;
	else
		_status = 400; // 400 BAD REQUEST
	ss >> _uri;
	if (_uri[0] != '/') // This'll segfault if there's nothign there of course PLUS so much other shit
		_status = 400; // BAD REQUEST
	// std::cout << "_uri is: [" << _uri << "]" << std::endl;
	ss >> _httpVersion;
	if (_httpVersion.compare("HTTP/1.1"))
		_status = 505; // HTTP VERSION NOT SUPPORTED
	// std::cout << "http version is: [" << _httpVersion << "]" << std::endl;
}

void	requestHandler::requestFields(std::map<std::string, std::string> fields) {
	(void)fields;
}

void	requestHandler::parseRequest() {
	std::string request(_buffer);
	std::istringstream	ss(request);
	std::string	line;
	
	std::getline(ss, line);
	requestLine(line);
	if (_status != 200) {
		formulateResponse();
		return ;
	}
	std::map<std::string, std::string>	fields;
	while (std::getline(ss, line)) {
		if (!line.compare("\r")) {
			std::cout << "You have reached the end of the header" << std::endl;
			break;
		}
		std::replace(line.begin(), line.end(), ':', ' ');
		std::stringstream stream(line);
		std::string key;
		stream >> key;
		std::string	value;
		stream >> value;
		transform(key.begin(), key.end(), key.begin(), ::tolower);
		fields[key] = value;
	}
	for (std::map<std::string, std::string>::iterator it = fields.begin(); it != fields.end(); it++)
		std::cout << "Field: [" << it->first <<"] " << "- " << "Value [" << it->second << "]" << std::endl;
	requestFields(fields);
	// std::string word;
	
	// ss >> word;
	// if (!word.compare("POST"))
	// 	_method = POST;
	// else if (!word.compare("DELETE"))
	// 	_method = DELETE;
	// else if (!word.compare("GET"))
	// 	_method = GET;
	// else
	// 	_status = 400; // 400 BAD REQUEST
	// ss >> _uri;
	// if (_uri[0] != '/') // This'll segfault if there's nothign there of course PLUS so much other shit
	// 	_status = 400; // BAD REQUEST
	// // std::cout << "_uri is: [" << _uri << "]" << std::endl;
	// ss >> _httpVersion;
	// if (_httpVersion.compare("HTTP/1.1"))
	// 	_status = 505; // HTTP VERSION NOT SUPPORTED
	// // std::cout << "http version is: [" << _httpVersion << "]" << std::endl;

	// ss >> _host;
	// std::string hostname;
	// if (_host.compare("Host:")) { // Actually it's case insensitive cause of course it is
	// 	_status = 400; // BAD REQUST
	// }
	// ss >> _host; // Gotta check that there's actually some content here before the newline
	// // _host += " " + hostname; // If there is no Host --- 400 BAD REQUEST
	// // _host = hostname
	// // std::cout << "host is: [" << _host << "]" << std::endl;


	// /* When we're done here with the Parsing */
	// formulateResponse();

}

void	requestHandler::formulateResponse() {
	// std::cout << "Yes this is the response hi on fd: " << _clientFd << std::endl;

	if (_status != 200) {
		// Error responses and sheet
	}
	if (_method == GET) { // Function for get responses
		std::string requestedFile("pages");
		if (!_uri.compare("/")) {
			_uri += "index.html";
		}
		requestedFile += _uri;
		std::ifstream myfile;
		myfile.open(requestedFile); // Protect it later
		std::ostringstream fileContent;
		fileContent << myfile.rdbuf();
		_response = fileContent.str();

		std::string	header = "HTTP/1.1 200 OK\nContent-Type: text/html; charset=UTF-8\nContent-Length:";
		int len = _response.size();
		header.append(std::to_string(len));
		header.append("\n\n");
		header.append(_response);

		_response = header;
		// std::cout << "Response is gonna be [" << _response << "]" << std::endl;
	}
	// Gonna need the fd in order to send the response.
}

char*	requestHandler::getBuffer() {
	return _buffer;
}

void	requestHandler::bufferIsFull() {
	_fullBuffer = true;
}

bool	requestHandler::getFullState() const {
	return _fullBuffer;
}

	// is this really the best way to do this?
void	requestHandler::resetHandler() {
	// std::cout << "Called how many times?" << std::endl;
	memset(_buffer, 0, sizeof(_buffer));
	_response.clear();
	_status = 200;
	_host.clear();
	_method = EMPTY;
	_httpVersion.clear();
	_fullBuffer = false;
}

// requestHandler::requestHandler(char *buffer) : _keepAlive(true), _method(0) {
	
// 	_requestString = std::string(buffer);
// }

requestHandler::~requestHandler() {}

