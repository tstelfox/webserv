/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   requestHandler.hpp                                 :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/03/25 18:47:39 by tmullan       #+#    #+#                 */
/*   Updated: 2022/04/06 12:17:46 by tmullan       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <string>

	/* Ye this is all very hypothetical */

class requestHandler {

	public:
		requestHandler(int fd);
		requestHandler();
		~requestHandler();

		// void	setFd(int fd);

		void	fillBuffer(char *buff, int valread);
		char	*getBuffer();
		void	bufferIsFull();
		bool	getFullState();

		int		getFd() const;
		void	parseRequest();

		void		formulateResponse();
		std::string	getResponse() const;

	public:
		enum	methodTypes { EMPTY, GET, POST, DELETE };

	private:
		int			_clientFd;

		/* Buffer variables */
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

		/* Status Code and response info */
		int			_status;

		/* Final response string to be sent */
		std::string	_response;
};
