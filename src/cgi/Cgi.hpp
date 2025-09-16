/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kez-zoub <kez-zoub@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/01 18:59:34 by kez-zoub          #+#    #+#             */
/*   Updated: 2025/09/16 18:18:44 by kez-zoub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
# define CGI_HPP

# include "../include.hpp"
# include <iostream>
# include <unistd.h>
# include <sys/wait.h>

class Request;
class locationConf;

class Cgi
{
	private:
		Request*	_req;
		char	**argv;
		char	**env;
		std::string	root;
		std::string	_interpreter;
		std::string	script_name;
		std::string	path_info;
		Uvec		body;
		t_cgi_state	_state;
		int			child_pid;
		int			_write_in;
		int			_read_from;
		std::ifstream	_file;
		void	set_argv(void);
		void	set_env(void);
		void	cgi_err(std::string throw_msg, int status_code, t_req_state req_state);
	public:
		Cgi(Request *req, std::string interp);
		~Cgi(void);

		void	set_script_name(const std::string &name);
		void	set_path_info(const std::string &path);
		void	write_body();
		const std::string&	read_output();
		std::string	run(void);

		// to be deleted
		void	print_env(void);
};

#endif
