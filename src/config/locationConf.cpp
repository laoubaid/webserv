/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   locationConf.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/21 12:00:47 by laoubaid          #+#    #+#             */
/*   Updated: 2025/09/02 05:17:21 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "locationConf.hpp"
#include "serverConf.hpp"

locationConf::locationConf(std::string& str, serverConf& cfg) {
    if (str.empty())
        throw std::runtime_error("unvalid location argumnet!");
    path_ = resolve_path(str);
    methods_ = GET_BIT | POST_BIT | DELETE_BIT;
    autoindex_ = cfg.is_autoindex();
    redirect_ = cfg.get_redirect();
    root_ = cfg.get_root();
    is_upset_ = false;
    is_indexed_ = false;
    has_methods_ = false;
    has_redirect_ = false;
    has_cgi_ = false;
    if (redirect_.first)
        has_redirect_ = true;
    if (cfg.is_index()) {
        index_ = cfg.get_index();
        is_indexed_ = true;
    }
}

locationConf::~locationConf() {}

void locationConf::set_methods(std::vector<std::string>& values) {
    if (!has_methods_)
        methods_ = 0;
    has_methods_ = true;
    if (values.empty())
        throw std::runtime_error("Missing methods arguments");
    for (size_t i = 0; i < values.size(); ++i) {
        if (values[i] == "GET") {
            methods_ |= GET_BIT;
        } else if (values[i] == "POST") {
            methods_ |= POST_BIT;
        } else if (values[i] == "DELETE") {
            methods_ |= DELETE_BIT;
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
    redirect_.first = code;
    redirect_.second = values[1];
    has_redirect_ = true;
}

void locationConf::set_root(std::vector<std::string>& values) {
    if (values.size() != 1)
        throw std::runtime_error("invalid root paramter!");
    
    root_ = values[0];
}

void locationConf::set_index(std::vector<std::string>& values) {
    if (values.size() != 1)
        throw std::runtime_error("invalid index paramter!");
    is_indexed_ = true;
    index_ = values[0];
}

void locationConf::set_auto_index(std::vector<std::string>& values) {
    if (values.size() != 1)
        throw std::runtime_error("invalid autoindex paramter!");
    
    if (values[0] == "on")
        autoindex_ = true;
    else if (values[0] == "off")
        autoindex_ = false;
    else
        throw  std::runtime_error("invalid autoindex paramter!");
}

void locationConf::set_up_store(std::vector<std::string>& values) {
    if (values.size() != 1)
        throw std::runtime_error("invalid upload_store paramter!");
    is_upset_ = true;
    up_store_ = values[0];
}

void locationConf::set_cgi(std::vector<std::string>& values) {
    if (values.size() != 2)
        throw std::runtime_error("invalid CGI paramter!");
    has_cgi_ = true;
    cgi_ext_ = values[0];
    cgi_path_ = values[1];
}
