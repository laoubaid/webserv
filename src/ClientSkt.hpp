/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientSkt.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/15 14:33:58 by laoubaid          #+#    #+#             */
/*   Updated: 2025/04/22 11:18:07 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef _CLIENT_SOCKET_HPP_
#define _CLIENT_SOCKET_HPP_

#include "include.hpp"
#include "Socket.hpp"


class ClientSkt : Socket
{
    private:
    public:
        ClientSkt(int clt_fd);
        ~ClientSkt();
};


#endif

