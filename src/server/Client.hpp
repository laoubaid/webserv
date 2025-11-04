/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/15 14:33:58 by laoubaid          #+#    #+#             */
/*   Updated: 2025/10/31 16:23:07 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _CLIENT_SOCKET_HPP_
#define _CLIENT_SOCKET_HPP_

#include "../req/Request.hpp"
#include "../req/httpParsingIncludes.hpp"
#include "../resp/HTTPResponse.hpp"
#include "../config/serverConf.hpp"
#include "../include.hpp"
#include "Socket.hpp"

# include "../cgi/Cgi.hpp"

# define RECV_BUF 8192

class Client : Socket
{
	private:
		Request				*request_;
		HttpResponse		*response_;
		const serverConf&	conf_;

		std::string			resbuf_;
		Uvec				vec_buf_;

		std::time_t			req_timeout_;
		std::time_t			cgi_timeout_;
		std::time_t			resp_timeout_;
		int					state_timout_;

		int					epoll_fd_;
		Cgi*				cgi_;

		sockaddr_in			client_addr_;

	public:
		Client(int clt_fd, const serverConf& conf, int ep_fd, sockaddr_in client_addr);
		~Client();
		int receive(int epoll_fd);

		int process_recv_data();

		int send_response();

		bool operator<(const Client &other) const {
			return this->get_fd() < other.get_fd();
		}

		int get_state() const {
			return (request_) ? request_->getReqState() : -1;
		}

		int get_fd_client();

		void	set_req_state(t_req_state stat) {
			if (request_)
				request_->setReqState(stat);
		}

		void set_cgi_obj(std::map <int, Client*> &cgi_pipes, int flag);

		int cgi_pipe_io(int pipfd);

		sockaddr_in get_client_addr();

		bool	check_timeout();
		void	reset_req_timeout() { req_timeout_ = std::time(NULL);}
		void	reset_cgi_timeout() { cgi_timeout_ = std::time(NULL);}
		void	reset_resp_timeout() { resp_timeout_ = std::time(NULL);}

		int		kill_cgi();

};


#endif

