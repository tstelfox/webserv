/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Pars_add_vars_to_vec.cpp                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: akramp <akramp@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/04/25 12:00:13 by akramp        #+#    #+#                 */
/*   Updated: 2022/05/09 16:52:07 by ubuntu        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Parsing.hpp"

bool    WSERV::Parser::server_block_errorcheck_pushback(std::string &line, std::map<std::string, std::string> &serv_vars)
{
    if (line.find("server") != std::string::npos && line.find("{") != std::string::npos)
    {
        /*  Searches for garbo and double brackets                  */
        if (search_for_double_brackets(line) == -1 || ascii_counter(line) != 7)
            throw IncorrectConfigExcep();

        /*  Add server map to vector                                */
        if (_server_count > 0)
        {
            _serv_map_vec.push_back(serv_vars);
            serv_vars.clear();
        }
        _server_count++;

        /*  Check if the bracket is still open                      */
        if (_in_sev_bloc == true)
            throw IncorrectConfigExcep();
        _in_sev_bloc = true;
        return (true);
    }
    if (line.find("server") != std::string::npos && line.find("{") == std::string::npos)
    {
        /*  Check if server has no extra letters around it          */
        if (ascii_counter(line) == 6)
            throw IncorrectConfigExcep();
    }
    if (_server_count == 0)
    {
        unsigned long i = 0;
        /*  Skips empty lines at the start of file                   */
        for (i = 0; std::isspace(line[i]); i++){}
        if(i == line.length() || line.empty())
            return (true);
        /*  Else throws, if there are no server blocks               */
        throw IncorrectConfigExcep();
    }
    return (false);
}

bool WSERV::Parser::location_block_errorcheck_pushback(std::string &line, \
    std::vector<std::map<std::string, std::string> > &temp)
{
    if (line.find("location") != std::string::npos && line.find("{") != std::string::npos)
    {
        /*  Check for double brackets                       */
        if (search_for_double_brackets(line) == -1)
            throw IncorrectConfigExcep();

        /*  Checks for if location has garbo around it      */
        if (ascii_counter(&line[line.find("{")] + 1) != 0 || (line.find("🔥") - line.find("location") != 8) \
            || skip_whitespace(line, 0) != line.find("location"))
            throw IncorrectConfigExcep();

        /*  Check if location block is closed               */
        if (_in_loc_bloc == true)
            throw IncorrectConfigExcep();
        _in_loc_bloc = true;

        /*  Adds the location temnp vector of maps to the servers vector*/
        if (_prev_loc_count < _server_count)
        {
            _loc_map_vec.push_back(temp);
            temp.clear();
        }
        int i = line.find("🔥", 0);
        line.erase(i,_separator.length()); //delete fire emoji
        std::string temp_str = line.substr(i, line.find("{") - 1);
        if (temp_str.find(" ") != std::string::npos)
            temp_str.erase(temp_str.find(" "), temp_str.size());
        if (temp_str.find("{") != std::string::npos)
            temp_str.erase(temp_str.find("{"), temp_str.size());
        _loc_path.push_back(temp_str);
        return (true) ;
    }
    return (false);
}

/*  This function check if the clsoing bracket is without
    wrong input around it. And adds the map to the vector
    temp, So it can later be add to the vector that holds
    all the vectors that contain all the locations per
    server.                                                 */
bool    WSERV::Parser::closing_location_block_check(std::string &line, \
    std::map<std::string, std::string> &loc_vars, \
    std::vector<std::map<std::string, std::string> > &temp)
{
    if (_in_loc_bloc == true && line.find("}") != std::string::npos)
    {
        if (search_for_double_brackets(line) == -1)
            throw IncorrectConfigExcep();
        temp.push_back(loc_vars);
        loc_vars.clear();
        _prev_loc_count = _server_count;
        _in_loc_bloc = false;
        if (line.find("}") + 1 != line.length() || skip_whitespace(line, 0) != line.find("}"))
            throw IncorrectConfigExcep();
        return (true);
    }
    return (false);
}

/*  This functions does the same as the one above,
    except it just adds it to a vector of maps
    of the servers                                        */
bool    WSERV::Parser::closing_server_block_check(std::string &line)
{
    if (_in_sev_bloc == true && _in_loc_bloc == false && line.find("}") != std::string::npos)
    {
        if (search_for_double_brackets(line) == -1)
            throw IncorrectConfigExcep();
        _in_sev_bloc = false;
        if (line.find("}") + 1 != line.length() || skip_whitespace(line, 0) != line.find("}"))
            throw IncorrectConfigExcep();
        return (true);
    }
    return (false);
}
