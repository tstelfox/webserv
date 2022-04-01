/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   responseHandler.cpp                                :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/03/30 18:13:16 by tmullan       #+#    #+#                 */
/*   Updated: 2022/04/01 16:08:28 by tmullan       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "responseHandler.hpp"
#include <iostream>

responseHandler::responseHandler(requestHandler& request) {
	std::cout << "Response constructed" << std::endl;
	_request = &request;
}

responseHandler::~responseHandler() {}

