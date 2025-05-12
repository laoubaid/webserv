/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerSkt.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/20 19:01:01 by laoubaid          #+#    #+#             */
/*   Updated: 2025/05/12 04:35:05 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _SERVER_SOCKET_HPP_
#define _SERVER_SOCKET_HPP_

#include "../include.hpp"
#include "Socket.hpp"

class ServerSkt : public Socket
{
	private:
	public:
		ServerSkt(t_conf cfg);
		~ServerSkt();

};


#endif
