/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 11:24:34 by laoubaid          #+#    #+#             */
/*   Updated: 2025/05/02 17:48:51 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"

Socket::Socket(t_conf cfg)
{
    std::cout << "Socket SVR Default constracteur called!" << std::endl;
	s_fd_ = socket(cfg._domain, cfg._type, cfg._protocol);
    if (s_fd_ == -1) {
		socket_related_err(" socket() failed! ", 1);
	}
    s_addr_ = cfg._address;
}

Socket::Socket(int clt_fd){
    std::cout << "Socket CLT Default constracteur called!" << std::endl;
	s_fd_ = clt_fd;
}


Socket::~Socket() {
	close(s_fd_);
}

int Socket::get_fd() const {
    return s_fd_;
}


void Socket::bind() {
	if (::bind(s_fd_, (const sockaddr *)&s_addr_, sizeof(s_addr_))) {
		socket_related_err(" bind() failed! ", 1);
	}
}

void Socket::listen(int s__n) {
    if (::listen(s_fd_, s__n)) {
        socket_related_err(" listen() failed! ", 1);
    }
}

void socket_related_err(const char *msg, int should_exit)
{
    std::cerr << RED_CLR << "ERROR :" << DEF_CLR; // clean this shit
	fflush(stderr);
	perror(msg);
	if (should_exit) {
		exit(EXIT_FAILURE); // Use EXIT_FAILURE for standard error exit
	}
}
