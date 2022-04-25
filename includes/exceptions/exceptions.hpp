/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   exceptions.hpp                                     :+:    :+:            */
/*                                                     +:+                    */
/*   By: akramp <akramp@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/04/13 20:57:50 by akramp        #+#    #+#                 */
/*   Updated: 2022/04/17 13:24:23 by ubuntu        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXCEPTIONS_HPP
# define EXCEPTIONS_HPP
# include <exception>
# include <sstream>

class OpenFileExcep : public std::exception
{
	private:
		char _ret_msg[239];
	public:
		OpenFileExcep();
		virtual ~OpenFileExcep() throw();
		virtual const char* what() const throw();

};

class TooManyArgsExcep : public std::exception
{
	private:
		char _ret_msg[209];
	public:
		TooManyArgsExcep();
		virtual ~TooManyArgsExcep() throw();
		virtual const char* what() const throw();
};

class TooLittleArgsExcep : public std::exception
{
	private:
		char _ret_msg[203];
	public:
		TooLittleArgsExcep();
		virtual ~TooLittleArgsExcep() throw();
		virtual const char* what() const throw();
};

class IncorrectConfigExcep : public std::exception
{
	private:
		char _ret_msg[216];
	public:
		IncorrectConfigExcep();
		virtual ~IncorrectConfigExcep() throw();
		virtual const char* what() const throw();
};
#endif
