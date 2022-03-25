/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   server.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/02/04 18:38:07 by tmullan       #+#    #+#                 */
/*   Updated: 2022/03/25 18:57:14 by tmullan       ########   odam.nl         */
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

		int				firstResponse(int sock); // This shit is temporary bollocks
		int				connectionError(short revents);
		void			newConnection();
		void			closeConnection(std::vector<struct pollfd>::iterator it);
	private:
		serverBoy();
		serverSock		*_socket;
		clientConnecter	poller;
		// requestHandler	handler;
};

