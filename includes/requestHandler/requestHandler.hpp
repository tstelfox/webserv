/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   requestHandler.hpp                                 :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/03/25 18:47:39 by tmullan       #+#    #+#                 */
/*   Updated: 2022/03/25 19:13:13 by tmullan       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <string>

	/* Ye this is all very hypothetical */

class requestHandler {

	public:
		requestHandler();
		~requestHandler();

	private:
		std::string	_requestString;
		bool		_keepAlive;
		int			_method;
};