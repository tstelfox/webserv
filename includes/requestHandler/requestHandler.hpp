/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   requestHandler.hpp                                 :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/03/25 18:47:39 by tmullan       #+#    #+#                 */
/*   Updated: 2022/03/28 17:01:25 by tmullan       ########   odam.nl         */
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

	private:
		char		_buffer[1024];
		int			_buffSize;
		bool		_fullBuffer;
		// std::string	_requestString;
		bool		_keepAlive;
		int			_method;
};