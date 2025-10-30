/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/16 16:22:40 by laoubaid          #+#    #+#             */
/*   Updated: 2025/10/30 16:51:37 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "include.hpp"
#include "server/webServ.hpp"
#include "config/configParser.hpp"

bool stopFlag = false;

void signalHandler(int signal) {
    if (signal == SIGINT) {
        stopFlag = true;
        std::cout << "\nSIGINT received! Stopping..." << std::endl;
    }
}

int main(int ac, char **av)
{
	signal(SIGINT, signalHandler);
	if (ac != 2) {
		std::cerr << "[!] invalid number of arguments!\nUsage:\n\t./webserv <config file>" << std::endl;
		return 1;
	}

	std::vector<serverConf>* servers = NULL;
	try {
		servers = get_config(av[1]).parser();
		std::cout << "\033[32mConfiguration file parsed successfully!\033[0m" << std::endl;
		webServ WS(*servers);
	
		WS.setup_epoll();
		if (WS.setup_servers())
			WS.run();

	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		std::cout << "cleaning..." << std::endl;
		delete servers;
		return 2;
	}
	delete servers;
	return 0;
}
