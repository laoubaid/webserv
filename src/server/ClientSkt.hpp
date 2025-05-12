/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientSkt.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/15 14:33:58 by laoubaid          #+#    #+#             */
/*   Updated: 2025/05/12 04:35:05 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef _CLIENT_SOCKET_HPP_
#define _CLIENT_SOCKET_HPP_

#include "../include.hpp"
#include "Socket.hpp"


class ClientSkt : Socket
{
    private:
        std::string request_;
        int         recv_done_;
    public:
        ClientSkt(int clt_fd);
        ~ClientSkt();

        char *get_request() {
            return (char *)request_.c_str();
        }
        int handle_request(const char *buf);
        void print_request();
        
        bool is_recv_done() const {
            return recv_done_;
        }
        void reset_recv_done() {
            recv_done_ = 0;
        }

        bool operator<(const ClientSkt &other) const {
            return this->get_fd() < other.get_fd();
        }

};


#endif

