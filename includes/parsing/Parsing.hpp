/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   parsing.hpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: akramp <akramp@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/04/07 17:36:00 by akramp        #+#    #+#                 */
/*   Updated: 2022/04/25 12:54:45 by akramp        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSING_HPP
# define PARSING_HPP
#	include <fstream>
#	include <vector>
#	include <map>
#	include <iostream> //std::cout
#	include <algorithm> //std::find
#	include <ctype.h> //std::isalpha
#	include "serverConfig.hpp"
#	include "exceptions.hpp"
#	include "colours.hpp"


// void    open_file(std::string file_name, std::ifstream &file);
// void    read_file_to_vect(std::ifstream   &configfile);
// void    close_file(std::ifstream &file);
// void    check_argv();
// void    parsing(int argc, char **argv);
// template<typename T>

namespace WSERV
{
	size_t      skip_whitespace(std::string &line, size_t start);
	size_t      ascii_counter(std::string line);
	int         search_for_double_brackets(std::string line);

	void 		close_file(std::ifstream &file);
	void 		open_file(std::string file_name, std::string &str);
	void 		check_argc(int argc);
	
	class Parser
	{
		private:
			std::vector<WSERV::serverConfig>	_vec_servers;
			std::string		_config_name;
			std::string		_configfile;
			std::string 	_separator;
			size_t			 _prev_loc_count;
			size_t 			_server_count;
			bool			_in_sev_bloc;
			bool 			_in_loc_bloc;
			std::vector<std::map<std::string, std::string> > _serv_map_vec;
			std::vector<std::vector<std::map<std::string, std::string> > >_loc_map_vec;
		public:
			Parser(int argc, char **argv);
			virtual ~Parser();
			Parser(Parser const & copy); //do we need these?
			Parser & operator = (Parser const & copy);
			void	read_file_to_vect();
			void    add_vector_vars_to_server_class();
			bool    server_block_errorcheck_pushback(std::string &line, \
					std::map<std::string, std::string> &serv_vars);
			bool	location_block_errorcheck_pushback(std::string &line, \
					std::vector<std::map<std::string, std::string> > &temp);
			bool	closing_location_block_check(std::string &line, \
					std::map<std::string, std::string> &loc_vars, \
					std::vector<std::map<std::string, std::string> > &temp);
			bool	closing_server_block_check(std::string &line);
			std::vector<WSERV::serverConfig>	get_serverConfig(void) const;
	};
}



#endif
