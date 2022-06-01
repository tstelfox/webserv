/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Parsing.cpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: akramp <akramp@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/04/07 17:51:46 by akramp        #+#    #+#                 */
/*   Updated: 2022/06/01 20:29:01 by ask           ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Parsing.hpp"

void set_port_func(WSERV::serverConfig  &S_temp, std::string data);
void set_host_func(WSERV::serverConfig  &S_temp, std::string data);
void set_server_name_func(WSERV::serverConfig  &S_temp, std::string data);
void set_Location_vec_func(WSERV::serverConfig  &S_temp, std::vector<WSERV::Location> data);
void set_error_page_func(WSERV::serverConfig  &S_temp, std::string data);
void set_cgi_func(WSERV::Location  &L_temp, std::string data);
void set_root_func(WSERV::Location  &L_temp, std::string data);
void set_autoindex_func(WSERV::Location  &L_temp, std::string data);
void set_allow_method_func(WSERV::Location  &L_temp, std::string data);
void set_index_func(WSERV::Location  &L_temp, std::string data);
void set_max_file_size_func(WSERV::Location  &L_temp, std::string data);
void set_loc_path(WSERV::Location  &L_temp, std::string data);

std::vector<std::string> WSERV::Parser::get_loc_path(void) const
{
    return (this->_loc_path);
}

std::vector<WSERV::serverConfig>	WSERV::Parser::get_serverConfig(void) const
{
    return (this->_vec_servers);
}


void    WSERV::Parser::read_file_to_vect()
{
    std::string line;
    std::map<std::string, std::string> serv_vars;
    std::map<std::string, std::string> loc_vars;
    std::string str_l;
    std::string str_r;
    std::vector<std::map<std::string, std::string> > temp;
    size_t start = 0;
    std::istringstream iss(_configfile);
    
    _separator = "🔥";
    _prev_loc_count = 1;
    _server_count = 0;
    _in_sev_bloc = false;
    _in_loc_bloc = false;

    while (std::getline(iss, line))
    {
        size_t i = 0;

        /*  Error checks and when at the end of a block{}
            add it to the vector or to the map                      */
        if (server_block_errorcheck_pushback(line, serv_vars) == true)
            continue;
        if (location_block_errorcheck_pushback(line, temp) == true)
            continue;
        if (closing_location_block_check(line, loc_vars, temp) == true)
            continue;
        if (closing_server_block_check(line) == true)
            continue;

        /*  Goes to start and skips empty lines            uwu      */
        start = skip_whitespace(line, i);
        i = start;
        if (start == line.length())
            continue ;

        /*  finds and removes fire emoji, since unicode is a b*tch.
            Also adds the left and right side to temp vars          */
        i = line.find("🔥", 0);
        if (i == std::string::npos || i > line.length())
        {
            if (line.find("}") != std::string::npos)
                throw IncorrectConfigExcep();
            i = line.length();
        }
        str_r = line.substr(start, i - start);
        line.erase(i,_separator.length()); //delete fire emoji
        str_l = line.substr(i, line.length()-i);

        /*  Adds the temp vars into pairs of the map
            that will later be added to a vector of maps            */
        if (str_r.length() != 0 && str_l.length() != 0 && _in_loc_bloc == 0)
        {
            if(serv_vars.insert(std::pair<std::string, std::string>(str_r, str_l)).second == false)
                throw IncorrectConfigExcep();
        }
        else if (str_r.length() != 0 && str_l.length() != 0 && _in_loc_bloc > 0)
            if (loc_vars.insert(std::pair<std::string, std::string>(str_r, str_l)).second == false)
                throw IncorrectConfigExcep();
    }
    /*  checks if the brackets are not closed properly              */
    if (_in_sev_bloc == true || _in_loc_bloc == true)
        throw IncorrectConfigExcep();

    /*  Adds the serever to a vector of maps
        and location to a vector of vector of maps                  */
    _loc_map_vec.push_back(temp);
    _serv_map_vec.push_back(serv_vars);
}

