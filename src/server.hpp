/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   server.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/02/04 18:38:07 by tmullan       #+#    #+#                 */
/*   Updated: 2022/02/04 19:42:51 by tmullan       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "webserv.hpp"

class serverBoy {

	public:
		serverBoy(serverSock &sock);

		void	run_server(int backlog);

		~serverBoy();
	private:
		serverBoy();
		serverSock	*socket;
		int			ready_socket;
};
