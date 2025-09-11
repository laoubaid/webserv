/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/20 19:01:01 by laoubaid          #+#    #+#             */
/*   Updated: 2025/09/07 15:22:01 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _SERVER_SOCKET_HPP_
# define _SERVER_SOCKET_HPP_

# include "../include.hpp"
# include "../config/serverConf.hpp"
# include "Socket.hpp"
# include "Client.hpp"

class Server : public Socket
{
	private:
	public:
		std::map <int, Client*>	client_sockets;
		const serverConf&		conf;

		Server(const serverConf& cfg, sockaddr_in addr);
		Server(const serverConf& cfg, int fd);
		~Server();

		int	accept_connections(int epfd);
		int	add_to_epoll(int epfd);
		int	launch();

};


#endif
