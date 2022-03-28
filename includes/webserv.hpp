/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   webserv.hpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/02/04 19:01:03 by tmullan       #+#    #+#                 */
/*   Updated: 2022/03/28 13:01:33 by tmullan       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <unistd.h>
#include <sstream>
#include <fstream>
#include <iostream>
#include <sys/fcntl.h>
#include <vector>

#include "socket.hpp"
#include "server.hpp"
#include "clientConnecter.hpp"