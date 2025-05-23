/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerSkt.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/20 19:01:01 by laoubaid          #+#    #+#             */
/*   Updated: 2025/05/23 19:24:06 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _SERVER_SOCKET_HPP_
#define _SERVER_SOCKET_HPP_

#include "../include.hpp"
#include "Socket.hpp"

class Server : public Socket
{
	private:
	public:
		Server(t_conf cfg);
		~Server();

};


#endif
