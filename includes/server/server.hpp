/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   server.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/02/04 18:38:07 by tmullan       #+#    #+#                 */
/*   Updated: 2022/04/07 14:56:49 by tmullan       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "socket.hpp"
#include "clientConnecter.hpp"
#include <string>

	/* The structure is incorrect.
		- Polling cannot occur within the server object
			because if there are multiple servers there must be a single poll()
			to handle all of the listening sockets on the various ports.
		- A poller general function will probably have to be called and have
			a vector or map of servers passed to it for it to loop through the
			listening socket for each of them. */

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
};

