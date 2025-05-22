/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientSkt.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/15 14:33:58 by laoubaid          #+#    #+#             */
/*   Updated: 2025/05/22 10:18:32 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _CLIENT_SOCKET_HPP_
#define _CLIENT_SOCKET_HPP_

#include "../req/HTTPRequestParser.hpp"
#include "../req/httpParsingIncludes.hpp"
#include "../include.hpp"
#include "Socket.hpp"

class ClientSkt : Socket
{
    private:
        HTTPRequestParser   *request_;
        recv_state          req_stat_;

        std::string         response_;

    public:
        ClientSkt(int clt_fd);
        ~ClientSkt();

        int process_recv_data(const char *buf, int len, uint32_t event);
        // void print_request();

        void send_response(uint32_t event);


        bool operator<(const ClientSkt &other) const {
            return this->get_fd() < other.get_fd();
        }

};


#endif

