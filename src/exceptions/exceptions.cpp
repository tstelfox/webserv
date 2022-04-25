/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   exceptions.cpp                                     :+:    :+:            */
/*                                                     +:+                    */
/*   By: akramp <akramp@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/04/13 20:57:47 by akramp        #+#    #+#                 */
/*   Updated: 2022/04/17 13:23:26 by ubuntu        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "includes/exceptions/exceptions.hpp"
#include <sstream>
#include "../colors/colors.h"
#include <iostream>

OpenFileExcep::OpenFileExcep() : _ret_msg("\n" COLOR_DARKRED \
    ERROR_ASCII "\n" COLOR_RED "File doesn't exist or the permissions are wrong.\n" \
    "Try \'chmod 400 [PATH/FILENAME]\' and execute the program again." COLOR_RESET "\n") {}

OpenFileExcep::~OpenFileExcep() throw(){}

const char* OpenFileExcep::what() const throw()
{
    return (_ret_msg);
}

TooLittleArgsExcep::TooLittleArgsExcep() : _ret_msg("\n" COLOR_DARKRED \
ERROR_ASCII "\n" COLOR_RED "Too little arguments.\n" \
"Try adding a configfile and execute the program again\n" COLOR_RESET) {}

TooLittleArgsExcep::~TooLittleArgsExcep() throw(){}

const char* TooLittleArgsExcep::what() const throw()
{
    return (_ret_msg);
}

TooManyArgsExcep::TooManyArgsExcep() : _ret_msg("\n" COLOR_DARKRED \
ERROR_ASCII "\n" COLOR_RED "Too many arguments.\n" \
"Try adding just one configfile and execute the program again.\n" COLOR_RESET) {}

TooManyArgsExcep::~TooManyArgsExcep() throw(){}

const char* TooManyArgsExcep::what() const throw()
{
    return (_ret_msg);
}

IncorrectConfigExcep::IncorrectConfigExcep() : _ret_msg("\n" COLOR_DARKRED \
ERROR_ASCII "\n" COLOR_RED "Incorrect configfile.\n" \
"Read the README for more information and execute the program again\n" COLOR_RESET) {}

IncorrectConfigExcep::~IncorrectConfigExcep() throw(){}

const char* IncorrectConfigExcep::what() const throw()
{
    return (_ret_msg);
}

//\033[38;5;88m◢◤◢◤◢◤◢◤◢◤◢◤◢◤ ᴇʀʀᴏʀ ◢◤◢◤◢◤◢◤◢◤◢◤◢◤\033[38;5;196m\033[37m 70
