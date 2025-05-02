/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientSkt.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/15 14:33:58 by laoubaid          #+#    #+#             */
/*   Updated: 2025/05/02 19:58:51 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef _CLIENT_SOCKET_HPP_
#define _CLIENT_SOCKET_HPP_

#include "include.hpp"
#include "Socket.hpp"


class ClientSkt : Socket
{
    private:
        std::string    request_;
    public:
        ClientSkt(int clt_fd);
        ~ClientSkt();
        // std::string get_response();

        char *get_request() {
            return (char *)request_.c_str();
        }
        int handle_request(const char *buf);
        void print_request();

        bool operator<(const ClientSkt &other) const {
            return this->get_fd() < other.get_fd();
        }

};


#endif

