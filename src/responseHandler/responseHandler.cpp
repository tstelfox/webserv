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
#include "colours.hpp"
#include <iostream>
#include <map>
#include <fstream>
#include <sstream>
#include <sys/stat.h> // For checking if directory
#include <dirent.h> // For getting directory contents
#include <time.h> // time for time


responseHandler::responseHandler(std::string requestLine, WSERV::serverConfig const &configs,
                                 std::map <std::string, std::string> &fields)
        : _requestLine(requestLine), _config(configs), _requestFields(fields) {
//    std::cout << ITALIC << COLOR_NEON << "Brr I find out what to respond now" << FORMAT_RESET << RESET_COLOUR << std::endl;
}

responseHandler::responseHandler() {}

responseHandler::~responseHandler() {}

std::string responseHandler::parseAndRespond(int status, int method, std::string uri) {
    if (status != 200)
        return respondError(status);

    int locationStatus = matchLocation(uri);
    if (locationStatus)
        return respondError(locationStatus);

    /* TODO Check that method is allowed in this location - get input in a map or something */
    /* if (_method is not in location list of methods)
        set method to 0 and triggers a 405 */


    /* Parsing method */
    std::cout << "Request Line is: " << _requestLine << std::endl;
    switch (method) {
        case 0:
            return respondError(405);
        case 1:
            return getResponse(uri);
        case 2:
            std::cout << "POST request" << std::endl;
            return postResponse(uri);
//            break;
        case 3:
            std::cout << "DELETE request" << std::endl;
//            return deleteResponse(uri);
    }
    return "Placeholder"; // TODO ths thing
}

int responseHandler::matchLocation(std::string uri) {
    std::vector <WSERV::Location> locationsVec = _config.get_Location_vec();
    WSERV::Location location;

    bool aMatch = false;
    for (std::vector<WSERV::Location>::iterator locIter = locationsVec.begin();
         locIter != locationsVec.end(); locIter++) {
        /* Exact match */
        if (!uri.compare(locIter->get_location_path())) {
            std::cout << "Exact Location match" << std::endl;
            location = *locIter;
            aMatch = true;
            break;
        }
        /* location is incorporated into uri */
//        std::cout << RED << "Location path is: " << locIter->get_location_path() << " and uri is: " << uri << RESET_COLOUR << std::endl;
        if (uri.find(locIter->get_location_path()) != std::string::npos) {
//            std::cout << "Location is a part of the uri: " << locIter->get_location_path() << std::endl;
            location = *locIter;
            aMatch = true;
//            if ((locIter + 1) == locationsVec.end())
//                break;
        }
    }
    if (!aMatch)
        return 404;
    _location = location;
    std::cout << "The correct location is: " << _location.get_location_path() << std::endl;

    return 0;
}

std::string responseHandler::getResponse(std::string uri) {

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
     * Requested path is root + location + uri
     * */
    /* TODO if it is a redirection, does it matter if we're asking for a directory or a file or what? */

    std::string tempRed = "/takemehome";
    if (!uri.compare(tempRed)) {
        std::cout << "Redirection stuff for " << uri << std::endl;
        return redirectionResponse();
    }

    std::cout << "location root is: " << _location.get_root() << " and, if present, index is: " << _location.get_index()
              << std::endl;

    std::string finalUri = uri.substr(_location.get_location_path().length());
    std::string requestedPath;
    if (uri.find(_location.get_root()) != std::string::npos)
        requestedPath = finalUri;
    else {
        // TODO The adding of the slash may be hacky and not always work
        requestedPath = _location.get_root() + "/" + finalUri;
    }
    std::cout << CYAN << "Correct full requested path is: " << requestedPath << " and the finalUri: " << finalUri << RESET_COLOUR << std::endl;


    /* Check for index -
        if there is no index
            if root is directory:
                directory listing
            else
                return 404 (It's what nginx does) [Update: not really]
        else
            look for root plus index
            */
    // CHeck if index absent
    if (_location.get_index().empty()) {
        // if directory
        if (isDirectory(requestedPath)) {
            // Directory Listing
            if (!_location.get_autoindex()) {
                return respondError(403);
            } else {
                return buildDirectoryListing(requestedPath);
            }
        }
//        else { // return 404
//            std::cout << "Not in here right?" << std::endl;
//            return respondError(404);
//        }
    }
    std::string requestedFile;
    if (isDirectory(requestedPath))
        requestedFile = requestedPath + _location.get_index();
    else
        requestedFile = requestedPath;
    std::cout << "Requested file is ultimately: " << requestedFile << std::endl;
    std::ifstream myFile;
    myFile.open(requestedFile);
    if (myFile.fail()) {
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

    /*Check that the method in question is an allowed method*/

    /* Max file Size will only be important for POST I believe */

    /* Default file when a directory is requested is just the index - Include in parsing */

    return responseHeader;
}

std::string responseHandler::postResponse(std::string uri) {
//    std::cout << "Poche seghe: " <<
    (void) uri;
    return "placeholder";
}

std::string responseHandler::respondError(int status) {

    std::cout << MAGENTA << "We be responding to an error" << RESET_COLOUR << std::endl;
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
    return response;
}

std::string responseHandler::extractErrorFile(int status) { // So there is still some sheet here
    std::ifstream errFile;
    std::string path = _config.get_error_page();
//    std::cout << "Error file path: " << path << std::endl;
    path += std::to_string(status) + ".html";
//    std::cout << "Error File path: " << path << std::endl;
    errFile.open(path);
    if (errFile.fail()) {
        // panic hard
    }
    std::ostringstream fileContent;
    fileContent << errFile.rdbuf();
    return fileContent.str();
}

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

std::string responseHandler::redirectionResponse() {
    std::string placeHolder = "put_test/index.html";
    std::string redirectResponse = "HTTP/1.1 301 Moved Permanently\nLocation: ";

//    redirectResponse += _config.get_host() + ":";
//    redirectResponse += std::to_string(_config.get_port()[0]) + placeHolder + "\n\n";

    redirectResponse += placeHolder + "\n\n";

    std::cout << redirectResponse << std::endl;
    return redirectResponse;
}

/* < ---------- Response header building utils ---------- > */

std::string responseHandler::buildHttpLine(int status) {
    std::map<int, std::string> statusCodes;
    statusCodes[200] = "OK";
    statusCodes[400] = "Bad Request";
    statusCodes[403] = "Forbidden";
    statusCodes[404] = "Not Found";
    statusCodes[405] = "Method Not Allowed";
    statusCodes[505] = "HTTP Version Not Supported";

    std::string response = "HTTP/1.1 ";
    response += std::to_string(status) + " "; // C++11 oops fix this later
    response += statusCodes[status] + "\n";
    response += "Server: Flamenco \033[31m Flame \033[37m Server\n";
    return response;
}

std::string responseHandler::buildDateLine() {

    time_t now = time(0);
    char *date = ctime(&now); // TODO check if it leaks
    std::string stringDate = date;
    stringDate.insert(3, ",");
    stringDate.resize(stringDate.size() - 1);
    stringDate = "Date: " + stringDate + " GMT\n";
    return stringDate;
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
            htmlFile += "<a href =\"" + directory + iter[0] + "\">" + iter[0] + "</a>" + firstPad + iter[1] + padding +
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
        htmlFile += "<a href =\"" + directory + iter[0] + "\">" + iter[0] + "</a>" + firstPad + iter[1] + padding +
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

bool responseHandler::isDirectory(std::string path) {
    struct stat s;
    if (lstat(path.c_str(), &s) == 0) {
        if (S_ISDIR(s.st_mode))
            return true;
        else
            return false;
    }
    return false;
}
