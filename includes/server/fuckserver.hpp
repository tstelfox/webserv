/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   server.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/02/04 18:38:07 by tmullan       #+#    #+#                 */
/*   Updated: 2022/04/21 17:51:57 by tmullan       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "socket.hpp"
#include "clientConnecter.hpp"
#include <string>

class serverBoy {

	public:
		serverBoy(std::vector<openPort>	serverBlocks);

		~serverBoy();

		void			runServer();
		// serverSock		*getSocket() const;
		std::vector<openPort>	getOpenPorts() const;

		// int				respondToClient(int sock); // This shit is temporary bollocks
		// int				connectionError(short revents);
		// void			newConnection();
		// void			closeConnection(std::vector<struct pollfd>::iterator it);
	private:
		serverBoy();

		// This is how we tie this shit in
		std::vector<openPort>	_openPorts;
};

