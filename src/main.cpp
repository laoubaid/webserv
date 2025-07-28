/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/16 16:22:40 by laoubaid          #+#    #+#             */
/*   Updated: 2025/07/28 16:22:57 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "include.hpp"
#include "server/Server.hpp"
#include "server/Client.hpp"

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
	epoll_event clt_event {};
	clt_event.data.fd = client_fd;
	clt_event.events = EPOLLIN | EPOLLOUT;// | EPOLLET;
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

int main(int ac, char **av)
{
	int		 server_fd;

	t_conf	conf;

	
	init_conf(&conf);
	Server	svr_skt(conf);

	server_fd = svr_skt.get_fd();
	svr_skt.bind();
	svr_skt.listen(SOMAXCONN);

	
	int epoll_fd = epoll_create1(0);
	if (epoll_fd == -1)
		socket_related_err(" epoll_create1() failed! ", 1);

	epoll_event svr_event {}; // init all struct member to 0
	epoll_event eventQueue[100]; // change to [MAX_EVENTS] later

	svr_event.data.fd = server_fd;
	svr_event.events = EPOLLIN;  // We’re interested in readable events (e.g., when a client connects or sends data).
	
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &svr_event) == -1) {
		close(epoll_fd);
		socket_related_err(" epoll_ctl() failed! ", 1);
	}

    int buf_size = 100;
    std::map <int, Client*> client_sockets;


	while (true) {
		int n = epoll_wait(epoll_fd, eventQueue, 100, -1); // chang to max_events
		if (n == -1) {
			close(epoll_fd);
			socket_related_err(" epoll_wait() failed! ", 1);
		}
		for (int i = 0; i < n; i++) {
			
            // first the server socket check for new connections
            ////////////////////////////////////////////////////
            
            if (eventQueue[i].data.fd == server_fd) {
				int client_fd = new_connection(server_fd, epoll_fd);
				if (client_fd == -1)
					continue;
                if (set_to_NonBlocking(client_fd)) // is this allowed
                    socket_related_err(" fcntl() failed! ", 1); // not clean wth
                client_sockets[client_fd] = new Client(client_fd);
			}
            
            
            // the client socket check for data receiving and sending
            /////////////////////////////////////////////////////////

            else {
				int client_fd = eventQueue[i].data.fd;
                char buf[buf_size];
                std::vector <char> vec_buf;

                memset(buf, 0, buf_size); // Clear the buffer before each recv

				if (eventQueue[i].events & EPOLLIN) {
                    // std::cout << "EPOLLIN event detected!" << std::endl;
					int nread = recv(client_fd, buf, buf_size - 1, 0);

                    vec_buf.assign(buf, buf + nread);

					if (nread <= 0) {
                        if (nread == 0)
                            std::cout << "Client disconnected! (recv() == 0)" << std::endl;
                        else
                            perror("recv() failed");
						close(client_fd);
						epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
						std::cout << DISC_CLR << "\n$ Client disconnected! (epoll IN) fd: " << client_fd << DEF_CLR << std::endl;
						continue;
					}

                    client_sockets[client_fd]->process_recv_data(vec_buf, eventQueue[i].events);
				}
			}
		}
	}
	close(server_fd);

	return 0;
}
