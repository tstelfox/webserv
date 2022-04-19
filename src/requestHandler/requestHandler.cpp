/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   requestHandler.cpp                                 :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/03/25 19:06:20 by tmullan       #+#    #+#                 */
/*   Updated: 2022/04/19 12:07:06 by tmullan       ########   odam.nl         */
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
	(void)_keepAlive;
	(void)_method;
}

requestHandler::requestHandler() {}

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
	else if (!std::isupper(field[0]))
		_status = 501; // Method Not Implemented
	else
		_status = 400; // BAD REQUEST
	ss >> _uri;
	if (_uri[0] != '/') // This'll segfault if there's nothign there of course PLUS so much other shit
		_status = 400; // BAD REQUEST
	// std::cout << "_uri is: [" << _uri << "]" << std::endl;
	ss >> _httpVersion;
	if (!_httpVersion.empty() && _httpVersion.compare("HTTP/1.1")) {
		_status = 505; // HTTP VERSION NOT SUPPORTED
	}
}

void	requestHandler::requestFields(std::map<std::string, std::string> fields) {
	
		/* Check that there is a valid Host */
	std::map<std::string, std::string>::iterator	it;
	it = fields.find("host");
	if (it == fields.end()) {
		_status = 400;
		formulateResponse();
		return ;
	}
		/* Test the following with no host
			It may be that if the Uri is an absolute path then
			host can actually be omitted (?) */
	if (fields["host"].empty()) {
		std::cout << "No host" << std::endl;
		_status = 400;
	}
	else
		_host = fields["host"];
	formulateResponse();
}

int		requestHandler::fullHeaderReceived() {
	std::string request(_buffer);
	std::istringstream ss(request);
	std::string line;
	while (std::getline(ss, line)) {
		if(!line.compare("\r")) {
			// setBufferAsFull(); Request might have a body so not ready for this
			std::cout << "It's full" << std::endl;
			return 1;
		}
	}
	return 0;
}

void	requestHandler::parseRequest() {
	std::string request(_buffer);
	std::istringstream	ss(request);
	std::string	line;

	std::cout << "Parse another thing:\n" << _buffer << std::endl;
	
	// Need to work out how to not parse the first line like this if we haven't got the complete header
	if (!fullHeaderReceived())
		return ;

	std::getline(ss, line);
	requestLine(line);
	if (_status != 200) {
		formulateResponse();
		return ;
	}
	std::map<std::string, std::string>	fields;
	std::cout << "Request line is: " << _method << " " << _uri << " " << _httpVersion << std::endl;
	while (std::getline(ss, line)) {
		if (!line.compare("\r")) {
			setBufferAsFull();
			// This doesn't ever happen for some reason whenever the request arrives in more parts?
			std::cout << "---- UE BRO END OF HEADER -----" << std::endl;
			break;
		}
		std::replace(line.begin(), line.end(), ':', ' ');
		std::stringstream stream(line);
		std::string key;
		stream >> key;
		std::string	value;
		while (stream) {
			std::string rcodio;
			stream >> rcodio;
			value += rcodio + " ";
		}
		value.resize(value.size() - 2); // Also could just str.replace(", ") or smoething cause this be abit retarded lol
		transform(key.begin(), key.end(), key.begin(), ::tolower);
		fields[key] = value;
	}
	for (std::map<std::string, std::string>::iterator it = fields.begin(); it != fields.end(); it++)
		std::cout << "Field: [" << it->first <<"] " << "- " << "Value [" << it->second << "]" << std::endl;
	std::cout << std::endl;
	requestFields(fields);

	// /* When we're done here with the Parsing */
	// formulateResponse();
}

