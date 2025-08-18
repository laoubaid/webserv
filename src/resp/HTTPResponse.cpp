/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/03 19:19:18 by laoubaid          #+#    #+#             */
/*   Updated: 2025/08/18 04:26:56 by laoubaid         ###   ########.fr       */
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

const std::string HttpResponse::generateResponse() {

    resp_buff_.clear();
    std::cout << GRN_CLR << "Generating response ..." << DEF_CLR << std::endl;
    if (request_->getParsingCode() == 400) {
        resp_buff_ = BADR_400_;
        resp_stat_ = DONE;
    } else if (request_->getParsingCode() == 200) {
        if (request_->getMethod() == GET) {
            responesForGet();
        } else {  // this is for POST and DELETE methods, temporarily of course hhhh
            std::cout << GRN_CLR << "200 OK" << DEF_CLR << std::endl;
            std::string html = "<!DOCTYPE html><html><body><h1>Hello from the WebServer!</h1></body></html>\n";
            resp_buff_ = "HTTP/1.1 200 OK\r\n"
                        "Content-Type: text/html\r\n"
                        "Content-Length: " + std::to_string(html.size()) + "\r\n"
                        "Connection: keep-alive\r\n"
                        "\r\n" + html;
        }
    } else if (request_->getParsingCode() == 413) {
        resp_buff_ = ELRG_413_;
        resp_stat_ = DONE;
    } else {
        std::cout << RED_CLR << request_->getParsingCode() <<  " Internal Server Error!" << DEF_CLR << std::endl;
        resp_buff_ = IERR_500_;
        resp_stat_ = DONE;
    }
    return resp_buff_;
}

t_resp_state& HttpResponse::getRespState() {
    return resp_stat_;
}

std::string&   HttpResponse::getRespBuff() {
    return resp_buff_;
}

