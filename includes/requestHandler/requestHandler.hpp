/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   requestHandler.hpp                                 :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/03/25 18:47:39 by tmullan       #+#    #+#                 */
/*   Updated: 2022/04/20 11:30:31 by tmullan       ########   odam.nl         */
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

		/* Management of incoming request */
		void	fillBuffer(char *buff, int valread);
		void	setBufferAsFull();
		bool	getFullState() const;
		void	resetHandler();
		void	parseRequest();

		/* Retrieve response */
		std::string	getResponse() const;

		/* Testing */
		char	*getBuffer();
		// int		getFd() const;

	private:

		/* Request parsing */
		int		fullHeaderReceived();
		void	requestLine(std::string request);
		void	requestFields(std::map<std::string, std::string> fields);

		/* Response formulation */
		void		buildHeader();
		void		respondGet();
		void		extractErrorFile();
		void		formulateResponse();

	public:
		enum	methodTypes { EMPTY, GET, POST, DELETE };

	private:
		// int			_clientFd; // Not entirely sure I need to have this info within the class

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
		std::string	_name;
		// std::string	_statusCode; Just add them in the formulator?

		/* Final response string to be sent */
		std::string	_response;
};
