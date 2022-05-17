/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Pars_setters.cpp                                   :+:    :+:            */
/*                                                     +:+                    */
/*   By: akramp <akramp@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/04/25 10:33:46 by akramp        #+#    #+#                 */
/*   Updated: 2022/05/17 11:53:11 by akramp        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Parsing.hpp"
#include "exceptions.hpp"
#include <map>
#include <utility>

void set_port_func(WSERV::serverConfig  &S_temp, std::string data)
{
    // int start = 0;
    // std::string delimiter = " ";
    // int end = data.find(delimiter);
    // std::vector<int> ports;
    // while (end != -1)
    // {
    //     ports.push_back(std::atoi((data.substr(start, end - start).c_str())));
    //     start = end + delimiter.size();
    //     end = data.find(delimiter, start);
    // }
    // ports.push_back(std::atoi((data.substr(start, end - start).c_str())));
    if (data.find(" ") != std::string::npos)
    {
        std::cerr << COLOR_RED << "ONLY ONE PORTS PLS ..." << std::endl;
        throw IncorrectConfigExcep();
    }
    S_temp.set_port(std::atoi(data.c_str()));
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
    if (data.find(" ") != std::string::npos)
        throw IncorrectConfigExcep();
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
    std::map<int, std::string> temp;
    size_t pos = 0;
    size_t count = 1;
    size_t i = 0;
    std::string temp_s;

    pos = data.find(" ");
    while (i < data.length())
    {
        temp_s = data.substr(i, pos-i);
        if (temp_s == "GET")
            count = 1;
        else if (temp_s == "POST")
            count = 2;
        else if (temp_s == "DELETE")
            count = 3;
        else
        {
            std::cerr << COLOR_RED << "\n!!! ONLY \"GET\" \"POST\" \"DELETE\" ARE ALLOWED METHODS !!!" << std::endl;
            throw IncorrectConfigExcep();
        }    
        temp.insert(std::make_pair(count, temp_s));
        if (pos == std::string::npos)
            break ;
        i = pos;
        pos = data.find(" ", i+1);
        if (pos > data.length())
            pos = data.length();
        i++;
    }
    L_temp.set_allow_method( temp );
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

void WSERV::Parser::set_redirection(WSERV::Location  &L_temp, std::string data, std::string loc_path)
{
    std::pair<std::string, std::string> temp;
    size_t find_space = data.find(" ");
    std::string temp_s = loc_path;

    temp = std::make_pair(temp_s, data.substr(find_space+1, data.length() - find_space+1));
    L_temp.set_redirect(temp);
}