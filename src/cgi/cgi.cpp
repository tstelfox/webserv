/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   cgi.cpp                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: akramp <akramp@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/18 14:48:55 by akramp        #+#    #+#                 */
/*   Updated: 2022/05/19 15:01:53 by akramp        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "cgi.hpp"
#include "exceptions.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <iostream>
#include <strings.h>

void    WSERV::Cgi::open_file(int rw)
{
    if (rw > 1 || rw < 0)
        throw std::runtime_error("wrong read or write num for fd, only 0 or 1");
    _cgi_fd[rw] = open(_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (_cgi_fd[rw] < 0)
        throw OpenFileExcep();
}

WSERV::Cgi::Cgi(std::string &path) : _path(path)
{
    pid_t	pid;
    int status;
    char *newenviron[] = { NULL };
    _cgi_fd = new int [2];
    
    // std::cout << path << _path << std::endl;  
    
    pipe(_cgi_fd);
    pid = fork();
	if (pid == -1)
		throw std::runtime_error("Fork Failed in cgi");
    if (pid == 0)
    {
        std::cout << "test!!" << std::endl;
        if (execve(_path.c_str(), NULL, newenviron) == -1)
            throw std::runtime_error("Error: execve failed in cgi");
        if (dup2(_cgi_fd[READ], STDOUT_FILENO) == -1 || close(_cgi_fd[READ]) == -1)
			throw std::runtime_error("Error: dup2/close failed");
        if (dup2(_cgi_fd[WRITE], STDOUT_FILENO) == -1 || close(_cgi_fd[WRITE]) == -1)
			throw std::runtime_error("Error: dup2/close failed");
        
		exit(0);
    }
    waitpid(0, &status, 0);

    char string[sizeof(_cgi_fd[WRITE])];
    bzero(string, sizeof(string));
    int ret = read(_cgi_fd[WRITE], string, sizeof(_cgi_fd[WRITE]));
    string[ret] = '\0';
    std::cout << string << std::endl;
    // close(_cgi_in_fd);
}

WSERV::Cgi::~Cgi()
{
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
	return *this;
}