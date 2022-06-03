/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   client_getters.cpp                                 :+:    :+:            */
/*                                                     +:+                    */
/*   By: ask <ask@student.codam.nl>                   +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/06/01 20:23:55 by ask           #+#    #+#                 */
/*   Updated: 2022/06/01 20:25:23 by ask           ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "client.hpp"

std::string client::getBuffer() const { return _buffer; }

std::string client::getResponse() const { return _response; }

int     client::getSocket() const { return _socket; }

bool client::isBufferFull() const { return _isBuffFull; }

bool client::isCgi() const { return _isCgi; }

int client::getCgiFd() const { return _cgiFd; }

std::string client::getCgiResponse() const { return _cgiResponse; }