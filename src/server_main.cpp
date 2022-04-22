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
#include "serverBlock.hpp"

#include <iostream>
#include <unistd.h>
#include <sstream>
#include <fstream>

int main() {

    // Parse config file and then use that info to count serverBlocks and give specificatons
     int openPortsNum = 1;
     std::vector<serverBlock>	openPorts;
     for (int i = 0; i < openPortsNum; i++) {
     	// Retrieve the info to create the openPort class
         serverBlock	tempServer(8080);
         openPorts.push_back(tempServer);
     }

     poller littyServer(openPorts);



    /*Functioning main pre restructure*/
//    serverSock hello(AF_INET, SOCK_STREAM, 0, 8080, INADDR_ANY);
//    serverBoy littyServer(hello);
//
//
//    littyServer.runServer();
    std::cout << "Exiting via here?" << std::endl;
    return 0;
}