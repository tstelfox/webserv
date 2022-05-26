/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   responseHandler.cpp                                  :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/01 18:30:39 by tmullan       #+#    #+#                 */
/*   Updated: 2022/05/01 18:30:39 by tmullan       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "responseHandler.hpp"
#include "cgi.hpp"
#include "colours.hpp"
#include <iostream>
#include <map>
#include <fstream>
#include <sstream>
#include <sys/stat.h> // For checking if directory
#include <dirent.h> // For getting directory contents
#include <time.h> // time for time
#include <stdio.h> // remove()
#include <unistd.h>


responseHandler::responseHandler(std::string requestLine, WSERV::serverConfig const &configs,
                                 std::map <std::string, std::string> &fields, std::string body)
        : _requestLine(requestLine), _config(configs), _requestFields(fields), _body(body), _isCGI(false), _cgiFd(-1) {}

responseHandler::responseHandler() {}

responseHandler::~responseHandler() {}

std::string responseHandler::parseAndRespond(int status, int method, std::string uri) {
    if (status != 200)
        return respondError(status);
    matchLocation(uri);
    std::map<int, std::string> allowedMethod = _location.get_allow_method();

//    for (std::map<int, std::string>::iterator it = allowedMethod.begin(); it != allowedMethod.end(); it++)
//        std::cout << RED << "UEEE " << it->first << " ohhh " << it->second << RESET_COLOUR << std::endl;
//    std::cout << RED << "AOOOOOOO " << allowedMethod.size() << RESET_COLOUR <<std::endl;
//    std::cout << "Requested method is " << method << " And there is " << allowedMethod.count(method) << std::endl;


    /* if (_method is not in location list of methods)
        set method to 0 and triggers a 405 */
    if (allowedMethod.empty()) {
        allowedMethod[1] = "GET";
        allowedMethod[2] = "POST";
    }
    if (allowedMethod.count(method) == 0) {
        std::cout << "That method is not allowed yo" << std::endl;
        method = 0;
    }
    /* Parsing method */
    std::cout << "Request Line is: " << _requestLine << std::endl;
    switch (method) {
        case 0:
            return respondError(405);
        case 1:
            return getResponse(uri);
        case 2:
            if (_body.size() > _location.get_max_file_size())
                return respondError(413);
            return postResponse(uri);
        case 3:
            return deleteResponse(uri);
        default:
            return respondError(405);
    }
}

int responseHandler::matchLocation(std::string uri) {
    std::vector <WSERV::Location> locationsVec = _config.get_Location_vec();
    WSERV::Location location;
    int success = 0;

    bool aMatch = false;
    for (std::vector<WSERV::Location>::iterator locIter = locationsVec.begin();
         locIter != locationsVec.end(); locIter++) {
        std::string path = locIter->get_location_path();
//        std::cout << "The fucking path is: " << path << std::endl;
        /* Exact match */
        if (!uri.compare(path)) {
            std::cout << GREEN << "Exact Location match" << RESET_COLOUR << std::endl;
            location = *locIter;
            aMatch = true;
            success = 1;
            break;

        }
        /* Check if the first part of the uri is an exact match of the location */
//        std::cout << COLOR_HOTPINK << "URI: " << uri << " and path: [" << path << "]" << RESET_COLOUR << std::endl;
//        std::cout << COLOR_DARKPINK << "Comparison: " << uri.compare(0, path.size(), path) << RESET_COLOUR << std::endl;
        if ((path.length() > 1 && !uri.compare(0, path.size(), path))
            || (path.length() == 1 && std::count(uri.begin(), uri.end(), '/') == 1)) {
            std::cout << GREEN << "Partial match" << RESET_COLOUR << std::endl;
            location = *locIter;
        }

    }
    _location = location;
    std::cout << "The correct location is: " << _location.get_location_path() << std::endl;

    return success;
}

     /*
     * nginx behaviour:
     *      - Directory listing only if there is no set index for that directory
     *      - root is where to go retrieve the files by adding location at the end
     *      - root cannot be a file
     *      - root is obligatory for us
     *
     *      SO WE WILL DO
     *      - compare uri with location
     *      - Check for root (obligatory)
     *      - Check if there is an index
     *          - If uri leads to root which is a folder without index then directory listing
     *          - If uri leads to root which is a folder with index then display index
     *
     *
     * */

