/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 11:24:29 by laoubaid          #+#    #+#             */
/*   Updated: 2025/08/21 17:24:28 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _SOCKET_HPP_
#define _SOCKET_HPP_

#include "../include.hpp"

#define SERVER 1
#define CLIENT 2

class Socket
{
	private:
		int			s_fd_;
		sockaddr_in	s_addr_;

	public:
		Socket(sockaddr_in addr);
		Socket(int skt_fd);
		~Socket();

        int get_fd() const;
		int bind();
		int listen(int s__n);

		sockaddr_in get_sockeaddr() const {
			return s_addr_;
		}
};

#endif
