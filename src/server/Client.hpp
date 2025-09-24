/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/15 14:33:58 by laoubaid          #+#    #+#             */
/*   Updated: 2025/09/23 16:01:38 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _CLIENT_SOCKET_HPP_
#define _CLIENT_SOCKET_HPP_

#include "../req/HTTPRequestParser.hpp"
#include "../req/httpParsingIncludes.hpp"
#include "../resp/HTTPResponse.hpp"
#include "../config/serverConf.hpp"
#include "../include.hpp"
#include "Socket.hpp"

# include "../cgi/Cgi.hpp"

# define RECV_BUF 4096
// # define RECV_BUF 128
// # define RECV_BUF 64

class Client : Socket
{
	private:
		Request				*request_;
		HttpResponse		*response_;
		const serverConf&	conf_;

		std::string			resbuf_;
		Uvec				vec_buf_;

		std::time_t			timeout_;

		int					epoll_fd_;
		Cgi*				cgi_;

		sockaddr_in			client_addr_;

	public:
		Client(int clt_fd, const serverConf& conf, int ep_fd, sockaddr_in client_addr);
		~Client();
		int receive(int epoll_fd);

		int process_recv_data();//Uvec &vec_buf);//, uint32_t event);
		void print_whatever(std::string whatever);

		int send_response();

		bool operator<(const Client &other) const {
			return this->get_fd() < other.get_fd();
		}

		int get_state() const {
			return (request_) ? request_->getReqState() : -1;
		}

		int get_fd_client(); //!  redifine

		void	set_req_state(t_req_state stat) {
			if (request_)
				request_->setReqState(stat);
		}

		void set_cgi_obj(std::map <int, Client*> &cgi_pipes, int flag);

		int cgi_pipe_io(int pipfd);

		sockaddr_in get_client_addr();

		bool	check_timeout();
		void	reset_timeout() { timeout_ = std::time(NULL);}

		void log() const;

};


#endif

