/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   requestHandler.cpp                                 :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/03/25 19:06:20 by tmullan       #+#    #+#                 */
/*   Updated: 2022/03/30 17:48:10 by tmullan       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "requestHandler.hpp"
#include <iostream>
// #include <string>
#include <sstream>

requestHandler::requestHandler() : _buffSize(0), _fullBuffer(false), _method(0), 
				_keepAlive(true), _status(200) {
	memset(_buffer, 0, 1024);
	(void)_keepAlive;
	(void)_method;
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

void	requestHandler::parseRequest() {
	std::string request;
	request = _buffer;
	std::cout << "Parsing the following request:\n" << request << std::endl;
	std::stringstream	ss(request);
	std::string word;
	ss >> word;
	if (!word.compare("POST")) {
		_method = POST;
		std::cout << "POST method" << std::endl;
	}
	else if (!word.compare("DELETE")) {
		_method = DELETE;
		std::cout << "DELETE method" << std::endl;
	}
	else if (!word.compare("GET")) {
		_method = GET;
		std::cout << "GET method" << std::endl;
	}
	// else {
	// 	// 400 BAD REQUEST
	// }
	ss >> _uri;
	if (_uri[0] != '/') // This'll segfault if there's nothign there of course
		_status = 400; // BAD REQUEST
	std::cout << "_uri is: [" << _uri << "]" << std::endl;
	ss >> _httpVersion;
	if (_httpVersion.compare("HTTP/1.1"))
		_status = 505; // HTTP VERSION NOT SUPPORTED
	std::cout << "http version is: [" << _httpVersion << "]" << std::endl;

	ss >> _host;
	std::string hostname;
	if (_host.compare("Host:")) { // Actually it's case insensitive cause of course it is
		_status = 400; // BAD REQUST
	}
	ss >> _host; // Gotta check that there's actually some content here before the newline
	// _host += " " + hostname; // If there is no Host --- 400 BAD REQUEST
	// _host = hostname
	std::cout << "host is: [" << _host << "]" << std::endl;
}

char*	requestHandler::getBuffer() {
	return _buffer;
}

void	requestHandler::bufferIsFull() {
	_fullBuffer = true;
}

bool	requestHandler::getFullState() {
	return _fullBuffer;
}

// requestHandler::requestHandler(char *buffer) : _keepAlive(true), _method(0) {
	
// 	_requestString = std::string(buffer);
// }

requestHandler::~requestHandler() {}

