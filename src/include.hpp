/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   include.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/15 12:50:12 by laoubaid          #+#    #+#             */
/*   Updated: 2025/04/16 18:18:01 by laoubaid         ###   ########.fr       */
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

/// error handling 
#define socket_related_err(msg, fd, flag) \
	do { perror(msg); if (fd != -1) {close(fd);} if (flag) {exit(1);}} while (false)


#endif
