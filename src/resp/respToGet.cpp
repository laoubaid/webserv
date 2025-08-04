/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   respToGet.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 15:23:45 by laoubaid          #+#    #+#             */
/*   Updated: 2025/08/04 16:38:17 by laoubaid         ###   ########.fr       */
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
            result += ' '; // '+' becoms a space just like %20
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

std::string read_file(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file)
        throw std::runtime_error("Cannot open file");

    std::string content;
    while (!file.eof()) {
        char c;
        file.get(c);
        content += c;
    }

    return content;
}

std::string serveStaticContent(const std::string& path) {
    std::string body = read_file(path);
    std::string resp_ = "HTTP/1.1 200 OK\r\n"
                    "Content-Type: " + HttpResponse::getMimeType(path) + "\r\n"
                    "Content-Length:  " + std::to_string(body.size()) + "\r\n"
                    "Connection: keep-alive\r\n"
                    "\r\n" + body;
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
            resp_ = serveStaticContent(path);
        } else {
            resp_ = "HTTP/1.1 403 FORBIDDEN\r\n"
                    "Content-Type: text/html\r\n"
                    "Content-Length:  64\r\n"
                    "Connection: close\r\n"
                    "\r\n"
                    "<html><body><center><h1>403 Forbidden</h1><center></body></html>";
        }
        // this sh!t clearly needs a clean up, you cant just leave those gainet blocks of text in there?
    } else {
        resp_ = "HTTP/1.1 404 Not Found\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: 64\r\n"
            "Connection: close\r\n"
            "\r\n"
            "<html><body><center><h1>404 Not Found</h1><center></body></html>";
    }
    // note we are starting to read a file using the path from url needs alot more (use EPOLL for read btw!!)
    return resp_;
}

std::string HttpResponse::responesForGet(HTTPRequestParser &request) {
    std::string response_;

    // im lost
    // std::cout << "request line : " << request.getMethod() << " " << request.getTarget() << std::endl; 
    std::string web_root = "./www/";       // get this from config

    // this should be implemanted in the httpreq obj creation

    // Directories NEXT !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! <<<----

    std::string path = url_decode(request.getTarget());
    path = resolve_path(path);

    response_ = process_path(path);
        
    return response_;
}

