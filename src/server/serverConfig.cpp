/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   serverConfig.cpp                                   :+:    :+:            */
/*                                                     +:+                    */
/*   By: turloughmullan <turloughmullan@student.      +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/04/25 19:37:04 by turloughmul   #+#    #+#                 */
/*   Updated: 2022/05/19 13:56:58 by akramp        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */


#include "serverConfig.hpp"
#include <string>

WSERV::serverConfig::serverConfig() {
}

WSERV::serverConfig::~serverConfig() {
}

WSERV::serverConfig::serverConfig(serverConfig const &copy) {
    *this = copy;
}

WSERV::serverConfig &WSERV::serverConfig::operator=(serverConfig const &copy) {
    this->_port = copy._port;
    this->_host = copy._host;
    this->_server_name = copy._server_name;
    this->_error_page = copy._error_page;
    this->_Location_vec = copy._Location_vec;
    return *this;
}

/* Getters & Setters*/

int WSERV::serverConfig::get_port() const {
    return (this->_port);
}

void WSERV::serverConfig::set_port(int const data) {
    this->_port = data;
}

std::string WSERV::serverConfig::get_host() const {
    return (this->_host);
}

void WSERV::serverConfig::set_host(std::string const &data) {
    this->_host = data;
}

std::string WSERV::serverConfig::get_server_name() const {
    return (this->_server_name);
}

void WSERV::serverConfig::set_server_name(std::string const data) {
    this->_server_name = data;
}

// unsigned int WSERV::serverConfig::get_maxfilesize() const {
//     return (this->_maxfilesize);
// }

// void WSERV::serverConfig::set_maxfilesize(int const data) {
//     this->_maxfilesize = data;
// }

std::string WSERV::serverConfig::get_error_page() const {
    return (this->_error_page);
}

void WSERV::serverConfig::set_error_page(std::string const data) {
    this->_error_page = data;
}

std::vector<WSERV::Location> WSERV::serverConfig::get_Location_vec() const {
    return (this->_Location_vec);
}

void WSERV::serverConfig::set_Location_vec(std::vector<WSERV::Location> const data) {
    this->_Location_vec = data;
}
