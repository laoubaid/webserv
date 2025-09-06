/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/15 15:47:55 by laoubaid          #+#    #+#             */
/*   Updated: 2025/09/06 01:13:23 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webServ.hpp"

webServ::webServ()
{
}

webServ::webServ(std::vector<serverConf>& s_cfgs) {
    srv_cfgs = s_cfgs;
    std::cout << "\n\nStarting web server..." << std::endl;
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
        throw std::runtime_error("epoll_create1() failed! ");
    // std::cout << "epoll fd: " << epoll_fd_ << std::endl;
    return 0;
}

int webServ::setup_servers() {
    Server *    svtmp;
    for (size_t i = 0; i < srv_cfgs.size(); ++i) {
        const std::vector<sockaddr_in> listens = srv_cfgs[i].get_addrs();
        for (size_t j = 0; j < listens.size(); ++j) {
            svtmp = new Server(srv_cfgs[i], listens[j]);
            int svfd = svtmp->get_fd();
            srvr_skts_[svfd] = svtmp;
        }
    }
    std::map<int, Server*>::iterator it;
    for (it = srvr_skts_.begin(); it != srvr_skts_.end(); ++it) {
        if (it->second->launch())
            continue;
        it->second->add_to_epoll(epoll_fd_);
    }
    
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
        return (*it).second->accept_connections(epoll_fd_);    // it returns the number of accepted clients
    }
    return 0;
}

int webServ::handle_clients(epoll_event clt_evt) {
    int             stat_;
    int             client_fd = clt_evt.data.fd;
    Server *  svr_skt = get_server(client_fd);
    std::map<int, Client *>::iterator it = svr_skt->client_sockets.find(client_fd);

    stat_ = (*it).second->get_state();

    // if ((clt_evt.events & EPOLLOUT))
    //     std::cout << "EPOLLOUT event detected! " << clt_evt.data.fd << std::endl;
    // else
    //     std::cout << "EPOLLIN event detected! " << clt_evt.data.fd << std::endl;

    if (clt_evt.events & EPOLLIN) {
        // std::cout << "EPOLLIN event detected! " << clt_evt.data.fd << std::endl;
        stat_ = (*it).second->receive(epoll_fd_);

        if (stat_ == -1)
            return client_fd;
    }
    if ((clt_evt.events & EPOLLOUT) && stat_ == RESP) {
        // std::cout << "EPOLLOUT event detected! " << clt_evt.data.fd << std::endl;
        if ((*it).second->send_response())
            return client_fd;
    }
    return 0;
}

int webServ::run() {
    std::vector<int> tobekilled;
    int nevents = 0;
    while (true) {
		// std::cout << "- -\n";
		nevents = epoll_wait(epoll_fd_, eventQueue, MAX_EVENTS_, 500);
		// while ((nevents = epoll_wait(epoll_fd_, eventQueue, MAX_EVENTS_, -1)) == -1) {                  // condition for debuging with strace (check man)
		// 	if (errno == EINTR) continue; // Interrupted by signal, retry
		// 	perror("epoll_wait");
		// 	exit(EXIT_FAILURE);
		// }
		if (nevents < 0)
            throw std::runtime_error("epoll_wait() failed!");
		
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

