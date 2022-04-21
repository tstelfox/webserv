/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   server_main.cpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/02/03 15:40:57 by tmullan       #+#    #+#                 */
/*   Updated: 2022/04/21 20:48:09 by tmullan       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

#include <iostream>
#include <unistd.h>
#include <sstream>
#include <fstream>

int		main() {

    // Parse config file and then use that info to count serverBlocks and give specificatons
	// int serverBlocksNum = 1;
	// std::vector<openPort>	serverBlocks;
	// for (int i = 0; i < serverBlocks; i++) {
	// 	// Retrieve the info to create the openPort class
	// 	openPort	newPort(8080);
	// 	serverBlocks.push_back(newPort);
	// }
	
	// serverBoy		littyServer(serverBlocks);


	serverSock	hello(AF_INET, SOCK_STREAM, 0, 8080, INADDR_ANY);
	serverBoy	littyServer(hello);


	littyServer.runServer();
	std::cout << "Exiting via here?" << std::endl;
	return 0;
}