/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/15 14:33:58 by laoubaid          #+#    #+#             */
/*   Updated: 2025/08/08 17:39:58 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _CLIENT_SOCKET_HPP_
#define _CLIENT_SOCKET_HPP_

#include "../req/HTTPRequestParser.hpp"
#include "../req/httpParsingIncludes.hpp"
#include "../resp/HTTPResponse.hpp"
#include "../include.hpp"
#include "Socket.hpp"

# define RECV_BUF 4096

class Client : Socket
{
    private:
        HTTPRequestParser   *request_;
        HttpResponse        *response_;
        t_req_state          req_stat_;

        std::string         resbuf_;
        Uvec                vec_buf_;

    public:
        Client(int clt_fd);
        ~Client();
        int receive(int epoll_fd);

        int process_recv_data();//Uvec &vec_buf);//, uint32_t event);
        // void print_request();

        void send_response(uint32_t event, int epoll_fd);


        bool operator<(const Client &other) const {
            return this->get_fd() < other.get_fd();
        }

};


#endif