std::string responseHandler::getResponse(std::string const& uri) {

    /* TODO clean up and split this func */

    std::string redirection = _location.get_redirect().first;
    if (!redirection.empty()) {
        std::cout << "Diocane " << "[" << redirection << "]" << std::endl;
        if (uri == redirection) {
            std::cout << "Redirection stuff for " << uri << " to " << _location.get_redirect().second << std::endl;
            return redirectionResponse(_location.get_redirect().second);
        }
    }

    std::cout << COLOR_BABYBLUE << "location root is: " << _location.get_root() <<
                " and, if present, index is: " << _location.get_index() << RESET_COLOUR << std::endl;

    std::string requestedPath = rootResolution(uri);

    std::cout << CYAN << "Correct full requested path is: " << requestedPath << " and the uri: " << uri << RESET_COLOUR << std::endl;

    /* Check for index -
        if there is no index
            if root is directory:
                directory listing
            else
                return 404 (It's what nginx does) [Update: not really]
        else
            look for root plus index
            */

    std::string requestedFile;
    if (isDirectory(requestedPath) && !_location.get_index().empty())
        requestedFile = requestedPath + _location.get_index();
    else
        requestedFile = requestedPath;
    if (requestedFile[0] == '/')
        requestedFile.erase(0, 1);
    std::cout << "Requested file is ultimately: " << requestedFile << std::endl;
    if (isDirectory(requestedFile)) {
        std::cout << "Should come here" << std::endl;
        // Directory Listing
        if (!_location.get_autoindex()) {
            return respondError(403);
        } else {
            return buildDirectoryListing(requestedFile);
        }
    }
    std::ifstream myFile;

    // TODO check if it is a cgi request.
    int cgiFd;
    cgiFd = cgiRequest(requestedFile);
    if (cgiFd != -1) {
        std::cout << "Panic: " << cgiFd << std::endl;
        _cgiFd = cgiFd;
        _isCGI = true;
        return "This-Is-a-CGI___frfr";
    }
    else if (cgiFd == -99)
        return respondError(404);

    myFile.open(requestedFile);
    if (myFile.fail()) {
//        std::cout << CYAN << "This here?: " << requestedFile << RESET_COLOUR << std::endl;
        return respondError(404);
    }

    std::ostringstream fileContent;
    fileContent << myFile.rdbuf();
    std::string responseBody = fileContent.str();
    myFile.close();
//    std::cout << "Opened content: " << fileContent << std::endl;

    std::string responseHeader;
    responseHeader = buildHttpLine(200);
    responseHeader += buildDateLine();
    responseHeader += "Content-type: text/html; charset=UTF-8\nContent-Length:";
    responseHeader.append(std::to_string(responseBody.size()));
    responseHeader.append("\n\n");
    std::cout << RED << "<<<<-------- The response header ------->>>>\n" << "RESPONSE: " << RESET_COLOUR
              << responseHeader << std::endl;
    responseHeader.append(responseBody + "\n");

    return responseHeader;
}

std::string responseHandler::postResponse(std::string const& uri) {

    std::string path = rootResolution(uri);
    std::string fileName = path + "/Madonna";

    while (std::ifstream(fileName))
        fileName += "Maiala";
    std::ofstream file(fileName);
    if (!file) {
        std::cout << "File creation failed" << std::endl;
        return respondError(404);
    }
    file << _body;
    file.close();
    std::string response = "HTTP/1.1 201 Created\n";
    response += buildDateLine() + "Location: http://" + _config.get_host() + ":" \
            + std::to_string(_config.get_port()) + "/" + fileName + "\n\n";

    return response;
}

std::string responseHandler::deleteResponse(std::string uri) {

    std::string filePath = rootResolution(uri);
    std::cout << RED << "Delete tae fack: " << filePath << RESET_COLOUR << std::endl;

    std::string response;
    // 404 not found
    // 410 gone (?)
    // 403 Forbidden (God knows how)
    if (remove(filePath.c_str())) {
        std::cout << "It didn't delete shit" << std::endl;
        std::cout << errno << std::endl;
        perror("wassup: ");
        if (errno == 2)
            return respondError(404);
    }
    else {
        response = "HTTP/1.1 200 OK\n" + buildDateLine() + "\n";
        response += "<html>\n   <body>\n        <h1>That shit has been duly yote.</h1>\n    </body>\n</html>";
    }
    std::cout << "Successful delete: " << response << std::endl;
    return response;
}

std::string responseHandler::respondError(int status) {

    std::cout << MAGENTA << "We be responding to an error: " << status << RESET_COLOUR << std::endl;
    std::string response;
    response = buildHttpLine(status);
    response += buildDateLine();

    //Content
    response += "Content-type: text/html; charset=UTF-8\nContent-Length:";
    // Extract file
    std::string body = extractErrorFile(status);
//    std::cout << "Error html: " << body << std::endl;
    response.append(std::to_string(body.size()));
    //Error files
    response += "\nConnection: close";

    //End of Header
    response.append("\n\n");
//    std::cout << RED << "<<<<-------- The response header ------->>>>\n" << RESET_COLOUR << response << std::endl;
    response.append(body + "\n");
    std::cout << RED << "<<<<-------- The response ------->>>>\n" << RESET_COLOUR << response << std::endl;
    return response;
}