void	requestHandler::buildHeader() {

		/* Actually constructs on the base of the request 
			Although this map solution is a bit stupid lol but whatevs*/
	std::map<int, std::string> statusCodes;
	statusCodes[200] = "OK";
	statusCodes[400] = "Bad Request";
	statusCodes[404] = "Not Found";
	statusCodes[501] = "Method Not Implemented";
	statusCodes[505] = "HTTP Version Not Supported";
	/* Full list: https://developer.mozilla.org/en-US/docs/Web/HTTP/Status#server_error_responses
	300-399 Redirection messages
	301 Moved permanently
	401 Unauthorised
	403 Forbidden - Tied into directoy listing perhaps
	405 method not allowed 
	406 not acceptable
	*/


	std::string header = "HTTP/1.1 ";
	header += std::to_string(_status) + " ";
	header += statusCodes[_status] + "\n";

	// Server name
	if (_name.empty())
		header += "Server: Mumyer and Moederje's Server\n";
	// Date
	time_t now = time(0);
	char *date = ctime(&now);
	std::string stringDate = date;
	stringDate.insert(3, ",");
	stringDate.resize(stringDate.size() - 1);
	header += "Date: " + stringDate + " GMT\n";

	header += "Content-type: text/html; charset=UTF-8\nContent-Length:";
	if (_status != 200) {
		extractErrorFile();
	}
	// else {
			/* The following is still simplified */
	int len = _response.size();
	std::cout << "The response header is:\n" << header << std::endl;
	header.append(std::to_string(len));
	header.append("\n\n");
	header.append(_response);
	// }

	_response = header;
	// std::cout << "Full response is:\n" << _response << std::endl;
	// resetHandler();
	memset(_buffer, 0, sizeof(_buffer));
	_buffSize = 0; // Don't forget this you idiot
}

void	requestHandler::extractErrorFile() {
	std::ifstream errFile;
	if (_status == 505) {
		errFile.open("pages/errorPages/httpVersionError.html");
	}
	if (_status == 400) {
		errFile.open("pages/errorPages/badRequest.html");
	}
	if (_status == 404) {
		std::cout << "In ehre?" << std::endl;
		errFile.open("pages/errorPages/fileNotFound.html");
	}
	if (errFile.fail()) {
		//I honestly dunno lel
	}
	std::ostringstream fileContent;
	fileContent << errFile.rdbuf();
	_response = fileContent.str();
	// std::cout << "Error response:\n" << _response << std::endl;
	errFile.close();
}

void	requestHandler::respondGet() {
	std::string requestedFile("pages");
	if (!_uri.compare("/")) { // Ok lol this is still set to default hahaha
		_uri += "index.html";
	}
	requestedFile += _uri;
	std::ifstream myfile;
	myfile.open(requestedFile);
	if (myfile.fail()) {
		_status = 404; // File not Found
		buildHeader();
		return ;
	}
	std::ostringstream fileContent;
	fileContent << myfile.rdbuf();
	_response = fileContent.str();
	myfile.close();

	// std::cout << "Yo where is the 404 not found shit?" << _response << std::endl;
	buildHeader();
	// std::string	header = "HTTP/1.1 200 OK\nContent-Type: text/html; charset=UTF-8\nContent-Length:";
	// int len = _response.size();
	// header.append(std::to_string(len));
	// header.append("\n\n");
	// header.append(_response);

	// _response = header;
}

void	requestHandler::formulateResponse() {

	if (_status != 200) {
		// std::cout << "How many times you actually in here and with what status? " << _status << std::endl;
		buildHeader();
	}
	else if (_method == GET) { // Function for get responses
		respondGet();
	}
	else if (_method == POST) {

	}
	else if (_method == DELETE) {
		
	}
}

char*	requestHandler::getBuffer() {
	return _buffer;
}

void	requestHandler::setBufferAsFull() {
	_fullBuffer = true;
}

bool	requestHandler::getFullState() const {
	return _fullBuffer;
}

	// is this really the best way to do this?
void	requestHandler::resetHandler() {
	// std::cout << "Called how many times?" << std::endl;
	memset(_buffer, 0, sizeof(_buffer));
	// _response.clear();
	_status = 200;
	_host.clear();
	_method = EMPTY;
	_httpVersion.clear();
	_fullBuffer = false;
	std::cout << "Handler cleared" << std::endl;
}

requestHandler::~requestHandler() {}

