/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   clientConnecter.cpp                               :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/03/16 17:24:25 by tmullan       #+#    #+#                 */
/*   Updated: 2022/03/16 19:01:04 by tmullan       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

clientConnecter::clientConnecter() {}

clientConnecter::~clientConnecter() {}

void	 clientConnecter::setPollFd(int fd, short events) {
	struct pollfd	newPollFd;
	newPollFd.fd = fd;
	newPollFd.events = events;
	_connections.push_back(newPollFd);
}
