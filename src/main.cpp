/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/16 16:22:40 by laoubaid          #+#    #+#             */
/*   Updated: 2025/09/23 17:25:33 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "include.hpp"
#include "server/webServ.hpp"
#include "config/configParser.hpp"


int main(int ac, char **av)
{
	signal(SIGPIPE, SIG_IGN);
	init_status_lines(); //! move it away
	init_error_pages(); //! move it away


	if (ac != 2) {
		std::cerr << "[!] invalid number of arguments!\nUsage:\n\t./webserv <config file>" << std::endl;
		return 1; // more details
	}

	std::vector<serverConf>* servers;
	try {
		std::cout << "-------------------------------------------------------------------\n";
		servers = get_config(av[1]).parser();
		std::cout << "\033[32mConfiguration file parsed successfully!\033[0m" << std::endl;
		// return 1;
		webServ WS(*servers);
	
		WS.setup_epoll();
		WS.setup_servers();
		WS.run();
		std::cout << "cleaning..." << std::endl;
		delete servers;
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}

	return 0;
}
