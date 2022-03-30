/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   requestHandler.hpp                                 :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/03/25 18:47:39 by tmullan       #+#    #+#                 */
/*   Updated: 2022/03/30 16:46:41 by tmullan       ########   odam.nl         */
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
		
		void	bufferIsFull();
		bool	getFullState();

		void	parseRequest();

	public:
		enum	methodTypes { EMPTY, GET, POST, DELETE };

	private:
		char		_buffer[1024];
		int			_buffSize;
		bool		_fullBuffer;

		/* Request line Info */
		int			_method;
		std::string	_uri;
		std::string	_httpVersion;

		/* "Optional" Request Header Info */
 		std::string	_host;
		bool		_keepAlive;
};
