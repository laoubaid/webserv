/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   respToGet.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 15:23:45 by laoubaid          #+#    #+#             */
/*   Updated: 2025/08/16 01:25:21 by laoubaid         ###   ########.fr       */
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

size_t get_file_size(std::fstream &file) {
    file.seekg(0, std::ios::end);
    size_t end_pos = static_cast<size_t>(file.tellg());
    file.seekg(0, std::ios::beg);  // reset to beginning
    return end_pos;
}

bool HttpResponse::serveStaticContent(const std::string& path) {
    file_.open(path, std::ios::in | std::ios::binary);
    if (!file_.is_open()) {
        resp_buff_ = FORB_403_;
        resp_stat_ = DONE;
        return false;
    }
    resp_buff_ = OK_200_;
    resp_buff_ += "Content-Type: " + HttpResponse::getMimeType(path) + "\r\n" "Content-Length: " + std::to_string(get_file_size(file_)) + "\r\n\r\n";
    resp_stat_ = LOAD;
    return true;
}

bool is_directory(const std::string& path) {
    struct stat s;
    if (stat(path.c_str(), &s) == 0)
        return S_ISDIR(s.st_mode);
    return false;
}

bool HttpResponse::list_directory(const std::string& path) {
    std::string direname;
    DIR* dir = opendir(path.c_str());

    resp_buff_.clear();
    if (dir == NULL) {
        resp_buff_ = FORB_403_;
        return false;
    }
    size_t idx = path.rfind("/");
    if (idx != std::string::npos)
        direname = path.substr(idx + 1);

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
    resp_buff_ = OK_200_;
    resp_buff_ += "Content-Type: text/html\r\n"
        "Content-Length: " + std::to_string(body.size()) + "\r\n\r\n" + body;

    closedir(dir);
    return true;
}

void HttpResponse::process_path(std::string& path) {

    if (path.size() == 1 && path == "/")
        path += "index.html";
    path = "./www" + path;
    std::cout << ">>>>>>>>>> final target path : " << path << std::endl;

    if (!access(path.c_str(), F_OK)) {
        if (!access(path.c_str(), R_OK)) {
            if (is_directory(path)) {
                list_directory(path);
                resp_stat_ = DONE;
            } else {
                serveStaticContent(path);
            }
        } else
            resp_buff_ = FORB_403_;
    } else
        resp_buff_ = NOTF_404_;
    // note we are starting to read a file using the path from url needs alot more (use EPOLL for read btw!!)
}

bool HttpResponse::read_file_continu() {
    // Open the file only once in the class member 
    if (!file_.is_open() || resp_stat_ == DONE) {
        return false;
    }

    char buffer[100000];
    file_.read(buffer, sizeof(buffer));
    resp_buff_ = std::string(buffer, file_.gcount());
    if (file_.eof()) {
        resp_stat_ = DONE;
        file_.close();
    }
    return true;
}

void HttpResponse::responesForGet() {
    if (resp_stat_ == STRT) {
        // std::cout << "request line : " << request.getMethod() << " " << request.getTarget() << std::endl; 
        std::string web_root = "./www/";       // get this from config
    
        // this should be implemanted in the httpreq obj creation
    
        std::string path = url_decode(request_->getTarget());
        path = resolve_path(path);
    
        process_path(path);
    } else if (resp_stat_ == LOAD) {
        // continu handling response here
        read_file_continu();
    }
}

