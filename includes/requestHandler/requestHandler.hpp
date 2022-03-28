/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   requestHandler.hpp                                 :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/03/25 18:47:39 by tmullan       #+#    #+#                 */
/*   Updated: 2022/03/28 14:04:16 by tmullan       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <string>

	/* Ye this is all very hypothetical */

class requestHandler {

	public:
		requestHandler();
		// requestHandler(char *buffer);
		~requestHandler();

		void	fillBuffer(char *buff, int valread);
		char	*getBuffer();

	private:
		char		_buffer[1024];
		int			_buffSize;
		std::string	_requestString;
		bool		_keepAlive;
		int			_method;
};