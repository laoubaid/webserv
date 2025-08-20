/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/16 16:22:40 by laoubaid          #+#    #+#             */
/*   Updated: 2025/08/19 22:38:24 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "include.hpp"
#include "server/webServ.hpp"
#include "config/configParser.hpp"


int main(int ac, char **av)
{

	// webServ WS;

	if (ac != 2)
		return 1; // more details

	std::cout << "-------------------------------------------------------------------\n";
	if (get_config(av[1]))
		return 1; // if the config file is not found or has errors
	std::cout << "\033[32mConfiguration file parsed successfully!\033[0m" << std::endl;
	
	// std::cout << "\n\nStarting server..." << std::endl;
	// WS.setup_epoll();
	// WS.setup_servers();
	// WS.run();

	return 0;
}
