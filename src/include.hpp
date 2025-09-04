/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   include.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kez-zoub <kez-zoub@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/15 12:50:12 by laoubaid          #+#    #+#             */
/*   Updated: 2025/09/04 10:23:16 by kez-zoub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _INCLUDE_HPP_
# define _INCLUDE_HPP_

# include <unistd.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <error.h>
# include <sys/socket.h>
# include <iostream>
# include <arpa/inet.h>
# include <sys/epoll.h>
# include <map>
# include <set>
# include <vector>
# include <sstream>
# include <fstream>

# define DEF_CLR "\033[0m"
# define BLK_CLR "\033[30m"
# define RED_CLR "\033[31m"
# define GRN_CLR "\033[32m"
# define YLW_CLR "\033[33m"
# define WHT_CLR "\033[37m"
# define DISC_CLR "\033[37;41m"
# define CONN_CLR "\033[30;42m"


# define HEADER_MAX_SIZE 1024

// class locationConf;

// # include "./config/serverConf.hpp"
// # include "./config/locationConf.hpp"

typedef struct	s_conf
{
	int			_domain;
	int			_type;
	int			_protocol;

	sockaddr_in _address;

}				t_conf;


typedef struct	s_conf2
{
	
}				t_conf2;

std::string         resolve_path(const std::string& str);

# include "req/httpParsingIncludes.hpp"
# include "req/HTTPRequestParser.hpp"
# include "cgi/Cgi.hpp"

#endif
