/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   server.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/02/04 18:38:07 by tmullan       #+#    #+#                 */
/*   Updated: 2022/02/04 18:52:18 by tmullan       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

class serverBoy {

	public:
		serverBoy();

		~serverBoy();
	private:
		serverSock	*socket;
};

