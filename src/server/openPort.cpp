/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   openPort.cpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/04/21 16:11:47 by tmullan       #+#    #+#                 */
/*   Updated: 2022/04/21 17:07:49 by tmullan       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "openPort.hpp"

openPort::openPort() {}

openPort::openPort(int port) {}

openPort::~openPort() {}

serverSock*	openPort::getSocket() const { return _listeningSocket; }