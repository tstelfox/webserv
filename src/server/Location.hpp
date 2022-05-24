/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Location.hpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: akramp <akramp@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/04/15 17:44:37 by akramp        #+#    #+#                 */
/*   Updated: 2022/05/18 14:33:48 by akramp        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
# define LOCATION_HPP
# include <string>
# include <map>
# include <utility>

namespace WSERV
{
    class Location
    {
        private:
            std::string _root;
			std::string _location_path;
			bool 		_autoindex;
			std::map<int, std::string> _allow_method;
			std::string _index;
			std::string _error_page; //
			unsigned int 		_max_file_size;
			std::string _cgi;//
			bool		 _auth_basic;
			std::pair<std::string, std::string>	_redirect;
        public:
            Location(/* args */);
            ~Location();
			Location(Location const & copy);
			Location & operator = (Location const & copy);
			friend class Server;
			friend class Parser;
			std::string get_root( void ) const;
			std::string get_location_path( void ) const;
			bool 		get_autoindex( void ) const;
			std::map<int, std::string> get_allow_method( void ) const;
			std::string get_index( void ) const;
			std::string get_error_page( void ) const;
			unsigned int 		get_max_file_size( void ) const;
			bool get_auth_basic( void ) const;
			std::pair<std::string, std::string> get_redirect( void ) const;
			std::string get_cgi( void ) const;

			void set_root( std::string const data);
			void set_location_path( std::string const data);
			void set_autoindex( bool const data);
			void set_allow_method( std::map<int, std::string> const & data);
			void set_index( std::string const data);
			void set_error_page( std::string const data);
			void set_max_file_size( unsigned int const data);
			void set_auth_basic(bool const data);
			void set_redirect(std::pair<std::string, std::string> const &data );
			void set_cgi(std::string const data);
	};
}


#endif
