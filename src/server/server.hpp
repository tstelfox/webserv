/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   server.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/02/04 18:38:07 by tmullan       #+#    #+#                 */
/*   Updated: 2022/02/11 21:08:46 by tmullan       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "webserv.hpp"

class serverBoy {

	public:
		serverBoy(serverSock &sock);

		~serverBoy();

		void	run_server(int backlog);
		serverSock	*get_socket() { return socket; }
	private:
		serverBoy();
		serverSock	*socket;
		int			ready_socket;
};

