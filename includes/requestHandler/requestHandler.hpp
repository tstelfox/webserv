/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   requestHandler.hpp                                 :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/03/25 18:47:39 by tmullan       #+#    #+#                 */
/*   Updated: 2022/04/19 11:37:41 by tmullan       ########   odam.nl         */
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


		/* A lot of these functions should realistically be made private */

		/* Management of request retrieval */
		void	fillBuffer(char *buff, int valread);
		char	*getBuffer();
		void	setBufferAsFull();
		bool	getFullState() const;
		int		getFd() const;
		void	resetHandler();

		/* Request parsing */
		void	requestLine(std::string request);
		void	requestFields(std::map<std::string, std::string> fields);
		void	parseRequest();

		/* Response formulation */
		void		buildHeader();
		void		respondGet();
		void		formulateResponse();
		std::string	getResponse() const;

	private:
		void		extractErrorFile();

		int			fullHeaderReceived();
		// Private funcs

	public:
		enum	methodTypes { EMPTY, GET, POST, DELETE };

	private:
		int			_clientFd; // Not entirely sure I need to have this info within the class

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
