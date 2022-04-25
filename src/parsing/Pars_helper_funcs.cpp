/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Pars_helper_funcs.cpp                              :+:    :+:            */
/*                                                     +:+                    */
/*   By: akramp <akramp@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/04/25 11:37:45 by akramp        #+#    #+#                 */
/*   Updated: 2022/04/25 12:04:53 by akramp        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Parsing.hpp"

void WSERV::close_file(std::ifstream &file)
{
    file.close();
}

void WSERV::open_file(std::string file_name, std::string &str)
{
    std::ifstream file;
    file.open(file_name.c_str());
    if (!(file.is_open()))
        throw OpenFileExcep();
    if(file)
    {
        std::ostringstream ss;
        ss << file.rdbuf(); // reading data
        str = ss.str();
    }
    close_file(file);
}

void WSERV::check_argc(int argc)
{
    if (argc > 2)
        throw TooManyArgsExcep();
    if (argc < 2)
        throw TooLittleArgsExcep();
}

size_t    WSERV::skip_whitespace(std::string &line, size_t start)
{
    while(std::isspace(line[start]))
            start++;
    return (start);
}

size_t  WSERV::ascii_counter(std::string line)
{
    int count = 0;
    int special_case_count = 0;
    bool special_case = false;
    unsigned int char_num = 0;

    for (unsigned long i = 0; i < line.length(); i++)
    {
        char_num = line[i];
        if (isspace(char_num) == false)
        {
            if ((special_case_count % 4 == 0 || i + 1 == line.length()) && special_case == true)
            {
                special_case = false;
                count = count - special_case_count;
                special_case_count = 0;
                if (i + 1 != line.length())
                    count++;
            }
            if (char_num < 32 || char_num > 255)
            {
                special_case_count++;
                special_case = true;
            }
            count++;
        }
    }
    return (count);
}

int    WSERV::search_for_double_brackets(std::string line)
{
    int bracket_count = 0;

    for (unsigned long i = 0; i < line.length(); i++)
    {
        if (line[i] == '{' || line[i] == '}')
            bracket_count++;
    }
    if (bracket_count != 1)
        return (-1);
    return (0);
}