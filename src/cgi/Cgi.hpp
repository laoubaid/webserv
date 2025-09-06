/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/01 18:59:34 by kez-zoub          #+#    #+#             */
/*   Updated: 2025/09/04 15:34:23 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
# define CGI_HPP

# include "../include.hpp"
# include <iostream>
# include <unistd.h>
# include <sys/wait.h>

class Request;

class Cgi
{
	private:
		char	**argv;
		char	**env;
		std::string	root;
		std::string	cgi_dir;
		std::string	script_name;
		std::string	path_info;
		Uvec		body;
		int			child_pid;
		// int			_stdin;
		// int		_stdout;

		void	set_argv(void);
		void	set_env(const Request& req);
	public:
		Cgi(void);
		~Cgi(void);

		void	set_script_name(const std::string &name);
		void	set_path_info(const std::string &path);
		std::string	run(const Request& req);

		// to be deleted
		void	print_env(void);
};

#endif
