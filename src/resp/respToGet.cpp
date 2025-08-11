/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   respToGet.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 15:23:45 by laoubaid          #+#    #+#             */
/*   Updated: 2025/08/11 10:49:09 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTPResponse.hpp"

std::string url_decode(const std::string& str) {
    std::string result;
    char hex[3] = {0};
    for (size_t i = 0; i < str.length(); ++i) {
        if (str[i] == '%' && i + 2 < str.length()) {
            hex[0] = str[i + 1];
            hex[1] = str[i + 2];
            result += static_cast<char>(std::strtol(hex, nullptr, 16));
            i += 2;
        } else if (str[i] == '+') {
            result += ' ';
        } else {
            result += str[i];
        }
    }
    return result;
}


std::string resolve_path(const std::string& str) {
    std::stringstream ss(str);
    std::string part;
    std::vector<std::string> stack;

    while (std::getline(ss, part, '/')) {
        if (part.empty() || part == ".")
            continue;
        if (part == "..") {
            if (!stack.empty())
                stack.pop_back();
        } else {
            stack.push_back(part);
        }
    }

    std::string result;
    for (size_t i = 0; i < stack.size(); ++i) {
        result += "/" + stack[i];
    }

    // return result.empty() ? ("./www/") : ("./www" + result);
    return result.empty() ? "/" : result;
}

bool read_file(const std::string& path, std::string& content) {
    std::ifstream file(path.c_str(), std::ios::binary);
    if (!file)
        return false;
    std::ostringstream ss;
    ss << file.rdbuf();
    content = ss.str();
    return true;
}

std::string serveStaticContent(const std::string& path) {
    std::string body;
    
    if (read_file(path, body) == false) {
        return FORB_403_;
    }
    std::string resp_;
    resp_ = OK_200_;
    resp_ += "Content-Type: " + HttpResponse::getMimeType(path) + "\r\n"
        "Content-Length: " + std::to_string(body.size()) + "\r\n"
        "\r\n" + body;

    return resp_;
}

bool is_directory(const std::string& path) {
    struct stat s;
    if (stat(path.c_str(), &s) == 0)
        return S_ISDIR(s.st_mode);
    return false;
}

std::string list_directory(const std::string& path) {
    std::string resp_;
    std::string direname;
    DIR* dir = opendir(path.c_str());

    if (dir == NULL) {
        return FORB_403_;
    }
    size_t idx = path.rfind("/");
    if (idx != std::string::npos)
        direname = path.substr(idx + 1);
    // NOTE the path prints as ./www/dir in the web page shouldnt it be just dir?
    std::string body = "<!DOCTYPE html><html><head><title>Index of " + direname + \
    "</title></head><body><h1>Index of " + direname + "</h1><ul>";

    struct dirent *ptr;
    ptr = readdir(dir);
    while (ptr) {
        std::string tmp = std::string(ptr->d_name);
        if (tmp != "." && tmp != "..")
            body += "<li><a href=\"" + direname + "/" + tmp + "\">" + tmp + "</a></li>";
        ptr = readdir(dir);
    }
    body += "</ul></body></html>";
    resp_ = OK_200_;
    resp_ += "Content-Type: text/html\r\n"
        "Content-Length: " + std::to_string(body.size()) + "\r\n\r\n" + body;

    closedir(dir);
    return resp_;
}

std::string process_path(std::string& path) {
    std::string resp_;

    if (path.size() == 1 && path == "/")
        path += "index.html";
    path = "./www" + path;
    std::cout << ">>>>>>>>>> final target path : " << path << std::endl;

    if (!access(path.c_str(), F_OK)) {
        if (!access(path.c_str(), R_OK)) {
            if (is_directory(path)) {
                resp_ = list_directory(path);
            } else {
                resp_ = serveStaticContent(path);
            }
        } else
            resp_ = FORB_403_;
    } else
        resp_ = NOTF_404_;
    // note we are starting to read a file using the path from url needs alot more (use EPOLL for read btw!!)
    return resp_;
}

std::string HttpResponse::responesForGet(HTTPRequestParser &request) {
    std::string response_;

    // im lost
    // std::cout << "request line : " << request.getMethod() << " " << request.getTarget() << std::endl; 
    std::string web_root = "./www/";       // get this from config

    // this should be implemanted in the httpreq obj creation

    std::string path = url_decode(request.getTarget());
    path = resolve_path(path);

    response_ = process_path(path);
        
    return response_;
}

