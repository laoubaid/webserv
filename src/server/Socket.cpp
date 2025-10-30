/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 11:24:34 by laoubaid          #+#    #+#             */
/*   Updated: 2025/10/30 16:45:36 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"

Socket::Socket(sockaddr_in addr)
{
	// std::cout << "Socket SVR Default constracteur called!" << std::endl;
	s_fd_ = socket(addr.sin_family, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if (s_fd_ == -1) {
		throw std::runtime_error("socket() creation failed! ");
	}
	s_addr_ = addr;
}

Socket::Socket(int clt_fd){
	// std::cout << "Socket CLT Default constracteur called!" << std::endl;
	s_fd_ = clt_fd;
}


Socket::~Socket() {
	// std::cout << "Socket destructor called!" << std::endl;
	close(s_fd_);
}

int Socket::get_fd() const {
	return s_fd_;
}


int Socket::bind() {
	if (::bind(s_fd_, (const sockaddr *)&s_addr_, sizeof(s_addr_)))
		return 1;
		// throw std::runtime_error("bind() failed!");

	return 0;
}

int Socket::listen(int s__n) {
	if (::listen(s_fd_, s__n)) {
		return 1;
		// throw std::runtime_error("listen() failed!");
	}
	return 0;
}

