/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/16 16:22:40 by laoubaid          #+#    #+#             */
/*   Updated: 2025/08/21 17:23:26 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "include.hpp"
#include "server/webServ.hpp"
#include "config/configParser.hpp"


int main(int ac, char **av)
{
	if (ac != 2) {
		std::cerr << "invalid number of arguments!\nUsage: ./webserv <config file>" << std::endl;
		return 1; // more details
	}

	try {
		std::cout << "-------------------------------------------------------------------\n";
		std::vector<serverConf> servers = get_config(av[1]).parser();
		std::cout << "\033[32mConfiguration file parsed successfully!\033[0m" << std::endl;
		
		// webServ WS(servers);
	
		// WS.setup_epoll();
		// WS.setup_servers();
		// WS.run();
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}


	return 0;
}
