/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   server_main.cpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/02/03 15:40:57 by tmullan       #+#    #+#                 */
/*   Updated: 2022/03/23 13:40:32 by tmullan       ########   odam.nl         */
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

	serf.runServer();
	std::cout << "Exiting via here?" << std::endl;
	return 0;
}