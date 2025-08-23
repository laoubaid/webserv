/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serverConf.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/21 11:03:10 by laoubaid          #+#    #+#             */
/*   Updated: 2025/08/22 18:19:49 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "serverConf.hpp"

serverConf::serverConf(/* args */) : clt_body_max_size(-1) {
    clt_body_max_size = 1024 * 1024;
    autoindex = false;
    root = "./www";
    is_indexed = false;
}

serverConf::serverConf(const serverConf& obj) {
    clt_body_max_size = obj.clt_body_max_size;
    root = obj.root;
    index = obj.index;
    err_pages = obj.err_pages;
    listens = obj.listens;
    locations = obj.locations;
}

serverConf::~serverConf() {}

void serverConf::add_listen(std::vector<std::string>& values) {
    if (values.size() != 1)
        throw std::runtime_error("invalid listen paramter number!");

    std::string::size_type pos = values[0].find(':');
    if (pos == std::string::npos) {
        throw std::runtime_error("invalid listen paramter " + values[0]);
    }

    std::string ip = values[0].substr(0, pos);
    std::string port_str = values[0].substr(pos + 1);
    int port = std::atoi(port_str.c_str());

    struct sockaddr_in _address;
    memset(&_address, 0, sizeof(_address));    // is this allowed as well???
    _address.sin_family = AF_INET;
    _address.sin_port = htons(port);  // convert to network byte order

    if (inet_pton(AF_INET, ip.c_str(), &_address.sin_addr) <= 0) {   // not alowed function????
        throw std::runtime_error("invalid listen IP paramter!" + ip);
    }
    listens[port] = _address;
}

void serverConf::add_err_page(std::vector<std::string>& values) {
    if (values.size() != 2)
        throw std::runtime_error("invalid error_page paramter!");

    int code = std::atoi(values[0].c_str());
    if (code < 400 || code > 511 || (code > 430 && code < 500))
        throw std::runtime_error("unknown error_page code!");
    // client errors are from 400 to 430
    // server errors are from 500 to 511
    err_pages[code] = values[1];
}

void serverConf::set_clt_max_body_size(std::vector<std::string>& values) {
    if (values.size() != 1)
        throw std::runtime_error("invalid client_max_body_size parameter!");

    std::string& raw = values[0];
    if (raw.empty())
        throw std::runtime_error("invalid client_max_body_size parameter!");

    char last_char = raw[raw.size() - 1];
    int multiplier = 1;
    std::string number_part = raw;

    if (last_char < '0' || last_char > '9') {
        number_part = raw.substr(0, raw.size() - 1);
        switch (last_char) {
            case 'K': case 'k':
                multiplier = 1024;
                break;
            case 'M': case 'm':
                multiplier = 1024 * 1024;
                break;
            case 'G': case 'g':
                multiplier = 1024 * 1024 * 1024;
                break;
            default:
                throw std::runtime_error(std::string("client_max_body_size unknown size suffix: ") + last_char);
        }
    }
    for (size_t i = 0; i < number_part.size(); ++i) {
        if (!std::isdigit(number_part[i]))
            throw std::runtime_error("invalid number in client_max_body_size: " + number_part);
    }
    int size = std::atoi(number_part.c_str());
    if (size <= 0)
        throw std::runtime_error("invalid size value!");

    size *= multiplier;
    clt_body_max_size = static_cast<size_t>(size);
}

void serverConf::set_root(std::vector<std::string>& values) {
    if (values.size() != 1)
        throw std::runtime_error("invalid root paramter!");
    
    root = values[0];
}

void serverConf::set_index(std::vector<std::string>& values) {
    if (values.size() != 1)
        throw std::runtime_error("invalid index paramter!");
    is_indexed = true;
    index = values[0];
}

void serverConf::add_location(locationConf& lct) {
    locations[lct.get_path()] = lct;
}

void serverConf::add_redir(std::vector<std::string>& values) {
    if (values.size() != 2)
        throw std::runtime_error("invalid return paramter!");

    int code = std::atoi(values[0].c_str());
    if (code < 300 || code > 308)
        throw std::runtime_error("unknown redirection code!");
    // redirections are from 300 to 308
    redirs[code] = values[1];
}

void serverConf::set_auto_index(std::vector<std::string>& values) {
    if (values.size() != 1)
        throw std::runtime_error("invalid autoindex paramter!");
    
    if (values[0] == "on")
        autoindex = true;
    else if (values[0] == "off")
        autoindex = false;
    else
        throw  std::runtime_error("invalid autoindex paramter!");
}

void serverConf::set_default() {
    if (listens.size() == 0) {
        struct sockaddr_in _address;
        memset(&_address, 0, sizeof(_address));    // is this allowed as well???
        _address.sin_family = AF_INET;
        _address.sin_port = htons(8080);  // convert to network byte order
        _address.sin_addr.s_addr = htonl(INADDR_ANY);
        listens[8080] = _address;
    }
    if (locations.empty()) {
        std::string tmp("/");
        locationConf lct(tmp, *this);
        add_location(lct);
    }
}
