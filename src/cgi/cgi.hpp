/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   cgi.hpp                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: akramp <akramp@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/18 14:48:51 by akramp        #+#    #+#                 */
/*   Updated: 2022/05/18 19:44:13 by akramp        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
# define CGI_HPP
# include <string>
# include "../server/poller.hpp"

namespace WSERV {
    class   Cgi
    {
        private:
            std::string _path;
            int _cgi_in_fd;
            int _cgi_out_fd;
        public:
            Cgi(std::string path);
            ~Cgi();
            Cgi(Cgi const & copy);
			Cgi & operator = (Cgi const & copy);
            friend poller; //?
            
            std::string get_path(void) const;
            int get_cgi_fd(void) const;

            void set_path(std::string const data);
            void set_cgi_fd(int const data);
            void open_file();
    };
}

#endif