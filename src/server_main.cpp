/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   server_main.cpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/02/03 15:40:57 by tmullan       #+#    #+#                 */
/*   Updated: 2022/03/22 15:53:43 by tmullan       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

#include <iostream>
#include <unistd.h>
#include <sstream>
#include <fstream>

int		main() {

    // Parse config file and from there build servers/sockets and sheet
	// Write a config file
	serverSock	hello(AF_INET, SOCK_STREAM, 0, 8080, INADDR_ANY);
	serverBoy	serf(hello);

	serf.runServer(3);
	// std::cout << "Exiting via here?" << std::endl;
	return 0;
}