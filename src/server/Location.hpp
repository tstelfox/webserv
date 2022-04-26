/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Location.hpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: akramp <akramp@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/04/15 17:44:37 by akramp        #+#    #+#                 */
/*   Updated: 2022/04/21 22:00:10 by ubuntu        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
# define LOCATION_HPP
# include <string>

namespace WSERV
{
    class Location
    {
        private:
            std::string _root;
			std::string _location_path;//
			bool 		_autoindex;
			std::string _allow_method;
			std::string _index;
			std::string _error_page; //
			int 		_max_file_size;
			bool 		_cgi_allowed_extensions;//
			std::string _default_cgi_path;//
			std::string _php_cgi;//
			bool		 _auth_basic;
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
			std::string get_allow_method( void ) const;
			std::string get_index( void ) const;
			std::string get_error_page( void ) const;
			int 		get_max_file_size( void ) const;
			bool 		get_cgi_allowed_extensions( void ) const;
			std::string get_default_cgi_path( void ) const;
			std::string get_php_cgi( void ) const;
			bool get_auth_basic( void ) const;

			void set_root( std::string const data);
			void set_location_path( std::string const data);
			void set_autoindex( bool const data);
			void set_allow_method( std::string const data);
			void set_index( std::string const data);
			void set_error_page( std::string const data);
			void set_max_file_size( int const data);
			void set_cgi_allowed_extensions( bool const data);
			void set_default_cgi_path( std::string const data);
			void set_php_cgi( std::string const data);
			void set_auth_basic(bool const data);
	};
}


#endif
