/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Location.cpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: akramp <akramp@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/04/15 17:44:40 by akramp        #+#    #+#                 */
/*   Updated: 2022/05/17 11:51:21 by akramp        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"
#include <iostream>

WSERV::Location::Location(/* args */)
{
}

WSERV::Location::~Location()
{
}

WSERV::Location::Location(Location const & copy)
{
	*this = copy;
}

WSERV::Location & WSERV::Location::operator = (Location const & copy)
{
	this->_root = 			copy._root;
	this->_location_path = 	copy._location_path;
	this->_autoindex = 		copy._autoindex;
	this->_allow_method = 	copy._allow_method;
	this->_index = 			copy._index;
	this->_error_page = 	copy._error_page;
	this->_max_file_size = 	copy._max_file_size;
	this->_cgi_allowed_extensions = 	copy._cgi_allowed_extensions;
	this->_default_cgi_path = 			copy._default_cgi_path;
	this->_php_cgi = 					copy._php_cgi;
	this->_auth_basic = 			copy._auth_basic;
	this->_redirect =		copy._redirect;
	return *this;
}

std::string WSERV::Location::get_root( void ) const
{
	return (this->_root);
}

std::string WSERV::Location::get_location_path( void ) const
{
	return (this->_location_path);
}

bool 		WSERV::Location::get_autoindex( void ) const
{
	return (this->_autoindex);
}

std::map<int, std::string> WSERV::Location::get_allow_method( void ) const
{
	return (this->_allow_method);
}

std::string WSERV::Location::get_index( void ) const
{
	return (this->_index);
}

std::string WSERV::Location::get_error_page( void ) const
{
	return (this->_error_page);
}

unsigned int 		WSERV::Location::get_max_file_size( void ) const
{
	return (this->_max_file_size);
}

bool 		WSERV::Location::get_cgi_allowed_extensions( void ) const
{
	return (this->_cgi_allowed_extensions);
}

std::string WSERV::Location::get_default_cgi_path( void ) const
{
	return (this->_default_cgi_path);
}

std::string WSERV::Location::get_php_cgi( void ) const
{
	return (this->_php_cgi);
}

bool WSERV::Location::get_auth_basic( void ) const
{
	return (this->_auth_basic);
}

std::pair<std::string, std::string> WSERV::Location::get_redirect( void ) const
{
	return (this->_redirect);
}


void WSERV::Location::set_redirect(std::pair<std::string, std::string> const &data )
{
	this->_redirect = data;
}

void WSERV::Location::set_root( std::string const data)
{
	this->_root = data;
}

void WSERV::Location::set_location_path( std::string const data)
{
	this->_location_path = data;
}

void WSERV::Location::set_autoindex( bool const data)
{
	this->_autoindex = data;
}

void WSERV::Location::set_allow_method( std::map<int, std::string> const  & data)
{
	this->_allow_method = data;	
}

void WSERV::Location::set_index( std::string const data)
{
	this->_index = data;
}

void WSERV::Location::set_error_page( std::string const data)
{
	this->_error_page = data;
}

void WSERV::Location::set_max_file_size( unsigned int const data)
{
	this->_max_file_size = data;
}

void WSERV::Location::set_cgi_allowed_extensions( bool const data)
{
	this->_cgi_allowed_extensions = data;
}

void WSERV::Location::set_default_cgi_path( std::string const data)
{
	this->_default_cgi_path = data;
}

void WSERV::Location::set_php_cgi( std::string const data)
{
	this->_php_cgi = data;
}

void WSERV::Location::set_auth_basic( bool const data)
{
	this->_auth_basic = data;
}

