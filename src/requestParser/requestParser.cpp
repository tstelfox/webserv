/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   requestParser.cpp                                  :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/01 18:30:39 by tmullan       #+#    #+#                 */
/*   Updated: 2022/05/01 18:30:39 by tmullan       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "requestParser.hpp"
#include "colours.hpp"
#include <iostream>

requestParser::requestParser(WSERV::serverConfig const &configs, std::map<std::string, std::string> &fields)
: _config(configs), _requestFields(fields) {
    std::cout << ITALIC << COLOR_NEON << "Brr I parse now" << FORMAT_RESET << RESET_COLOUR << std::endl;
}

requestParser::requestParser() {}

requestParser::~requestParser() {}

