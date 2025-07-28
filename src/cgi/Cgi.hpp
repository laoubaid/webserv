/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kez-zoub <kez-zoub@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/01 18:59:34 by kez-zoub          #+#    #+#             */
/*   Updated: 2025/07/15 17:14:27 by kez-zoub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
# define CGI_HPP

# include "../include.hpp"
# include <iostream>
# include <unistd.h>
# include <sys/wait.h>

class Cgi
{
	private:
		char	**argv;
		char	**env;

		void	set_argv(const std::string& target);
		void	set_env(const HTTPRequestParser& req);
	public:
		Cgi(void);
		~Cgi(void);

		std::string	run_cgi(const HTTPRequestParser& req);

		// to be deleted
		void	print_env(void);
};

#endif
