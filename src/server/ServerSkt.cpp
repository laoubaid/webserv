/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerSkt.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/20 19:01:04 by laoubaid          #+#    #+#             */
/*   Updated: 2025/04/22 11:15:53 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerSkt.hpp"

ServerSkt::ServerSkt(t_conf cfg) : Socket(cfg)
{
    std::cout << "ServerSkt constracteur called!" << std::endl;

	int reuse = 1;
	if (setsockopt(get_fd(), SOL_SOCKET, SO_REUSEADDR, (void *)&reuse, sizeof(reuse)) < 0) {
		perror("ERROR SO_REUSEADDR:");
		exit(EXIT_FAILURE);
	}
}

ServerSkt::~ServerSkt()
{
}