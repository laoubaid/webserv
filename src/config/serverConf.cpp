/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serverConf.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/21 11:03:10 by laoubaid          #+#    #+#             */
/*   Updated: 2025/09/25 01:17:25 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "serverConf.hpp"

serverConf::serverConf(/* args */) : clt_body_max_size_(-1) {
	clt_body_max_size_ = 1024 * 1024;
	send_timeout_ = 5;
	recv_timeout_ = 5;
	cgi_timeout_ = 5;
	autoindex_ = false;
	root_ = "./www/";
	is_indexed_ = false;
	redirect_ = std::make_pair(0, "");
}

serverConf::serverConf(const serverConf& obj) {
	clt_body_max_size_ = obj.clt_body_max_size_;
	root_ = obj.root_;
	is_indexed_ = obj.is_indexed_;
	index_ = obj.index_;
	autoindex_ = obj.autoindex_;
	err_pages_ = obj.err_pages_;
	listens_ = obj.listens_;
	locations_ = obj.locations_;
	redirect_ = obj.redirect_;
	send_timeout_ = obj.send_timeout_;
	recv_timeout_ = obj.recv_timeout_;
	cgi_timeout_ = obj.cgi_timeout_;
}

serverConf::~serverConf() {}

uint32_t parse_ipv4(const std::string& ip) {
	std::vector<int>    parts;
	std::stringstream   ss(ip);
	std::string         item;

	if (ip.size() > 15)
		throw std::runtime_error("Invalid IP: " + ip);
	while (std::getline(ss, item, '.')) {
		if (item.empty())
			throw std::runtime_error("Invalid IP: " + ip);
		for (size_t i = 0; i < item.size(); ++i) {
			if (std::isdigit(item[i]) == 0)
				throw std::runtime_error("Invalid IP: " + ip);
		}
		int num = atoi(item.c_str());
		if (num < 0 || num > 255)
			throw std::runtime_error("Invalid IP octet: " + ip);
		parts.push_back(num);
	}

	if (parts.size() != 4)
		throw std::runtime_error("Invalid IPv4 format: " + ip);

	return (parts[0] << 24) | (parts[1] << 16) | (parts[2] << 8) | parts[3];
}

void serverConf::add_listen(std::vector<std::string>& values) {
	if (values.size() != 1)
		throw std::runtime_error("invalid listen paramter number!");

	std::string::size_type pos = values[0].find(':');
	if (pos == std::string::npos) {
		throw std::runtime_error("invalid listen paramter " + values[0]);
	}
	std::string ip = values[0].substr(0, pos);
	std::string port_str = values[0].substr(pos + 1);

	for (size_t i = 0; i < port_str.size(); ++i) {
		if (std::isdigit(port_str[i]) == 0)
			throw std::runtime_error("Invalid IP: " + values[0]);
	}
	int port = std::atoi(port_str.c_str());
	uint32_t ip_num = parse_ipv4(ip);
	struct sockaddr_in _address;
	_address.sin_family = AF_INET;
	_address.sin_addr.s_addr = htonl(ip_num);
	_address.sin_port = htons(port);
	listens_.push_back(_address);
}

void serverConf::add_err_page(std::vector<std::string>& values) {
	if (values.size() != 2)
		throw std::runtime_error("invalid error_page paramter!");

	int code = std::atoi(values[0].c_str());
	if (code < 400 || code > 511 || (code > 430 && code < 500))
		throw std::runtime_error("unknown error_page code!");
	// client errors are from 400 to 430
	// server errors are from 500 to 511
	err_pages_[code] = values[1];
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
	clt_body_max_size_ = static_cast<size_t>(size);
}

void serverConf::set_root(std::vector<std::string>& values) {
	if (values.size() != 1)
		throw std::runtime_error("invalid root paramter!");
	
	root_ = resolve_path(values[0]) + "/";
}

void serverConf::set_index(std::vector<std::string>& values) {
	if (values.size() != 1)
		throw std::runtime_error("invalid index paramter!");
	is_indexed_ = true;
	index_ = resolve_path(values[0]) + "/";   //! might cause problems addin "/" for files !?
}

