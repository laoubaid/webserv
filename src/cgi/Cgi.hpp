/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/01 18:59:34 by kez-zoub          #+#    #+#             */
/*   Updated: 2025/11/01 03:06:16 by laoubaid         ###   ########.fr       */
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
		Request*		_req;
		char			**argv;
		char			**env;
		std::string		root;
		std::string		_interpreter;
		std::string		script_name;
		std::string		path_info;
		Uvec			body;
		t_cgi_state		_state;
		int				child_pid;

		int				exit_status_;

		int				_write_in;
		int				_read_from;
		std::ifstream	_in_file;

		std::ofstream	_out_file;
		std::string		_out_file_path;

		sockaddr_in		_clt_addr;

		void			set_argv(void);
		void			set_env(void);
		void			cgi_err(std::string throw_msg, int status_code, t_req_state req_state);
	
	public:
		Cgi(Request *req, std::string interp, sockaddr_in clt_addr);
		~Cgi(void);

		int		get_pipe(int flag);

		void	set_script_name(const std::string &name);
		void	set_path_info(const std::string &path);

		int		get_cgi_state() { return _state; }
		int		write_body();
		int		read_output();

		bool	check_process_status();
		int		get_cgi_exit_status();
		int		get_cgi_pid();

		std::string get_outfile_path();

		std::string	run(void);
};

#endif
