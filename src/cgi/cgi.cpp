/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   cgi.cpp                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: akramp <akramp@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/18 14:48:55 by akramp        #+#    #+#                 */
/*   Updated: 2022/05/18 19:49:23 by akramp        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "cgi.hpp"
#include "exceptions.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>

void    WSERV::Cgi::open_file()
{
    _cgi_in_fd = open(_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (_cgi_in_fd < 0)
        throw OpenFileExcep();
}

WSERV::Cgi::Cgi(std::string path) : _path(path)
{
    pid_t	pid;
    int status;
    open_file();

    pid = fork();
	if (pid == -1)
		throw std::runtime_error("Fork Failed in cgi");
    if (pid == 0)
    {
        // if (dup2(out_fd, STDOUT_FILENO) == -1 || close(out_fd) == -1)
		// 	throw std::runtime_error("Error: dup2/close failed in Cgi::execute");
        // execve(this->args[0], this->args, this->env);
		exit(0);
    }
    waitpid(0, &status, 0);
}

WSERV::Cgi::~Cgi()
{
    close(_cgi_fd);
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