/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   openPort.cpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/04/21 16:11:47 by tmullan       #+#    #+#                 */
/*   Updated: 2022/04/21 17:44:18 by tmullan       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "openPort.hpp"

openPort::openPort() {}

openPort::openPort(int port) {
	serverSock	sock(AF_INET, SOCK_STREAM, 0, port, INADDR_ANY);
	_listeningSocket = &sock;
}

openPort::~openPort() {}

serverSock*	openPort::getSocket() const { return _listeningSocket; }

int		openPort::acceptNewConnection() {
	socklen_t	addrlen;
	int new_fd = accept(_listeningSocket->getSock(), (struct sockaddr *)&_socket->getAddr(), (socklen_t *)&addrlen);
	if (new_fd < 0) {
		if (errno != EWOULDBLOCK) {
			perror("accept failed");
		}
		return 0;
	}
	int on = 1;
	if ((setsockopt(new_fd, SOL_SOCKET, SO_NOSIGPIPE, &on, sizeof(on)) < 0)) {
		std::cout << "sockoptions got fucked" << std::endl;
		return 0;
	}
	if ((fcntl(new_fd, F_SETFL, O_NONBLOCK)) < 0) {
		std::cout << "fcntl got fucked" << std::endl;
		return 0;
	}
	_poller.setPollFd(new_fd, (POLLIN|POLLOUT));

	// Create a new Request class in the map container
	_poller.newRequest(new_fd);
	return 1;
}

int		openPort::respondToClient(int socket) {

	std::string response = _poller.getRequests()[sock_fd].getResponse();

	char *hey = new char[response.length() + 1];
	std::strcpy(hey, response.c_str());

	int ret = send(sock_fd, hey, strlen(hey), 0);
	delete[] hey;
	return ret;
}

void	openPort::closeConnection(std::vector<struct pollfd>::iterator it) {
	std::cout << "Deleting connection [" << it->fd << "]" << std::endl;
	close(it->fd);
	// Also gotta delete the request class
	_poller.getRequests().erase(it->fd);
	_poller.getConnections().erase(it);
}
