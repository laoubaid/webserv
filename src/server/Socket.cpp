/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 11:24:34 by laoubaid          #+#    #+#             */
/*   Updated: 2025/08/11 14:22:27 by laoubaid         ###   ########.fr       */
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
	std::cout << "Socket destructor called!" << std::endl;
	close(s_fd_);
}

int Socket::get_fd() const {
    return s_fd_;
}


int Socket::bind() {
	if (::bind(s_fd_, (const sockaddr *)&s_addr_, sizeof(s_addr_)))
		return socket_related_err(" bind() failed! ", 1);
	return 0;
}

int Socket::listen(int s__n) {
    if (::listen(s_fd_, s__n)) {
        return socket_related_err(" listen() failed! ", 1);
    }
	return 0;
}

int socket_related_err(const char *msg, int should_exit)
{
    std::cerr << RED_CLR << "ERROR :" << DEF_CLR; // clean this shit
	fflush(stderr);
	perror(msg);
	return should_exit;
}
