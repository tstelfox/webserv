/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   serverConfig.hpp                                   :+:    :+:            */
/*                                                     +:+                    */
/*   By: turloughmullan <turloughmullan@student.      +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/04/25 19:42:04 by turloughmul   #+#    #+#                 */
/*   Updated: 2022/05/18 14:35:24 by akramp        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */


#ifndef WEBSERV_SERVERCONFIG_HPP
#define WEBSERV_SERVERCONFIG_HPP

# include "Location.hpp"
# include <cstring>
# include <map>
# include <set>
# include <vector>


namespace WSERV {
    class serverConfig {
    private:
        int _port; // std::set<int>        _port; In the future
        std::string _host;
        std::string _server_name;
        unsigned int _maxfilesize;
        std::string _error_page;
        int _time_out;
        std::vector<WSERV::Location> _Location_vec;

    public:
        serverConfig();

        ~serverConfig();

        serverConfig(serverConfig const &copy);

        serverConfig &operator=(serverConfig const &copy);

        friend class Parser;

        int get_port() const;

        std::string get_host() const;

        std::string get_server_name() const;

        unsigned int get_maxfilesize() const;

        std::string get_error_page() const;

        int get_time_out() const;

        std::vector<WSERV::Location> get_Location_vec() const;

        void set_port(int const data);

        void set_host(std::string const &data);

        void set_server_name(std::string const data);

        void set_Location_vec(std::vector<WSERV::Location> const data);

        void set_maxfilesize(int const data);

        void set_time_out(int const data);

        void set_error_page(std::string const data);
    };
}

#endif //WEBSERV_SERVERCONFIG_HPP
