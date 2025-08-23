/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webServ.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/15 15:47:51 by laoubaid          #+#    #+#             */
/*   Updated: 2025/08/22 10:29:05 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include "../include.hpp"
# include "../config/serverConf.hpp"
# include "Server.hpp"
# include "Client.hpp"

# define MAX_EVENTS_ 256

class webServ
{
    private:
        int                     epoll_fd_;
        std::vector<serverConf> srv_cfgs;
        std::map <int, Server*> srvr_skts_;

        epoll_event             eventQueue[MAX_EVENTS_]; // change to [MAX_EVENTS] later
    public:
        webServ();
        webServ(std::vector<serverConf>& s_cfgs);
        ~webServ();

        int setup_epoll();
        int setup_servers();
        Server* get_server(int svfd);
        int handle_connections(int fd);
        int handle_clients(epoll_event clt_evt);
        int run();
};


#endif
