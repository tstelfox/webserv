/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   server.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/02/04 18:38:07 by tmullan       #+#    #+#                 */
/*   Updated: 2022/02/11 21:16:57 by tmullan       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "webserv.hpp"

class serverBoy {

	public:
		serverBoy(serverSock &sock);

		~serverBoy();

		void	runServer(int backlog);
		serverSock	*getSocket();
	private:
		serverBoy();
		serverSock	*_socket;
		int			ready_socket;
};

