/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   locationConf.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/21 12:00:47 by laoubaid          #+#    #+#             */
/*   Updated: 2025/08/29 03:53:27 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "locationConf.hpp"
#include "serverConf.hpp"

locationConf::locationConf(std::string& str, serverConf& cfg) {
    if (str.empty())
        throw std::runtime_error("unvalid location argumnet!");
    path = resolve_path(str);
    methods = GET_BIT | POST_BIT | DELETE_BIT;
    autoindex = cfg.is_autoindex();
    redirect = cfg.get_redirect();
    root = cfg.get_root();
    is_upset = false;
    is_indexed = false;
    is_methods = false;
    has_redirect_ = false;
    if (redirect.first)
        has_redirect_ = true;
    if (cfg.is_index()) {
        index = cfg.get_index();
        is_indexed = true;
    }
}

locationConf::~locationConf() {}

void locationConf::set_methods(std::vector<std::string>& values) {
    if (!is_methods)
        methods = 0;
    is_methods = true;
    if (values.empty())
        throw std::runtime_error("Missing methods arguments");
    for (size_t i = 0; i < values.size(); ++i) {
        if (values[i] == "GET") {
            methods |= GET_BIT;
        } else if (values[i] == "POST") {
            methods |= POST_BIT;
        } else if (values[i] == "DELETE") {
            methods |= DELETE_BIT;
        } else {
            throw std::runtime_error("forbidden methode " + values[i]);
        }
    }
}

void locationConf::set_redirect(std::vector<std::string>& values) {
    if (values.size() != 2)
        throw std::runtime_error("invalid return paramter!");

    int code = std::atoi(values[0].c_str());
    if (code < 301 || code > 308 || (code > 303 && code < 307))
        throw std::runtime_error("unknown redirection code!");
    // valid redirections 301 302 303 307 308
    redirect.first = code;
    redirect.second = values[1];
    has_redirect_ = true;
}

void locationConf::set_root(std::vector<std::string>& values) {
    if (values.size() != 1)
        throw std::runtime_error("invalid root paramter!");
    
    root = values[0];
}

void locationConf::set_index(std::vector<std::string>& values) {
    if (values.size() != 1)
        throw std::runtime_error("invalid index paramter!");
    is_indexed = true;
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
    is_upset = true;
    up_store = values[0];
}