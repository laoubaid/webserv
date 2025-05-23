/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   include.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/15 12:50:12 by laoubaid          #+#    #+#             */
/*   Updated: 2025/05/23 19:22:11 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _INCLUDE_HPP_
#define _INCLUDE_HPP_

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <error.h>
#include <sys/socket.h>
#include <iostream>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/epoll.h>

#include <map>

#include <vector>

#define DEF_CLR "\033[0m"
#define BLK_CLR "\033[30m"
#define RED_CLR "\033[31m"
#define GRN_CLR "\033[32m"
#define YLW_CLR "\033[33m"
#define WHT_CLR "\033[37m"
#define DISC_CLR "\033[37;41m"
#define CONN_CLR "\033[30;42m"


#define HEADER_MAX_SIZE 1024

enum recv_state {
    IDLE = 0,
    PEND = 1,
    RESP = 2,
    CCLS = 3
};

typedef struct	s_conf
{
	int			_domain;
	int			_type;
	int			_protocol;

	sockaddr_in _address;

}				t_conf;


#endif
