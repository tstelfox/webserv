/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   cgi.cpp                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: akramp <akramp@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/18 14:48:55 by akramp        #+#    #+#                 */
/*   Updated: 2022/05/18 15:02:27 by akramp        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "cgi.hpp"

WSERV::Cgi::Cgi()
{
}

WSERV::Cgi::~Cgi()
{
}

WSERV::Cgi::Cgi(Cgi const & copy)
{
	*this = copy;
}

WSERV::Cgi & WSERV::Cgi::operator = (Cgi const & copy)
{
    this->_cgi_fd = copy._cgi_fd;
    this->_path = copy._path;
	return *this;
}