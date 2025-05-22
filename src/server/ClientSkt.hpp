/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientSkt.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/15 14:33:58 by laoubaid          #+#    #+#             */
/*   Updated: 2025/05/13 23:05:26 by laoubaid         ###   ########.fr       */
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
        std::string init_req_;
        int         recv_done_;
        int         header_len;

        HTTPRequestParser *request_;
        std::string response_;

    public:
        ClientSkt(int clt_fd);
        ~ClientSkt();

        char *get_request() {
            return (char *)init_req_.c_str();
        }
        int process_recv_data(const char *buf, int len, uint32_t event);
        void print_request();
        
        bool is_recv_done() const {
            return recv_done_;
        }
        void reset_recv_done() {
            recv_done_ = 0;
        }

        std::string get_response() {
            return response_;
        }

        void send_response(uint32_t event);


        bool operator<(const ClientSkt &other) const {
            return this->get_fd() < other.get_fd();
        }

};


#endif

