/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   requestHandler.hpp                                 :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/03/25 18:47:39 by tmullan       #+#    #+#                 */
/*   Updated: 2022/04/11 16:51:10 by tmullan       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <string>
#include <map>

class requestHandler {

	public:
		requestHandler(int fd);
		requestHandler();
		~requestHandler();

		// void	setFd(int fd);

		void	fillBuffer(char *buff, int valread);
		char	*getBuffer();
		void	bufferIsFull();
		bool	getFullState() const;
		int		getFd() const;

		void	requestLine(std::string request);
		void	requestFields(std::map<std::string, std::string> fields);
		void	parseRequest();


		void		formulateResponse();
		std::string	getResponse() const;

		void		resetHandler();


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
