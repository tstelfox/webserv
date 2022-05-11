/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Pars_setters.cpp                                   :+:    :+:            */
/*                                                     +:+                    */
/*   By: akramp <akramp@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/04/25 10:33:46 by akramp        #+#    #+#                 */
/*   Updated: 2022/05/09 15:03:00 by ubuntu        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Parsing.hpp"
#include "exceptions.hpp"

void set_port_func(WSERV::serverConfig  &S_temp, std::string data)
{
    int start = 0;
    std::string delimiter = " ";
    int end = data.find(delimiter);
    std::vector<int> ports;
    while (end != -1)
    {
        ports.push_back(std::atoi((data.substr(start, end - start).c_str())));
        start = end + delimiter.size();
        end = data.find(delimiter, start);
    }
    ports.push_back(std::atoi((data.substr(start, end - start).c_str())));
    S_temp.set_port(ports);
}

void set_host_func(WSERV::serverConfig  &S_temp, std::string data)
{
    if (S_temp.get_host().empty() == false)
        throw IncorrectConfigExcep();
    if (data == "localhost")
        S_temp.set_host( "127.0.0.1" );
    else
        S_temp.set_host( data );
}

void set_server_name_func(WSERV::serverConfig  &S_temp, std::string data)
{
    S_temp.set_server_name( data );
}

void set_Location_vec_func(WSERV::serverConfig  &S_temp, std::vector<WSERV::Location> data) //!
{
    S_temp.set_Location_vec( data );
}

void set_maxfilesize_func(WSERV::serverConfig  &S_temp, std::string data)
{
    S_temp.set_maxfilesize(std::atoi( data.c_str() ));
}

void set_time_out_func(WSERV::serverConfig  &S_temp, std::string data)
{
    S_temp.set_time_out(std::atoi( data.c_str() ));;
}

void set_error_page_func(WSERV::serverConfig  &S_temp, std::string data)
{
    S_temp.set_error_page( data );
}

void set_cgi_file_types_func(WSERV::serverConfig  &S_temp, std::string data)
{
    S_temp.set_cgi_file_types( data );
}

void set_root_func(WSERV::Location  &L_temp, std::string data)
{
    L_temp.set_root( data );
}

void set_loc_path(WSERV::Location  &L_temp, std::string data)
{
    L_temp.set_location_path( data );
}

void set_autoindex_func(WSERV::Location  &L_temp, std::string data)
{
    bool convert;

    if (!data.compare(0, 2, "on"))
        convert = true;
    else if (!data.compare(0, 3, "off"))
        convert = false;
    else
        throw IncorrectConfigExcep();
    L_temp.set_autoindex( convert );
}

void set_allow_method_func(WSERV::Location  &L_temp, std::string data)
{
    L_temp.set_allow_method( data );
}

void set_index_func(WSERV::Location  &L_temp, std::string data)
{
    L_temp.set_index( data );
}


void set_max_file_size_func(WSERV::Location  &L_temp, std::string data)
{
    L_temp.set_max_file_size( std::atoi(data.c_str()) );
}

void set_auth_basic_func(WSERV::Location  &L_temp, std::string data)
{
    bool convert;

    if (data.compare(0, 2, "on"))
        convert = true;
    else if (data.compare(0, 3, "off"))
        convert = false;
    else
        throw IncorrectConfigExcep();
    L_temp.set_auth_basic( convert );
}
