/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   server_main.cpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/02/03 15:40:57 by tmullan       #+#    #+#                 */
/*   Updated: 2022/05/18 14:45:50 by akramp        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
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
        std::cout << "time_out= " << S[i].get_time_out() << std::endl;
        std::cout << std::endl;
        for (size_t x = 0; x < S[i].get_Location_vec().size(); x++) {
            std::cout << GREEN << "< ----- Location [" << x << "] -------->" << RESET_COLOUR << std::endl;
            WSERV::Location  L_temp = S[i].get_Location_vec()[x];
            std::cout << "root= " << L_temp.get_root() << std::endl;
            std::cout << "location_path= " << L_temp.get_location_path() << std::endl;
            std::cout << "autoindex= " << L_temp.get_autoindex() << std::endl;
            std::map<int, std::string>::const_iterator it = L_temp.get_allow_method().begin();
            std::map<int, std::string> temp = L_temp.get_allow_method();
            for (it = temp.begin();
                it != temp.end(); ++it)
            {
                std::cout << it->first << " " << it->second << " " << "\n";
            }
            std::cout << "index= " << L_temp.get_index() << std::endl;
            std::cout << "error_page= " << L_temp.get_error_page() << std::endl;
            std::cout << "max_file_size= " << L_temp.get_max_file_size() << std::endl;
            std::cout << "cgi= " << L_temp.get_cgi() << std::endl;
            std::cout << "auth_basic= " << L_temp.get_auth_basic() << std::endl;

            std::pair <std::string, std::string> temp_pair = L_temp.get_redirect();
            std::cout << "redirect= " << temp_pair.first << " " << temp_pair.second << std::endl;
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
//   parsedContents(S);

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
