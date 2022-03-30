/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   requestHandler.hpp                                 :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/03/25 18:47:39 by tmullan       #+#    #+#                 */
/*   Updated: 2022/03/30 14:01:40 by tmullan       ########   odam.nl         */
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
		enum	methodTypes { GET, POST, DELETE };

	private:
		char		_buffer[1024];
		int			_buffSize;
		bool		_fullBuffer;

		
		std::string	_uri;
		std::string	_host;
		std::string	_httpVersion;
		bool		_keepAlive;
		int			_method;
};
