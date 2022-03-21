/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   webserv.hpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/02/04 19:01:03 by tmullan       #+#    #+#                 */
/*   Updated: 2022/03/21 16:48:27 by tmullan       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

// #include <iostream>
#include <unistd.h>
#include <sstream>
#include <fstream>
// #include <sys/poll.h>
// #include <sys/ioctl.h>
// #include <sys/socket.h>
// #include <netinet/in.h>
#include <iostream>
#include <sys/fcntl.h>
#include <vector>

#include "socket.hpp"
#include "server.hpp"
#include "clientConnecter.hpp"