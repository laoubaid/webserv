/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientSkt.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/16 16:27:54 by laoubaid          #+#    #+#             */
/*   Updated: 2025/04/22 11:15:33 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ClientSkt.hpp"

ClientSkt::ClientSkt(int clt_fd) : Socket(clt_fd)
{
    std::cout << "ClientSkt constracteur called!" << std::endl;
}

ClientSkt::~ClientSkt()
{
}