/* < ---------- CGI HANDLER ---------- > */

int responseHandler::cgiRequest(std::string request) {

    if (request.find(".py?") == std::string::npos) {
        std::cout << "Nothing to see here, go about your business." << std::endl;
        return -1;
    }
    std::cout << RED << "Holy friggin shit boys it's a cgi" << RESET_COLOUR << std::endl;
    std::string executablePath = request.substr(0, request.find(".py") + 3); // hacky but lel
    std::cout << executablePath << " dioganaccio INFAME" << std::endl;

    std::string arguments = request.substr(request.find(".py?") + 4);
    std::cout << "The arguments themselves are: " << arguments << std::endl;
    // Make a vector of arguments?
    std::vector<std::string> args;
    size_t pos = arguments.find("&");
    args.push_back(arguments.substr(0, pos));
    arguments.erase(0, pos + 1);
    while (pos != std::string::npos) {
        pos = arguments.find("&");
        args.push_back(arguments.substr(0, pos));
    }
    for (size_t i = 0; i < args.size(); i++) {
        args[i] = args[i].substr(args[i].find("=") + 1);
        std::cout << args[i] << std::endl;
    }

    // Hacky but living for it
//    char buffer[5000];
//    read(*exec.get_cgi_fd(), &buffer, 5000);
//    std::cout << buffer << std::endl;
    int ret = -99;
    try {
        WSERV::Cgi exec(executablePath, args[0], args[1]);
        ret = &exec.get_cgi_fd();
    }
    catch (const std::exception &e) {
        std::cout << e.what() << std::endl;
        return -99;
    }

    return ret;


//    exec.get_cgi_fd();

//    for (std::vector<std::string>::iterator it = args.begin(); it != args.end(); it++) {
//        *it = it->substr(it->find("="), 0);
//        std::cout << "The arguments: " << *it << std::endl;
//    }
    /* get the path out of the location I guess (?)*/
    /* Extract the arguments */

    /* Cgi  cgiExec(path, arg1, arg2);
     * int output = cgiExec.get_cgi_fd();
     * */

    return 0; // Placeholder
}

/* < ---------- DIRECTORY LISTING ---------- > */

std::string responseHandler::buildDirectoryListing(std::string &directory) {
    std::cout << "Gotta make the directory page for: " << directory << std::endl;

    /* nginx lists directories first in alphabetical order then files in alphabetical order
     * Date and time of creation or modification for each file
     * And then size of file "-" for directories
     * */
    std::cout << "Building the directory listing for " << directory << std::endl;

    DIR *dh;
    struct dirent *contents;

    std::set <std::vector<std::string> > fileSet;
    std::set <std::vector<std::string> > directorySet;
    dh = opendir(directory.c_str());
    if (!dh)
        std::cout << "No such directory as " << directory << std::endl;
    else {
        while ((contents = readdir(dh)) != NULL) {
            std::string name = contents->d_name;
            if (!name.compare("."))
                continue;
            struct stat s;
            std::string path = directory + "/" + name;
            if (lstat(path.c_str(), &s) == 0) {
                std::vector <std::string> details;
                if (name.compare("..")) {
                    details.push_back(name);
                    struct tm *timeInfo = localtime(&s.st_ctime);
                    std::string date = std::to_string(timeInfo->tm_mday) + "-" + std::to_string(timeInfo->tm_mon) + "-" \
 + std::to_string(timeInfo->tm_year + 1900);
                    details.push_back(date);
                } else {
                    name += "/";
                    details.push_back(name);
                }
                if (S_ISDIR(s.st_mode)) {
                    if (name.compare("../")) {
                        details.push_back("-");
                    }

                    directorySet.insert(details);
                } else if (S_ISREG(s.st_mode)) {
                    details.push_back(std::to_string(s.st_size));
                    fileSet.insert(details);
                }
            }
        }
        closedir(dh);
    }

    std::string directoryResponse = directoryListResponse(directorySet, fileSet, directory);
    return directoryResponse;
}

/* < ---------- REDIRECTION RESPONSE ---------- > */

std::string responseHandler::redirectionResponse(std::string redirectionUri) {
    std::string redirectResponse = "HTTP/1.1 301 Moved Permanently\nLocation: ";

//    redirectResponse += _config.get_host() + ":";
//    redirectResponse += std::to_string(_config.get_port()[0]) + placeHolder + "\n\n";

    redirectResponse += redirectionUri + "\n\n";
    std::cout << redirectResponse << std::endl;
    return redirectResponse;
}

/* < ---------- RESPONSE HEADER BUILDING UTILS ---------- > */

