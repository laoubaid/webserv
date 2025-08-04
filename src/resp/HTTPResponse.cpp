/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/03 19:19:18 by laoubaid          #+#    #+#             */
/*   Updated: 2025/08/04 02:06:19 by laoubaid         ###   ########.fr       */
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

std::string process_path(std::string& path) {
    std::string resp_;

    if (path.size() == 1 && path == "/")
        path += "index.html";
    path = "./www" + path;
    std::cout << ">>>>>>>>>> final target path : " << path << std::endl;
    if (!access(path.c_str(), R_OK)) {
        std::cout << "access permissions geranted!" << std::endl;
        std::string body = read_file(path);
        resp_ = "HTTP/1.1 200 OK\r\n"
                "Content-Type: text/html\r\n"
                "Content-Length:  " + std::to_string(body.size()) + "\r\n"
                "Connection: keep-alive\r\n"
                "\r\n" + body;
    } else {
        std::cout << "access: an error occured!" << std::endl;
        resp_ = "HTTP/1.1 403 FORBIDDEN\r\n"
                "Content-Type: text/html\r\n"
                "Content-Length:  0\r\n"
                "Connection: close\r\n"
                "\r\n";
    }
    // note we are starting to read a file using the path from url needs alot more (use EPOLL for read btw!!)
    return resp_;
}

std::string responesForGet(HTTPRequestParser &request) {
    std::string response_;

    // im lost
    // std::cout << "request line : " << request.getMethod() << " " << request.getTarget() << std::endl; 
    std::string web_root = "./www/";

    // this should be implemanted in the httpreq obj creation
    std::string path = resolve_path(request.getTarget());

    response_ = process_path(path);
        
    return response_;
}


std::string HttpResponse::generateResponse(HTTPRequestParser &request) {
    std::string response_;

    std::cout << GRN_CLR << "Generating response ..." << DEF_CLR << std::endl;
    if (request.getParsingCode() == 400) {
        response_ = "HTTP/1.1 400 Bad Request\r\n\r\n";
    } else if (request.getParsingCode() == 200) {
        if (request.getMethod() == GET) {
            response_ = responesForGet(request);
        }
        else { // this is for POST and DELETE methods, temporarily of course hhhh
            std::cout << GRN_CLR << "200 OK" << DEF_CLR << std::endl;
            std::string html = "<!DOCTYPE html><html><body><h1>Hello from the WebServer!</h1></body></html>";
            response_ = "HTTP/1.1 200 OK\r\n"
                        "Content-Type: text/html\r\n"
                        "Content-Length: " + std::to_string(html.size()) + "\r\n"
                        "Connection: keep-alive\r\n"
                        "\r\n" + html;
        }
    } else if (request.getParsingCode() == 413) {
        std::string html = "<!DOCTYPE html><html><body><h1>413 Payload Too Large</h1></body></html>";
        response_ = "HTTP/1.1 413 Payload Too Large\r\n"
                    "Content-Type: text/html\r\n"
                    "Content-Length: " + std::to_string(html.size()) + "\r\n"
                    "Connection: close\r\n"
                    "\r\n" + html;
    } else {
        std::cout << RED_CLR << request.getParsingCode() <<  " Internal Server Error" << DEF_CLR << std::endl;
        std::string html = "<!DOCTYPE html><html><body><h1>500 Internal Server Error</h1></body></html>";
        response_ = "HTTP/1.1 500 Internal Server Error\r\n"
                    "Content-Type: text/html\r\n"
                    "Content-Length: " + std::to_string(html.size()) + "\r\n"
                    "Connection: close\r\n"
                    "\r\n" + html;
    }
    return response_;
}
