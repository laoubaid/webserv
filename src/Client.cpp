/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/16 16:27:54 by laoubaid          #+#    #+#             */
/*   Updated: 2025/04/16 18:29:51 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client(/* args */)
{
}

Client::~Client()
{
}


// while (true) {
// 	int n = epoll_wait(epfd, events, MAX_EVENTS, -1);
// 	if (n == -1)
// 		socket_related_err("epoll_wait", epfd, 1);

// 	for (int i = 0; i < n; ++i) {
// 		if (events[i].data.fd == server_fd) {
// 			// New client
// 			int client_fd = accept(server_fd, NULL, NULL);
// 			if (client_fd == -1)
// 				continue;
// 			make_socket_non_blocking(client_fd);
// 			epoll_event client_event {};
// 			client_event.data.fd = client_fd;
// 			client_event.events = EPOLLIN;
// 			epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &client_event);
// 			std::cout << "New client connected\n";
// 		} else {
// 			// Client sent data
// 			int client_fd = events[i].data.fd;
// 			int r = recv(client_fd, buf, sizeof(buf) - 1, 0);
// 			if (r <= 0) {
// 				close(client_fd);
// 				continue;
// 			}
// 			buf[r] = '\0';
// 			std::cout << "Request:\n" << buf << "\n------------------\n";
// 			send(client_fd, response.c_str(), response.size(), 0);
// 			close(client_fd); // close after response (HTTP 1.0 style)
// 		}
// 	}
// }