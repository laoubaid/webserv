/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/20 19:01:04 by laoubaid          #+#    #+#             */
/*   Updated: 2025/08/18 04:30:42 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(t_conf cfg) : Socket(cfg)
{
    std::cout << "Server constracteur called!" << std::endl;

	int reuse = 1;
	if (setsockopt(get_fd(), SOL_SOCKET, SO_REUSEADDR, (void *)&reuse, sizeof(reuse)) < 0) {
		perror("ERROR SO_REUSEADDR:");
		exit(EXIT_FAILURE);
	}
}

Server::~Server()
{
}

int Server::accept_connections(int epoll_fd) {
	int count = 0;
	int client_fd = accept(get_fd(), NULL, NULL);
	while (client_fd != -1) {
		count++;
		struct epoll_event clt_event;
		memset(&clt_event, 0, sizeof(clt_event));
		clt_event.data.fd = client_fd;
		clt_event.events = EPOLLIN;
		epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &clt_event);
		std::cout << CONN_CLR <<"\n$ New client connected! fd: " << client_fd << DEF_CLR << std::endl;

		client_sockets[client_fd] = new Client(client_fd);

		client_fd = accept(get_fd(), NULL, NULL);
	}
	return count;
}

int	Server::add_to_epoll(int epoll_fd) {
	struct epoll_event svr_event;
	memset(&svr_event, 0, sizeof(svr_event)); // init all struct member to 0

	svr_event.data.fd = get_fd();
	svr_event.events = EPOLLIN;// | EPOLLET; // We’re interested in readable events (e.g., when a client connects)
	
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, get_fd(), &svr_event) == -1) {
		close(epoll_fd);
		return socket_related_err("Server: epoll_ctl() failed! ", 1);
	}
	return 0;
}

int Server::launch() {
	if (bind() == 0 && listen(SOMAXCONN) == 0) {
		std::cout << GRN_CLR << "Server is listening on port " << ntohs(get_sockeaddr().sin_port) << DEF_CLR << std::endl;
		return 0;
	}
	return 1;
}
