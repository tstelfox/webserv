/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   clientConnecter.hpp                               :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/03/16 17:24:04 by tmullan       #+#    #+#                 */
/*   Updated: 2022/03/16 19:04:58 by tmullan       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <vector>
#include "sys/poll.h"

class clientConnecter
{
	public:
		clientConnecter();
		~clientConnecter();

		void	setPollFd(int fd, short events);
		std::vector<struct pollfd>& getConnections();
	private:
		std::vector<struct pollfd> _connections;

		// Also keep the buffers and catalogue them with their fds in a map, probably

};