std::string responseHandler::buildHttpLine(int status) {
    std::map<int, std::string> statusCodes;
    statusCodes[200] = "OK";
    statusCodes[400] = "Bad Request";
    statusCodes[403] = "Forbidden";
    statusCodes[404] = "Not Found";
    statusCodes[405] = "Method Not Allowed";
    statusCodes[413] = "Request Entity Too Large";
    statusCodes[415] = "Unsupported Media Type";
    statusCodes[505] = "HTTP Version Not Supported";

    std::string response = "HTTP/1.1 ";
    response += std::to_string(status) + " "; // C++11 oops fix this later
    response += statusCodes[status] + "\n";
    response += "Server: Flamenco \033[31m Flame \033[37m Server\n";
    return response;
}

std::string responseHandler::buildDateLine() {

    time_t now = time(0);
    char *date = ctime(&now); // TODO check if it leaks (Doesn't seem to)
    std::string stringDate = date;
    stringDate.insert(3, ",");
    stringDate.resize(stringDate.size() - 1);
    stringDate = "Date: " + stringDate + " GMT\n";
    return stringDate;
}

std::string responseHandler::extractErrorFile(int status) { // So there is still some sheet here
    std::ifstream errFile;
    std::string path = _config.get_error_page();
//    std::cout << "Error file path: " << path << std::endl;
    path += std::to_string(status) + ".html";
    errFile.open(path);
    if (errFile.fail())
        return "Even extracting the error file failed.";
    std::ostringstream fileContent;
    fileContent << errFile.rdbuf();
    return fileContent.str();
}

std::string responseHandler::directoryListResponse(std::set <std::vector<std::string> > &directories,
                                                   std::set <std::vector<std::string> > &files, std::string directory) {
    // Creating the Html as I would like it
    std::string htmlFile = "<!DOCTYPE html>\n<html>\n<head>\n";
    htmlFile += "\t<title>Index of " + directory + "/</title>\n";
    htmlFile += "</head>\n<body>\n<h2>" + directory + "/</h2>\n<hr/>\n<pre>\n";

    std::cout << RED <<"Building the directory listing from: " << directory << RESET_COLOUR << std::endl;

    /* Don't look at this absolute horror, for god's sake */
//    std::string root = _location.get_root();
    for (std::set < std::vector < std::string > > ::iterator it = directories.begin(); it != directories.end(); it++) {
        std::vector <std::string> iter = *it;
        if (!iter[0].compare("../"))
            htmlFile += "<a href =\"" + iter[0] + "\">" + iter[0] + "</a>\n";
        else {
            int justification = 47 - iter[0].length() + iter[1].length();
            std::string firstPad = " ";
            firstPad.append(justification, ' ');
            std::string padding = " ";
            padding.append(20, ' ');
            htmlFile += "<a href =\"" + directory + "/" + iter[0] + "\">" + iter[0] + "</a>" + firstPad + iter[1] + padding +
                        iter[2] + "\n";
        }
    }
    for (std::set < std::vector < std::string > > ::iterator it = files.begin(); it != files.end();
    it++) {
        std::vector <std::string> iter = *it;
        int justification = 47 - iter[0].length() + iter[1].length();
        std::string firstPad = " ";
        firstPad.append(justification, ' ');
        std::string padding = " ";
        padding.append(20, ' ');
        htmlFile += "<a href =\"" + directory + "/" + iter[0] + "\">" + iter[0] + "</a>" + firstPad + iter[1] + padding +
                    iter[2] + "\n";
    }
    htmlFile += "</pre>\n<hr/>\n";

    //Building the actual header part
    std::string header = "HTTP/1.1 200 OK\nServer: Flamenco \033[31m Flame \033[37m Server\n";
    header += buildDateLine();
    header += "Content-type: text/html; charset=UTF-8\nContent-Length:";
    header += std::to_string(htmlFile.size());
    header += "\n\n";
    header += htmlFile;

    return header;
}

/* < ---------- GENERIC UTILS ---------- > */

bool responseHandler::isDirectory(std::string const& path) {
    struct stat s;
    if (lstat(path.c_str(), &s) == 0) {
        if (S_ISDIR(s.st_mode))
            return true;
        else
            return false;
    }
    return false;
}

std::string responseHandler::rootResolution(std::string const& uri) {

    std::string requestedPath;
    if (!_location.get_root().empty()) {
        if (!uri.compare(_location.get_location_path()))
            requestedPath = _location.get_root();
        else {
            if (std::count(uri.begin(), uri.end(), '/') > 1)
                requestedPath = _location.get_root() + uri.substr(_location.get_location_path().size());
            else
                requestedPath = _location.get_root() + uri;
        }
    }
    else {
        requestedPath = uri;
    }
    return requestedPath;
}

bool responseHandler::isCgiResponse() const {
    return _isCGI;
}

int responseHandler::getCgiFd() const {
    return _cgiFd;
}
