/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   locationConf.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/21 12:00:47 by laoubaid          #+#    #+#             */
/*   Updated: 2025/08/21 13:56:37 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "locationConf.hpp"

locationConf::locationConf(std::string& str) : path(str) , methods(0), autoindex(false) {}

locationConf::~locationConf() {}

void locationConf::set_methods(std::vector<std::string>& values) {
    for (size_t i = 0; i < values.size(); ++i) {
        if (values[i] == "GET") {
            methods |= 4;
        } else if (values[i] == "POST") {
            methods |= 2;
        } else if (values[i] == "DELETE") {
            methods |= 1;
        } else {
            throw std::runtime_error("forbidden methode " + values[i]);
        }
    }
}

void locationConf::add_redir(std::vector<std::string>& values) {
    if (values.size() != 2)
        throw std::runtime_error("invalid return paramter!");

    int code = std::atoi(values[0].c_str());
    if (code < 300 || code > 308)
        throw std::runtime_error("unknown redirection code!");
    // redirections are from 300 to 308
    redirs[code] = values[1];
}

void locationConf::set_root(std::vector<std::string>& values) {
    if (values.size() != 1)
        throw std::runtime_error("invalid root paramter!");
    
    root = values[0];
}

void locationConf::set_index(std::vector<std::string>& values) {
    if (values.size() != 1)
        throw std::runtime_error("invalid index paramter!");
    
    index = values[0];
}
void locationConf::set_auto_index(std::vector<std::string>& values) {
    if (values.size() != 1)
        throw std::runtime_error("invalid autoindex paramter!");
    
    if (values[0] == "on")
        autoindex = true;
    else if (values[0] == "off")
        autoindex = false;
    else
        throw  std::runtime_error("invalid autoindex paramter!");
}

void locationConf::set_up_store(std::vector<std::string>& values) {
    if (values.size() != 1)
        throw std::runtime_error("invalid upload_store paramter!");
    
    up_store = values[0];
}