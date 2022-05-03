/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   server_main.cpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/02/03 15:40:57 by tmullan       #+#    #+#                 */
/*   Updated: 2022/04/21 20:48:09 by tmullan       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
//#include "rubbish/serverBlock.hpp"
#include "poller.hpp"
#include "serverConfig.hpp"
#include "Parsing.hpp"
#include "colours.hpp"

#include <iostream>
#include <unistd.h>
#include <sstream>
#include <fstream>

void parsedContents(std::vector<WSERV::serverConfig> const& S) {
        for (size_t i = 0; i < S.size(); i++) {
        std::cout << RED << "< ---------  Server Block --------- >" << RESET_COLOUR << std::endl;
        std::cout << "port= " << S[i].get_port() << std::endl;
        std::cout << "host= " << S[i].get_host() << std::endl;
        std::cout << "server_name= " << S[i].get_server_name() << std::endl;
        std::cout << "maxfilesize= " << S[i].get_maxfilesize() << std::endl;
        std::cout << "error_page= " << S[i].get_error_page() << std::endl;
        std::cout << "cgi_file_types= " << S[i].get_cgi_file_types() << std::endl;
        std::cout << "time_out= " << S[i].get_time_out() << std::endl;
        std::cout << std::endl;
        for (size_t x = 0; x < S[i].get_Location_vec().size(); x++) {
            std::cout << GREEN << "< ----- Location [" << x << "] -------->" << RESET_COLOUR << std::endl;
            std::cout << "root= " << S[i].get_Location_vec()[x].get_root() << std::endl;
            std::cout << "location_path= " << S[i].get_Location_vec()[x].get_location_path() << std::endl;
            std::cout << "autoindex= " << S[i].get_Location_vec()[x].get_autoindex() << std::endl;
            std::cout << "allow_method= " << S[i].get_Location_vec()[x].get_allow_method() << std::endl;
            std::cout << "index= " << S[i].get_Location_vec()[x].get_index() << std::endl;
            std::cout << "error_page= " << S[i].get_Location_vec()[x].get_error_page() << std::endl;
            std::cout << "max_file_size= " << S[i].get_Location_vec()[x].get_max_file_size() << std::endl;
            std::cout << "cgi_allowed_extensions= " << S[i].get_Location_vec()[x].get_cgi_allowed_extensions()
                      << std::endl;
            std::cout << "default_cgi_path= " << S[i].get_Location_vec()[x].get_default_cgi_path() << std::endl;
            std::cout << "php_cgi= " << S[i].get_Location_vec()[x].get_php_cgi() << std::endl;
            std::cout << "auth_basic= " << S[i].get_Location_vec()[x].get_auth_basic() << std::endl;
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
}

int main(int argc, char *argv[]) {

    std::cout << "Starting..." << std::endl;
    std::vector<WSERV::serverConfig> S;
    // WSERV::Server serv(argc, argv);
    try {
        WSERV::Parser p(argc, argv);
        S = p.get_serverConfig();

    }
    catch (const std::exception &e) {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }
    /*Print all configs*/
    parsedContents(S);

    /*Extract all the unique ports and put them into a set.*/

    poller littyServer(S);
    littyServer.pollConnections();

    /*Functioning main pre restructure*/
//    serverSock hello(AF_INET, SOCK_STREAM, 0, 8080, INADDR_ANY);
//    serverBoy littyServer(hello);
//
//
//    littyServer.runServer();
//    std::cout << "Exiting via here?" << std::endl;
    return 0;
}
