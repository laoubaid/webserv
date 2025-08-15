/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/15 15:47:55 by laoubaid          #+#    #+#             */
/*   Updated: 2025/08/15 17:54:34 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webServ.hpp"

void	init_conf(t_conf *conf, int port) {
	conf->_domain = AF_INET;
	conf->_type = SOCK_STREAM | SOCK_NONBLOCK;
	conf->_protocol = 0;
	conf->_address.sin_family = conf->_domain;
	conf->_address.sin_addr.s_addr = INADDR_ANY;
	conf->_address.sin_port = htons(port);
}

webServ::webServ()
{
}

webServ::~webServ()
{
    for (std::map<int, Server*>::iterator it = srvr_skts_.begin(); it != srvr_skts_.end(); ++it) {
        delete it->second;
    }
    srvr_skts_.clear();
    close(epoll_fd_);
}

int webServ::setup_epoll() {
    epoll_fd_ = epoll_create1(0);
	if (epoll_fd_ == -1)
		return socket_related_err(" epoll_create1() failed! ", 1);
    // std::cout << "epoll fd: " << epoll_fd_ << std::endl;
    return 0;
}

int webServ::setup_servers() {
    t_conf conf;

    init_conf(&conf, 8080); // temporary
    Server *    svtmp = new Server(conf, epoll_fd_);
    int svfd = svtmp->get_fd();
    srvr_skts_[svfd] = svtmp;
    srvr_skts_[svfd]->launch();
    srvr_skts_[svfd]->add_to_epoll();

    // hardcoded second server to test if algo works (looks like its fine for now)
    // t_conf conf2;
    // init_conf(&conf2, 9999); // temporary
    // svtmp = new Server(conf2, epoll_fd_);
    // svfd = svtmp->get_fd();
    // srvr_skts_[svfd] = svtmp;
    // srvr_skts_[svfd]->launch();
    // srvr_skts_[svfd]->add_to_epoll();

    
    return 0;
}


Server* webServ::get_server(int fd) {
    std::map<int, Server *>::iterator it = srvr_skts_.find(fd);
    if (it != srvr_skts_.end()) {
        return (*it).second;
    } else {
        for (std::map<int, Server *>::iterator it = srvr_skts_.begin(); it != srvr_skts_.end(); ++it) {
            if ((*it).second->client_sockets.find(fd) != (*it).second->client_sockets.end())
                return (*it).second;
        }
    }
    return NULL;
}

int webServ::handle_connections(int fd) {
    std::map<int, Server *>::iterator it = srvr_skts_.find(fd);
    if (it != srvr_skts_.end()) {
        return (*it).second->accept_connections();    // it returns the number of accepted clients
    }
    return 0;
}

int webServ::handle_clients(epoll_event clt_evt) {
    int             stat_;
    int             client_fd = clt_evt.data.fd;
    Server *  svr_skt = get_server(client_fd);
    std::map<int, Client *>::iterator it = svr_skt->client_sockets.find(client_fd);

    if (clt_evt.events & EPOLLIN) {
        std::cout << "EPOLLIN event detected! " << clt_evt.data.fd << std::endl;
        
        stat_ = (*it).second->receive(epoll_fd_);
        if (stat_ == PEND)
            (*it).second->set_event(epoll_fd_, EPOLLIN);
        if (stat_ == RESP)
            (*it).second->set_event(epoll_fd_, EPOLLIN | EPOLLOUT);
        if (stat_ == -1)
            return client_fd;
    }
    if (clt_evt.events & EPOLLOUT) {
        std::cout << "EPOLLOUT event detected! " << clt_evt.data.fd << std::endl;
        if ((*it).second->send_response(epoll_fd_))
            return client_fd;
    }
    return 0;
}

int webServ::run() {
    std::vector<int> tobekilled;
    int nevents = 0;
    while (true) {
		std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++\n";
		nevents = epoll_wait(epoll_fd_, eventQueue, 100, -1);
		// while ((nevents = epoll_wait(epoll_fd_, eventQueue, MY_MAX_EVENTS, -1)) == -1) {                  // condition for debuging with strace (check man)
		// 	if (errno == EINTR) continue; // Interrupted by signal, retry
		// 	perror("epoll_wait");
		// 	exit(EXIT_FAILURE);
		// }
		if (nevents < 0) {
			close(epoll_fd_);
			return socket_related_err(" epoll_wait() failed! ", 1);
		}
		
		// 2. Handle triggered events
		for (int i = 0; i < nevents; i++) {
            if (handle_connections(eventQueue[i].data.fd))
                continue;
			if (int client_fd = handle_clients(eventQueue[i])) 
                tobekilled.push_back(client_fd);
		}
		for (std::vector<int>::iterator it = tobekilled.begin(); it != tobekilled.end(); ++it) {
            Server *  svr_skt = get_server(*it);
			epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, *it, NULL);
			delete svr_skt->client_sockets[*it];
			svr_skt->client_sockets.erase(*it);
		}
		tobekilled.clear();
	}
}

