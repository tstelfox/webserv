/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   socket.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/02/02 15:17:36 by tmullan       #+#    #+#                 */
/*   Updated: 2022/02/03 10:55:51 by tmullan       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#prama once
#include <sys/socket.h>

class socket 
{
	public:
		socket(int domain, int service, int protocol);
		~socket();
	private:
		int sock;
		/* data */
};

