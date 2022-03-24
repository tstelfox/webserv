/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   server.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/02/04 18:38:07 by tmullan       #+#    #+#                 */
/*   Updated: 2022/03/24 17:56:57 by tmullan       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "socket.hpp"
#include "clientConnecter.hpp"
#include <string>

class serverBoy {

	public:
		serverBoy(serverSock &sock);

		~serverBoy();

		void			runServer();
		serverSock		*getSocket();
		std::string		readBrowserRequest(char *buffer);

		int				firstResponse(int sock); // This shit is temporary bollocks
		int				connectionErr(short revents);
		// int				newConnection(int fd_num); This is a work in progress
		// void			closeConnection();
	private:
		serverBoy();
		serverSock		*_socket;
		clientConnecter	poller;
};

