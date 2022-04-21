/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   requestHandler.cpp                                 :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/03/25 19:06:20 by tmullan       #+#    #+#                 */
/*   Updated: 2022/04/21 15:56:39 by tmullan       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "requestHandler.hpp"
#include "colours.hpp"
#include <iostream>
#include <sstream>
#include <fstream>
#include <map>
#include <algorithm>

	/* < --------- Constructor / Destructor ----------> */

// requestHandler::requestHandler(int fd) : _clientFd(fd), _buffSize(0), _fullBuffer(false), _method(0),
requestHandler::requestHandler() : _buffSize(0), _fullBuffer(false), _method(0), 
				_keepAlive(true), _status(200) {
	memset(_buffer, 0, 1024);
	(void)_keepAlive;
	(void)_method;
}

requestHandler::~requestHandler() {}

		/* < ---------- Manage Incoming Request Buffer ----------- > */

void	requestHandler::fillBuffer(char *buff, int valread) {
	int temp = _buffSize;
	_buffSize += valread;
	for (int i = 0; i < valread; i++) {
		_buffer[temp] = buff[i];
		temp++;
	}
	_buffer[temp] = '\0';
}

void	requestHandler::setBufferAsFull() {
	_fullBuffer = true;
}

void	requestHandler::resetHandler() { // is this really the best way to do this?
	memset(_buffer, 0, sizeof(_buffer));
	// _response.clear();
	_status = 200;
	_host.clear();
	_method = EMPTY;
	_httpVersion.clear();
	_fullBuffer = false;
	std::cout << "Handler cleared" << std::endl;
}

		/* < --------- Parse Received Request ------ > */

int		requestHandler::fullHeaderReceived() {
	std::string request(_buffer);
	std::istringstream ss(request);
	std::string line;
	while (std::getline(ss, line)) {
		if(!line.compare("\r")) {
			// setBufferAsFull(); Request might have a body so not ready for this
			return 1;
		}
	}
	return 0;
}

void	requestHandler::parseRequestLine(std::string request) {
	std::stringstream	ss(request);
	std::string			field;
	
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
	if (_uri[0] != '/') { // This'll segfault if there's nothign there of course PLUS so much other shit
		// Consider also the possibility of an absolute uri e.g. http://github.com/tstelfox
		_status = 400;
	}
	// std::cout << "_uri is: [" << _uri << "]" << std::endl;
	ss >> _httpVersion;
	if (!_httpVersion.empty() && _httpVersion.compare("HTTP/1.1")) {
		_status = 505; // HTTP VERSION NOT SUPPORTED
	}
}

void	requestHandler::requestFields(std::map<std::string, std::string> &fields) {

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
	if (fields["host"].empty() || !fields["host"].compare(" ")) {
		std::cout << "No host" << std::endl;
		_status = 400;
	}
	else
		_host = fields["host"];
	formulateResponse();
}

void	requestHandler::parseRequest() {
	std::string 		request(_buffer);
	std::istringstream	ss(request);
	std::string			line;

	// std::cout << "Parse another thing:\n" << _buffer << std::endl;
	if (!fullHeaderReceived()) {
		// std::cout << "Not full yet" << std::endl;
		return ;
	}

	std::getline(ss, line);
	parseRequestLine(line);
	if (_status != 200) {
		setBufferAsFull();
		formulateResponse();
		return ;
	}
	std::map<std::string, std::string>	fields;
	std::cout << CYAN << "<-------Request line-------->\n" << RESET_COLOUR << \
			_method << " " << _uri << " " << _httpVersion << std::endl << std::endl;
	while (std::getline(ss, line)) {
		if (!line.compare("\r")) {
			setBufferAsFull();
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
		if (value.size() > 2)
			value.resize(value.size() - 2); // Also could just str.replace(", ") or smoething cause this be abit retarded lol
		transform(key.begin(), key.end(), key.begin(), ::tolower);
		fields[key] = value;
	}

	std::cout << MAGENTA << "<--------Optional Header requests------->" << RESET_COLOUR << std::endl;
	for (std::map<std::string, std::string>::iterator it = fields.begin(); it != fields.end(); it++)
		std::cout << "Field: [" << it->first <<"] " << "- " << "Value [" << it->second << "]" << std::endl;
	std::cout << std::endl;
	requestFields(fields);

	// /* When we're done here with the Parsing */
	// formulateResponse();
}

		/* < ------- Build Response Header ------ > */

void	requestHandler::extractErrorFile() {
	std::ifstream errFile;
	if (_status == 505) {
		errFile.open("pages/errorPages/httpVersionError.html");
	}
	if (_status == 400) {
		errFile.open("pages/errorPages/badRequest.html");
	}
	if (_status == 404) {
		errFile.open("pages/errorPages/fileNotFound.html");
	}
	if (_status == 405) { // This'll be dependent on location accepted methods
		errFile.open("pages/errorPages/methodNotAllowed.html");
	}
	if (errFile.fail()) {
		//I honestly dunno lel
	}
	std::ostringstream fileContent;
	fileContent << errFile.rdbuf();
	_response = fileContent.str();
	errFile.close();
}

void	requestHandler::buildHeader() {

		/* Actually constructs on the base of the request 
			Although this map solution is a bit stupid lol but whatevs*/
	std::map<int, std::string> statusCodes;
	statusCodes[200] = "OK";
	statusCodes[400] = "Bad Request";
	statusCodes[404] = "Not Found";
	statusCodes[405] = "Method Not Allowed";
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

	if (!_uri.compare("/favicon.ico"))
		header += "Content-type: image/avif,image/webp,image/apng,image/svg+xml,image/*,*/*;q=0.8\nContent-Length:";
	else
		header += "Content-type: text/html; charset=UTF-8\nContent-Length:";
	if (_status != 200) {
		// std::cout << "Extracting error file: " << _status << std::endl;
		extractErrorFile();
	}
		/* The following is still simplified */
	int len = _response.size();
	header.append(std::to_string(len));

	if (_status != 200) { // Need to check if this is always the case
		header += "\nConnection: close";
	}

	header.append("\n\n");
	std::cout << RED << "<<<<-------- The response header ------->>>>\n" << RESET_COLOUR << header << std::endl;

	// std::cout << YELLOW << "<-------- Response Body ------->\n" << RESET_COLOUR << _response << std::endl;
	header.append(_response);
	header.append("\n");
	_response = header;

	memset(_buffer, 0, sizeof(_buffer));
	_buffSize = 0; // Don't forget this you idiot
}


void	requestHandler::respondGet() {
	std::string requestedFile("pages");
	if (!_uri.compare("/")) { // This part is gonna be dependent on the location
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

	buildHeader();
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
		//respondPost();
	}
	else if (_method == DELETE) {
		//respondDelete();
	}
}

		/* <---------- Getter functions ---------> */

char*	requestHandler::getBuffer() {
	return _buffer;
}

bool	requestHandler::getFullState() const {
	return _fullBuffer;
}

std::string	requestHandler::getResponse() const {
	return this->_response;
}


