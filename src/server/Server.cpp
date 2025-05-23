/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerSkt.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/20 19:01:04 by laoubaid          #+#    #+#             */
/*   Updated: 2025/05/23 19:23:37 by laoubaid         ###   ########.fr       */
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