/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/16 16:22:40 by laoubaid          #+#    #+#             */
/*   Updated: 2025/08/09 18:00:25 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "include.hpp"
#include "server/Server.hpp"
#include "server/Client.hpp"
#include "config/configParser.hpp"

void	init_conf(t_conf *conf) {
	conf->_domain = AF_INET;
	conf->_type = SOCK_STREAM | SOCK_NONBLOCK;
	conf->_protocol = 0;
	conf->_address.sin_family = conf->_domain;
	conf->_address.sin_addr.s_addr = INADDR_ANY;
	conf->_address.sin_port = htons(8080);
}

int	new_connection(int server_fd, int epoll_fd) {
	int client_fd = accept(server_fd, NULL, NULL);
	if (client_fd == -1)
		return -1;
	struct epoll_event clt_event;
	memset(&clt_event, 0, sizeof(clt_event));
	clt_event.data.fd = client_fd;
	clt_event.events = EPOLLIN | EPOLLET;
	epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &clt_event);
	std::cout << CONN_CLR <<"\n$ New client connected! fd: " << client_fd << DEF_CLR << std::endl;

	return client_fd;
} 

int set_to_NonBlocking(int client_fd){
    int flags = fcntl(client_fd, F_GETFL, 0);
    if (flags == -1)
        return -1;
    flags |= O_NONBLOCK;
    return (fcntl(client_fd, F_SETFL, flags));
}

void setEvent(int epoll_fd, int client_fd, uint32_t event_type) {
    epoll_event ev;
    ev.events = event_type;  // or EPOLLIN | EPOLLET
    ev.data.fd = client_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_MOD, client_fd, &ev);
}

int main(int ac, char **av)
{
	int		 server_fd;

	t_conf	conf;

	if (ac != 2)
		return 1; // more details

	if (get_config(av[1]).name == "Error")
		return 1; // if the config file is not found or has errors
	std::cout << "\033[32mConfiguration file parsed successfully!\033[0m" << std::endl;
	std::cout << "\n\nStarting server..." << std::endl;

	init_conf(&conf);
	Server	svr_skt(conf);

	server_fd = svr_skt.get_fd();
	set_to_NonBlocking(server_fd);   // chang this plz its forbidden
	svr_skt.bind();
	svr_skt.listen(SOMAXCONN);

	
	int epoll_fd = epoll_create1(0);
	if (epoll_fd == -1)
		socket_related_err(" epoll_create1() failed! ", 1);

	struct epoll_event svr_event;
	memset(&svr_event, 0, sizeof(svr_event)); // init all struct member to 0
	epoll_event eventQueue[100]; // change to [MAX_EVENTS] later

	svr_event.data.fd = server_fd;
	svr_event.events = EPOLLIN | EPOLLET; // We’re interested in readable events (e.g., when a client connects)
	
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &svr_event) == -1) {
		close(epoll_fd);
		socket_related_err(" epoll_ctl() failed! ", 1);
	}

    std::map <int, Client*> client_sockets;  // this needs to exists for each server

	std::vector<Client *> leftovers;
	while (true) {
		int nevents = 0;
		// 1. Handle incomplete recv (non-blocking)
		std::cout << "++++++++++++++++++++++++++++++\n";
		// if (!leftovers.empty()) {
		// 	for (auto it = leftovers.begin(); it != leftovers.end(); ) {
		// 		if ((*it)->receive(epoll_fd) == PEND) { // or returns bool 
		// 			++it;
		// 		} else {
		// 			it = leftovers.erase(it); // done receiving, remove from leftovers
		// 		}
		// 	}
		// 	nevents = epoll_wait(epoll_fd, eventQueue, 100, 0); // Don't block, just poll for ready fds
		// } else {
		// 	nevents = epoll_wait(epoll_fd, eventQueue, 100, -1); // Block until new events
		// }
		nevents = epoll_wait(epoll_fd, eventQueue, 100, -1);
		if (nevents < 0) {
			close(epoll_fd);
			socket_related_err(" epoll_wait() failed! ", 1);
		}
		
		// 2. Handle triggered events
		
		for (int i = 0; i < nevents; i++) {
			
			// first the server socket check for new connections
            ////////////////////////////////////////////////////
            
            if (eventQueue[i].data.fd == server_fd) {
				int client_fd = new_connection(server_fd, epoll_fd);
				if (client_fd == -1)
				continue;
                // if (set_to_NonBlocking(client_fd)) // is this allowed
                //     socket_related_err(" fcntl() failed! ", 1); // not clean wth
				client_sockets[client_fd] = new Client(client_fd);
			}
            
			// the client socket check for data receiving and sending
			/////////////////////////////////////////////////////////
				
			else {
				int client_fd = eventQueue[i].data.fd;
				int stat_;

				if (eventQueue[i].events & EPOLLIN) {
                    std::cout << "EPOLLIN event detected!" << std::endl;
					
					stat_ = client_sockets[client_fd]->receive(epoll_fd);
					if (stat_ == PEND) {
						client_sockets[client_fd]->set_event(epoll_fd, EPOLLIN);
						// leftovers.push_back(client_sockets[client_fd]);
						// std::cout << "adding the client into leftovers list" << std::endl;
					}
					if (stat_ == RESP)
						client_sockets[client_fd]->set_event(epoll_fd, EPOLLIN | EPOLLOUT | EPOLLET);
					if (stat_ == -1) {
						delete client_sockets[client_fd];
						client_sockets.erase(client_fd);
						continue;
					}
				}
				if (eventQueue[i].events & EPOLLOUT) {
					std::cout << "EPOLLOUT event detected!" << std::endl;
					if (client_sockets[client_fd]->send_response(epoll_fd)) {
						delete client_sockets[client_fd];
						client_sockets.erase(client_fd);
					}
				}
			}
		}
	}
	close(server_fd);

	return 0;
}
