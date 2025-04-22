/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 11:24:29 by laoubaid          #+#    #+#             */
/*   Updated: 2025/04/22 11:17:25 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _SOCKET_HPP_
#define _SOCKET_HPP_

#include "include.hpp"

#define SERVER 1
#define CLIENT 2

class Socket
{
	private:
		int			s_fd_;
		sockaddr_in	s_addr_;

	public:
		Socket(t_conf cfg);
		Socket(int skt_fd);
		~Socket();

        int get_fd();
		void bind();
		void listen(int s__n);
		// void socket_related_err(const char *msg, int fd, int should_exit);
};

void socket_related_err(const char *msg, int should_exit);

#endif