void serverConf::add_location(locationConf& lct) {
	locations_[lct.get_path()] = lct;
}

void serverConf::set_timeout(std::vector<std::string>& values, int flag) {
	if (values.size() != 1)
		throw std::runtime_error("invalid timeout parameter!");

	std::string& raw = values[0];
	if (raw.empty())
		throw std::runtime_error("invalid timeout parameter!");

	char last_char = raw[raw.size() - 1];
	int multiplier = 1;
	std::string number_part = raw;

	if (last_char < '0' || last_char > '9') {
		number_part = raw.substr(0, raw.size() - 1);
		if (last_char == 'M')
			multiplier = 60;
		else
			throw std::runtime_error(std::string("timeout unknown size suffix: ") + last_char);
	}
	for (size_t i = 0; i < number_part.size(); ++i) {
		if (!std::isdigit(number_part[i]))
			throw std::runtime_error("invalid number in timeout: " + number_part);
	}
	int time = std::atoi(number_part.c_str());
	if (time <= 0)
		throw std::runtime_error("invalid timeout time value!");

	time *= multiplier;
	if (time > 600)
		time = 600;  //* set max timeout to 5 min
	if (flag == 0)
		recv_timeout_ = static_cast<size_t>(time);
	else if (flag == 1)
		cgi_timeout_ = static_cast<size_t>(time);
	else
		send_timeout_ = static_cast<size_t>(time);

}

void serverConf::set_redirect(std::vector<std::string>& values) {
	if (values.size() != 2)
		throw std::runtime_error("invalid return paramter!");

	int code = std::atoi(values[0].c_str());
	if (code < 301 || code > 308 || (code > 303 && code < 307))
		throw std::runtime_error("unknown redirection code!");
	// valid redirections 301 302 303 307 308
	redirect_.first = code;
	redirect_.second = resolve_path(values[1]) + "/";
}

void serverConf::set_auto_index(std::vector<std::string>& values) {
	if (values.size() != 1)
		throw std::runtime_error("invalid autoindex paramter!");
	
	if (values[0] == "on")
		autoindex_ = true;
	else if (values[0] == "off")
		autoindex_ = false;
	else
		throw  std::runtime_error("invalid autoindex paramter!");
}

void serverConf::set_default() {
	if (listens_.size() == 0) {
		struct sockaddr_in _address;
		memset(&_address, 0, sizeof(_address));    // is this allowed as well???
		_address.sin_family = AF_INET;
		_address.sin_port = htons(8080);  // default
		_address.sin_addr.s_addr = htonl(INADDR_ANY);
		listens_.push_back(_address);
	}
	try {
		get_location("/");
	} catch(const std::exception& e) {
		std::string tmp("/");
		locationConf lct(tmp, *this);
		add_location(lct);
	}
}

bool serverConf::is_location(std::string& str) const {
	std::map<std::string, locationConf>::const_iterator it;
	for (it = locations_.begin(); it != locations_.end(); ++it){
		if ((*it).first.compare(0, str.size() + 1, str) == 0)
			return true;
	}
	return false;
}

const locationConf& serverConf::get_location(const std::string& path) const {
	std::map<std::string, locationConf>::const_iterator it = locations_.find(path);
	if (it == locations_.end()) {
		throw std::out_of_range("location not found");
	}
	return (*it).second;
}

const locationConf& serverConf::identifie_location(const std::string& str) const {
	std::stringstream ss(str);
	std::string part;
	std::vector<std::string> stack;

	while (std::getline(ss, part, '/')) {
		if (part.empty())
			continue;
		stack.push_back(part);
	}

	std::string tmp_path;
	while (stack.size()) {
		// form the path
		tmp_path.clear();
		for (size_t i = 0; i < stack.size(); ++i) {
			tmp_path += "/" + stack[i];
		}
		// check with location
		if (is_location(tmp_path)) {
			return get_location(tmp_path);
		} else {
			if (!stack.empty())
				stack.pop_back();
		}
	}
	return get_location("/");
}
