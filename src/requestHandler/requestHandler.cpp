/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   requestHandler.cpp                                 :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/03/25 19:06:20 by tmullan       #+#    #+#                 */
/*   Updated: 2022/03/30 13:57:02 by tmullan       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "requestHandler.hpp"
#include <iostream>
// #include <string>
#include <sstream>

requestHandler::requestHandler() : _buffSize(0), _fullBuffer(false), _keepAlive(true), _method(0) {
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
		// Possibly call a function here to do the rest of the parsing but also nah
		// It may be best to do the generic parsing first though
		std::cout << "GET method" << std::endl;
	}
	ss >> _uri;
	// _uri = word;
	std::cout << "_uri is: [" << _uri << "]" << std::endl;
	// I think if there is no specific method then it's a GET
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

