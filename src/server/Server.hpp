/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/20 19:01:01 by laoubaid          #+#    #+#             */
/*   Updated: 2025/09/26 02:42:49 by laoubaid         ###   ########.fr       */
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
		sockaddr_in			addr_;
		const serverConf&	conf_;
	
	public:
		std::map <int, Client*>	client_sockets;

		Server(const serverConf& cfg, sockaddr_in addr);
		~Server();

		int		accept_connections(int epfd);
		void	add_to_epoll(int epfd);
		int		launch();

		void check_timeout();
};


#endif
