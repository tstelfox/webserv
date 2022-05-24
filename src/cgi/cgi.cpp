/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   cgi.cpp                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: akramp <akramp@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/18 14:48:55 by akramp        #+#    #+#                 */
/*   Updated: 2022/05/24 16:05:44 by akramp        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "cgi.hpp"
#include "exceptions.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <iostream>
#include <strings.h>
#include <sys/stat.h>
#include <stdio.h>

WSERV::Cgi::Cgi(std::string &path, std::string first, std::string second) : _path(path)
{
    pid_t	pid;
    int status;
    char *newenviron[] = { NULL };
    _cgi_fd = new int [2];
    _argv = new char* [4];
    _argv[0] = strdup(path.c_str());
    _argv[1] = strdup(first.c_str());
    _argv[2] = strdup(second.c_str());
    _argv[3] = 0;

    pipe(_cgi_fd);
    pid = fork();
	if (pid == -1)
		throw std::runtime_error("Fork Failed in cgi");
    if (pid == 0)
    {
        if (dup2(_cgi_fd[READ], STDOUT_FILENO) == -1 || close(_cgi_fd[READ]) == -1)
			throw std::runtime_error("Error: dup2/close failed");
        if (dup2(_cgi_fd[WRITE], STDOUT_FILENO) == -1 || close(_cgi_fd[WRITE]) == -1)
			throw std::runtime_error("Error: dup2/close failed");
        if (execve(_path.c_str(), _argv, newenviron) == -1)
            throw std::runtime_error("Error: execve failed in cgi");
		exit(0);
    }
    waitpid(0, &status, 0);
}

WSERV::Cgi::~Cgi()
{
    free(_argv[0]);
    free(_argv[1]);
    free(_argv[2]);
    delete _argv;
    delete _cgi_fd;
}

WSERV::Cgi::Cgi(Cgi const & copy)
{
	*this = copy;
}

WSERV::Cgi & WSERV::Cgi::operator = (Cgi const & copy)
{
    this->_cgi_fd = copy._cgi_fd;
    this->_path = copy._path;
    this->_argv = copy._argv;
	return *this;
}

int *WSERV::Cgi::get_cgi_fd(void) const
{
    return (this->_cgi_fd);
}

std::string WSERV::Cgi::get_path(void) const
{
    return (this->_path);
}

char **WSERV::Cgi::get_argv(void) const
{
    return (this->_argv);
}
