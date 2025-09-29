/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/15 15:47:55 by laoubaid          #+#    #+#             */
/*   Updated: 2025/09/27 11:18:01 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webServ.hpp"

webServ::webServ() {}

webServ::webServ(std::vector<serverConf>& s_cfgs) {
	srv_cfgs = s_cfgs;
	std::cout << "\n\nStarting web server..." << std::endl;
}

webServ::~webServ()
{
	for (std::map<int, Server*>::iterator it = srvr_skts_.begin(); it != srvr_skts_.end(); ++it)
		delete it->second;
	srvr_skts_.clear();
	close(epoll_fd_);
}

int webServ::setup_epoll() {
	epoll_fd_ = epoll_create1(0);
	if (epoll_fd_ == -1)
		throw std::runtime_error("epoll_create1() failed! ");

	return 0;
}

int webServ::setup_servers() {
	Server *	svtmp;

	for (size_t i = 0; i < srv_cfgs.size(); ++i) {
		const std::vector<sockaddr_in> listens = srv_cfgs[i].get_addrs();
		for (size_t j = 0; j < listens.size(); ++j) {
			svtmp = new Server(srv_cfgs[i], listens[j]);
			int svfd = svtmp->get_fd();
			srvr_skts_[svfd] = svtmp;
		}
	}

	int			srv_cout = 0;
	std::map<int, Server*>::iterator	it;

	for (it = srvr_skts_.begin(); it != srvr_skts_.end(); ++it) {
		if (it->second->launch())
			continue;
		it->second->add_to_epoll(epoll_fd_);
		srv_cout++;
	}
	
	return srv_cout;
}


Server*	webServ::get_server(int fd) {
	std::map<int, Server*>::iterator	it;
	
	for (it = srvr_skts_.begin(); it != srvr_skts_.end(); ++it) {
		if ((*it).second->client_sockets.find(fd) != (*it).second->client_sockets.end()) {
			return (*it).second;
		}
	}
	return NULL;
}

int	webServ::handle_connections(int fd) {
	std::map<int, Server *>::iterator	it;

	it = srvr_skts_.find(fd);
	if (it != srvr_skts_.end())
		return (*it).second->accept_connections(epoll_fd_);								//* it returns the number of accepted clients
	return 0;
}

int webServ::handle_clients(epoll_event clt_evt) {
	int									stat_;
	int									client_fd = clt_evt.data.fd;
	Server*								svr_skt = get_server(client_fd);
	std::map<int, Client *>::iterator	it;

	if (svr_skt) {
		it = svr_skt->client_sockets.find(client_fd);
		if (it != svr_skt->client_sockets.end()) {
			stat_ = (*it).second->get_state();
			if (stat_ == CGI) {
				(*it).second->set_cgi_obj(cgi_pipes, 1);
				(*it).second->set_req_state(ACGI);
				return 0;
			}
			if (clt_evt.events & EPOLLIN) {
				// std::cout << "[EPOLLIN] event detected! > CLIENT: " << clt_evt.data.fd << std::endl;
				stat_ = (*it).second->receive(epoll_fd_);

				if (stat_ == -1) {
					(*it).second->set_cgi_obj(cgi_pipes, 0);
					return client_fd;
				}
			}
			if ((clt_evt.events & EPOLLOUT) && stat_ == RESP) {
				// std::cout << "[EPOLLOUT] event detected! > CLIENT: " << clt_evt.data.fd << std::endl;
				if ((*it).second->send_response()) {
					(*it).second->set_cgi_obj(cgi_pipes, 0);
					return client_fd;
				}
			} 
		}
	}
	return 0;
}

int webServ::handle_cgis(epoll_event event) {
	int									pipe_fd = event.data.fd;
	std::map<int, Client *>::iterator	it;

	it = cgi_pipes.find(pipe_fd);
	if (it != cgi_pipes.end()) {
		std::cout << "[INFO] WS EPOLL event detected! > PIPE: " << event.data.fd << std::endl;
		if ((*it).second->cgi_pipe_io(pipe_fd))
			return pipe_fd;
	}

	return 0;
}

void webServ::run() {
	std::vector<int>	toBeKilled;
	int					nevents = 0;

	signal(SIGPIPE, SIG_IGN);
	init_status_lines();
	init_error_pages();

	while (true) {
		nevents = epoll_wait(epoll_fd_, eventQueue, MAX_EVENTS_, -1);
		if (nevents < 0)
			throw std::runtime_error("epoll_wait() failed!");
		
		for (int i = 0; i < nevents; i++) {
			if (handle_connections(eventQueue[i].data.fd))
				continue;
			else if (int client_fd = handle_clients(eventQueue[i]))
				toBeKilled.push_back(client_fd);
			else {
				int pipe_fd = handle_cgis(eventQueue[i]);
				if (pipe_fd) {
					std::cout << "[INFO] WS adding pipe to the <toBeKilled> vector\n";
					toBeKilled.push_back(pipe_fd);
				}
			}
		}
		for (std::vector<int>::iterator it = toBeKilled.begin(); it != toBeKilled.end(); ) {
			int client_fd = *it;
			if (cgi_pipes.find(*it) != cgi_pipes.end()) {
				cgi_pipes.erase(*it);
				it = toBeKilled.erase(it);
			} else {
				Server *  svr_skt = get_server(client_fd);
				if (svr_skt) {
					epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, client_fd, NULL);
					if (svr_skt->client_sockets[client_fd]->kill_cgi()) {
						delete svr_skt->client_sockets[client_fd];
						it = toBeKilled.erase(it);
						svr_skt->client_sockets.erase(client_fd);
						continue;
					}
				}
				++it;
			}
		}
		check_timeouts();
	}
}

void webServ::check_timeouts() {
	// std::cout << "[INFO] WS checking timouts ...\n";
	std::map <int, Server*>::iterator it;

	for (it = srvr_skts_.begin(); it != srvr_skts_.end(); ++it) {
		(*it).second->check_timeout();
	}
}

