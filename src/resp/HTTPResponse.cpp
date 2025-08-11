/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/03 19:19:18 by laoubaid          #+#    #+#             */
/*   Updated: 2025/08/11 10:40:07 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTPResponse.hpp"

const std::string& HttpResponse::getMimeType(const std::string& path) {
    static const std::map<std::string, std::string> mime_types = {
        {".html", "text/html"},
        {".htm",  "text/html"},
        {".css",  "text/css"},
        {".js",   "application/javascript"},
        {".json", "application/json"},
        {".png",  "image/png"},
        {".jpg",  "image/jpeg"},
        {".jpeg", "image/jpeg"},
        {".gif",  "image/gif"},
        {".ico",  "image/x-icon"},
        {".txt",  "text/plain"},
        {".pdf",  "application/pdf"},
        {".svg",  "image/svg+xml"}
    };

    static const std::string default_type = "application/octet-stream";

    
    size_t idx = path.rfind("/");
    std::string tmp;
    if (idx != std::string::npos) {
        tmp = path.substr(idx + 1);
        idx = tmp.rfind('.');
        if (idx != std::string::npos) {
            tmp = tmp.substr(idx);
        }
    }
    
    std::map<std::string, std::string>::const_iterator it = mime_types.find(tmp);
    if (it != mime_types.end())
        return it->second;
    return default_type;
}

std::string HttpResponse::generateResponse(HTTPRequestParser &request) {
    std::string response_;

    std::cout << GRN_CLR << "Generating response ..." << DEF_CLR << std::endl;
    if (request.getParsingCode() == 400) {
        response_ = BADR_400_;
    } else if (request.getParsingCode() == 200) {
        if (request.getMethod() == GET) {
            response_ = HttpResponse::responesForGet(request);
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
        response_ = ELRG_413_;
    } else {
        std::cout << RED_CLR << request.getParsingCode() <<  " Internal Server Error" << DEF_CLR << std::endl;
        response_ = IERR_500_;
    }
    return response_;
}

t_resp_state& HttpResponse::getRespState() {
    return resp_stat_;
}

