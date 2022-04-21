/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   openPort.hpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/04/21 16:12:12 by tmullan       #+#    #+#                 */
/*   Updated: 2022/04/21 17:12:12 by tmullan       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

class openPort {

	public:
		openPort(int port);
		~openPort();
	
		serverSock	*getSocket() const;
		int			respondToClient(int socket);
		int			acceptNewConnection();
		void		closeConnection(std::vector<struct pollfd::iterator it);
	private:
		openPort();
		serverSock		*_listeningSocket;
		clientConnecter	_poller;
		// Various specifications
		

};