void    WSERV::Parser::add_vector_vars_to_server_class()
{
    serverConfig  *S_temp;
    int loc_count = 0;
    std::vector<Location> *vec_loc_temp;
    std::string cmp_serv[] = {"port", "host", "server_name", "error_page", "location"};
    std::string cmp_loc[] = {"root", "allowed_method", "index", "autoindex", \
        "max_file_size", "cgi", "return"}; //location path
    void (*set_funcs_serv[])(WSERV::serverConfig&, std::string) = {&set_port_func, \
        &set_host_func, &set_server_name_func, &set_error_page_func};
    void (*set_funcs_loc[])(WSERV::Location&, std::string) = {&set_root_func, \
        &set_allow_method_func, &set_index_func, &set_autoindex_func, \
        &set_max_file_size_func, &set_cgi_func};

    for (unsigned long servs = 0; servs < _serv_map_vec.size(); servs++)
    {
        S_temp = new serverConfig;
        vec_loc_temp = new std::vector<Location>;
        // int count = 0;
        for (std::map<std::string,std::string>::iterator it=_serv_map_vec[servs].begin(); it!=_serv_map_vec[servs].end(); ++it)
        {
            for (int var_name = 0; (unsigned int) var_name < (sizeof(cmp_serv)/sizeof(cmp_serv[0])); var_name++)
            {
                if (cmp_serv[var_name].compare(0, cmp_serv[var_name].length(), it->first) == 0)
                    set_funcs_serv[var_name](*S_temp, it->second);
            }
        }
        for (unsigned long locs = 0; locs < _loc_map_vec[servs].size(); locs++)
        {
            Location *L_temp;

            L_temp = new Location;
            _loc_count = loc_count;
            for (std::map<std::string,std::string>::iterator it=_loc_map_vec[servs][locs].begin(); it!=_loc_map_vec[servs][locs].end(); ++it)
            {
                for (int var_name = 0; (unsigned int) var_name < (sizeof(cmp_loc)/sizeof(cmp_loc[0])); var_name++)
                {
                    if (cmp_loc[var_name].compare(0, cmp_loc[var_name].length(), it->first) == 0)
                    {
                        if (cmp_loc[var_name] == "return")
                            set_redirection(*L_temp, it->second, _loc_path[_loc_count]);
                        else
                            set_funcs_loc[var_name](*L_temp, it->second);
                    }
                }
            }
            set_loc_path(*L_temp, _loc_path[loc_count]);
            loc_count++;
            (*vec_loc_temp).push_back(*L_temp);
            delete L_temp;
        }
        set_Location_vec_func(*S_temp, *vec_loc_temp);
        _vec_servers.push_back(*S_temp);
        delete vec_loc_temp;
        delete S_temp;
    }

    /* */
}

void WSERV::Parser::check_if_var_in_class_is_empty()
{
    for (unsigned int i = 0; i < _vec_servers.size(); i++)
    {
        if (_vec_servers[i].get_port() == 0)
            throw IncorrectConfigExcep();
        if (_vec_servers[i].get_host().empty() == true)
            throw IncorrectConfigExcep();
        if (_vec_servers[i].get_error_page().empty() == true) 
            _vec_servers[i].set_error_page("pages/errorPages/");
        if (_vec_servers[i].get_Location_vec().empty() == true)
            throw IncorrectConfigExcep();
    }

}

WSERV::Parser::Parser(int argc, char **argv)
{
    check_argc(argc);
    _config_name = argv[1];
    open_file(_config_name, _configfile);
    read_file_to_vect();
    add_vector_vars_to_server_class();
    check_if_var_in_class_is_empty();
}

WSERV::Parser::~Parser() {}

WSERV::Parser::Parser(Parser const & copy)
{
	*this = copy;
}

WSERV::Parser & WSERV::Parser::operator = (Parser const & copy)
{
	this->_vec_servers = copy._vec_servers;
	this->_config_name = copy._config_name;
	this->_configfile = copy._configfile;
    this->_loc_path = copy._loc_path;
	this->_separator = copy._separator;
	this-> _prev_loc_count = copy. _prev_loc_count;
	this->_server_count = copy._server_count;
	this->_in_sev_bloc = copy._in_sev_bloc;
	this->_in_loc_bloc = copy._in_loc_bloc;
	this->_serv_map_vec = copy._serv_map_vec;
	this->_loc_map_vec = copy._loc_map_vec;
	return *this;
}
