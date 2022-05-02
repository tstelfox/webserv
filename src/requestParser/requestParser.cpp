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

requestParser::requestParser(const configVector &configs, std::string request)
: _configs(configs), _request(request) {

}

requestParser::requestParser() {}

requestParser::~requestParser() {}

