/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/20 19:01:04 by laoubaid          #+#    #+#             */
/*   Updated: 2025/09/27 11:34:09 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(const serverConf& cfg, sockaddr_in addr) : Socket(addr), addr_(addr), conf_(cfg)
{
	std::cout << "Server constracteur called!" << std::endl;
	int reuse = 1;
	if (setsockopt(get_fd(), SOL_SOCKET, SO_REUSEADDR, (void *)&reuse, sizeof(reuse)) < 0)
		throw std::runtime_error("failed to set socket option to reuse address!");
}

Server::~Server() {
	for (std::map<int, Client*>::iterator it = client_sockets.begin(); it != client_sockets.end(); ++it)
		delete it->second;
	client_sockets.clear();
}

int	Server::accept_connections(int epoll_fd) {
	int	count = 0;
	int	client_fd;
	
	struct sockaddr_in client_addr;
	socklen_t addr_len = sizeof(client_addr);

	client_fd = accept(get_fd(), (struct sockaddr *)&client_addr, &addr_len);
	while (client_fd != -1) {
		count++;
		struct epoll_event clt_event;
		memset(&clt_event, 0, sizeof(clt_event));
		clt_event.data.fd = client_fd;
		clt_event.events = EPOLLIN | EPOLLOUT;
		epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &clt_event);
		std::cout << CONN_CLR <<"\n$ New client connected! fd: " << client_fd << DEF_CLR << std::endl;

		client_addr.sin_port = addr_.sin_port;
		client_sockets[client_fd] = new Client(client_fd, conf_, epoll_fd, client_addr);
		client_fd = accept(get_fd(), (struct sockaddr *)&client_addr, &addr_len);
	}
	return count;
}

void	Server::add_to_epoll(int epoll_fd) {
	struct epoll_event	svr_event;

	memset(&svr_event, 0, sizeof(svr_event));
	svr_event.data.fd = get_fd();
	svr_event.events = EPOLLIN | EPOLLOUT;
	
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, get_fd(), &svr_event) == -1) {
		throw std::runtime_error("Server: epoll_ctl() failed!");
	}
}

int	Server::launch() {
	if (bind() == 0 && listen(SOMAXCONN) == 0) {									//! info about bind and listen
		std::cout << GRN_CLR << "Server_" << get_fd() << " is listening on port " << ntohs(get_sockeaddr().sin_port) << DEF_CLR << std::endl;
		return 0;
	}
	return 1;
}

void Server::check_timeout() {
	std::map<int, Client*>::iterator it = client_sockets.begin();

	while (it != client_sockets.end()) {
		if (it->second->check_timeout()) {
			std::cout << "[INFO] SVR timeouted client " << std::endl;
		} else {
			++it;
		}
	}
}
