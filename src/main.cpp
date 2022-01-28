/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.cpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/01/28 11:37:35 by tmullan       #+#    #+#                 */
/*   Updated: 2022/01/28 12:32:54 by tmullan       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include <sys/socket.h>

int		main(int argc, char *argv[]) {
	
	/* Create the Socket
	socket(domain, type, protocol); */
	int server_fd = socket(AF_INET, SOCK_STREAM, 0); // Protect against failure

	/* Identify (name) a socket
	bind(int socket, const struct sockaddr *address, socklen_t address_len); */
	
	
}