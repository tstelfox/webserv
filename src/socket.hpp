/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   socket.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/02/02 15:17:36 by tmullan       #+#    #+#                 */
/*   Updated: 2022/02/03 11:23:53 by tmullan       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#prama once
#include <sys/socket.h>

class socketMan
{
	public:
		socketMan(int domain, int service, int protocol);
		~socketMan();
	private:
		int _connection;
};

