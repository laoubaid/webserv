/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/15 14:33:58 by laoubaid          #+#    #+#             */
/*   Updated: 2025/07/28 12:21:39 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _CLIENT_SOCKET_HPP_
#define _CLIENT_SOCKET_HPP_

#include "../req/HTTPRequestParser.hpp"
#include "../req/httpParsingIncludes.hpp"
#include "../include.hpp"
#include "Socket.hpp"

class Client : Socket
{
    private:
        HTTPRequestParser   *request_;
        e_req_state          req_stat_;

        std::string         resbuf_;

    public:
        Client(int clt_fd);
        ~Client();

        int process_recv_data(std::vector <char> buf, uint32_t event);
        // void print_request();

        void send_response(uint32_t event);


        bool operator<(const Client &other) const {
            return this->get_fd() < other.get_fd();
        }

};


#endif

