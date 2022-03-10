/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   server.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/02/04 18:38:07 by tmullan       #+#    #+#                 */
/*   Updated: 2022/03/10 15:43:52 by tmullan       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "webserv.hpp"
// #include "socket.hpp"

class serverBoy {

	public:
		serverBoy(serverSock &sock);

		~serverBoy();

		void			runServer(int backlog);
		serverSock		*getSocket();
		std::string		read_browser_request(char *buffer);

		void			first_response(int sock);
	private:
		serverBoy();
		serverSock	*_socket;
		int			ready_socket;
};

