/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   socket.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/02/02 19:19:15 by tmullan       #+#    #+#                 */
/*   Updated: 2022/02/03 11:24:01 by tmullan       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "socket.hpp"

socketMan::socketMan(int domain, int service, int protocol) {
	_connection = socket(domain, service, protocol);
}

socketMan::~socketMan() {
	
}

