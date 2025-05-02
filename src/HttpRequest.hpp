/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 10:52:04 by laoubaid          #+#    #+#             */
/*   Updated: 2025/04/24 10:52:57 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "include.hpp"

#ifndef _HTTP_REQUEST_HPP_
#define _HTTP_REQUEST_HPP_

class HttpRequest {
    public:
        std::string method;
        std::string path;
        std::string version;
        std::map<std::string, std::string> headers;


        HttpRequest(/* args */);
        ~HttpRequest();


        static HttpRequest parse(const char* raw) {
            HttpRequest req;
            std::istringstream stream(raw);
            std::string line;

            // Parse request line
            std::getline(stream, line);
            std::istringstream request_line(line);
            request_line >> req.method >> req.path >> req.version;

            // Parse headers
            while (std::getline(stream, line) && line != "\r") {
                size_t colon = line.find(':');
                if (colon != std::string::npos) {
                    std::string key = line.substr(0, colon);
                    std::string value = line.substr(colon + 1);
                    size_t start = value.find_first_not_of(" \t");
                    size_t end = value.find_last_not_of("\r\n");
                    req.headers[key] = value.substr(start, end - start + 1);
                }
            }

            return req;
        }
};

#endif