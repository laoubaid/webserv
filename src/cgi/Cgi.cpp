/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kez-zoub <kez-zoub@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/01 18:59:31 by kez-zoub          #+#    #+#             */
/*   Updated: 2025/09/08 14:21:14 by kez-zoub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cgi.hpp"

Cgi::Cgi(void) : argv(NULL), env(NULL)
{
	root = "/var/www";
	cgi_dir = "/cgi-bin";
}

Cgi::~Cgi(void)
{
	// std::cout << "cgi destructor called\n";
	for (std::size_t i = 0; env && env[i]; i++)
	{
		// std::cout << "deleting " << env[i] << std::endl;
		delete[] env[i];
	}
	delete[] env;

	for (std::size_t i = 0; argv && argv[i]; i++)
	{
		// std::cout << "deleting " << env[i] << std::endl;
		delete[] argv[i];
	}
	delete[] argv;
}

std::string	get_auth_type(const Request& req)
{
	Uvec	auth_value;
	if (req.getFieldValue("authorization", auth_value))
	{
		Uvec::iterator	it = auth_value.find(' ');
		if(it != auth_value.end())
			return (std::string(auth_value.begin(), it));
		return (std::string(""));
	}
	return (std::string(""));
}

void	set_content_type(const Request& req, std::vector<std::string>& env_vec)
{
	Uvec	content_type_value;
	if (req.getFieldValue("content-type", content_type_value))
		env_vec.push_back(std::string("CONTENT_TYPE=") + std::string(content_type_value.begin(), content_type_value.end()));
}

std::string	get_server_name(const Request& req)
{
	Uvec	host;
	if (req.getFieldValue("host", host))
	{
		Uvec::iterator	it = host.find(':');
		return (std::string(host.begin(), it));
	}
	return (std::string(""));
}

void	Cgi::set_script_name(const std::string &name)
{
	script_name = name;
}

void	Cgi::set_path_info(const std::string &path)
{
	path_info = path;
}

void	Cgi::set_env(const Request& req)
{
	std::cout << "setting up env vars\n";
	std::vector<std::string>	env_vec;

	// values that are hardcoded and needs to be changed
	std::string	DocumentRoot = "/var/www/html"; // The exact mapping depends on the server’s configuration (DocumentRoot, Alias, ScriptAlias, etc).
	std::string	client_ip = "42.13.37.42"; // replace with client ip address
	std::string	server_port = "8080"; // get port from config file

	env_vec.push_back(std::string("AUTH_TYPE=") + get_auth_type(req));
	if (req.getBodySize())
		env_vec.push_back(std::string("CONTENT_LENGTH=") + ft_itos(req.getBodySize()));
	set_content_type(req, env_vec);
	env_vec.push_back(std::string("GATEWAY_INTERFACE=CGI/1.1"));
	env_vec.push_back(std::string("PATH_INFO=") + path_info);
	if (path_info.size())
		env_vec.push_back(std::string("PATH_TRANSLATED=") + DocumentRoot + path_info);
	env_vec.push_back(std::string("QUERY_STRING=") + req.getTarget().query);
	env_vec.push_back(std::string("REMOTE_ADDR=") + client_ip);
	env_vec.push_back(std::string("REMOTE_HOST=") + client_ip);
	std::string	method;
	switch (req.getMethod())
	{
		case DELETE:
			method = "DELETE";
			break;
		case POST:
			method = "POST";
			break;
		case GET:
			method = "GET";
			break;
	}
	env_vec.push_back(std::string("REQUEST_METHOD=") + method);
	env_vec.push_back(std::string("SCRIPT_NAME=") + script_name);
	env_vec.push_back(std::string("SERVER_NAME=") + get_server_name(req));
	env_vec.push_back(std::string("SERVER_PORT=") + server_port);
	env_vec.push_back(std::string("SERVER_PROTOCOL=HTTP/1.1"));
	env_vec.push_back(std::string("SERVER_SOFTWARE=webserv/1.0.0"));

	// turn this vector to array of strings in the heap
	try
	{
		env = new char*[env_vec.size() +1];
		for (std::size_t i = 0; i < env_vec.size(); i++)
		{
			env[i] = ft_strdup(env_vec[i]);
		}
		env[env_vec.size()] = NULL;
	}
	catch(const std::exception& e)
	{
		// error during malloc
		// set up approprieted error handling
	}
	
}

void	Cgi::set_argv(void)
{
	// std::string	cgi_root_location = "/home/kamal/projects/42cursus-webserv/www"; // should be replace with cgi-bin location in config file
	std::string	script_path = root + script_name;

	try
	{
		if (has_shebang(script_path))
		{
			argv = new char*[2];
			argv[0] = ft_strdup(script_path);
			argv[1] = NULL;
		}
		else
		{
			std::string	ext = get_ext(script_path);
			std::map<std::string, std::string>	interp = {
				{"cgi", "/usr/bin/python3"}, // not rfc specified, maybe ask for it in config file
				{"py", "/usr/bin/python3"},
				{"php", "/usr/bin/php"}
			};
			argv = new char*[3];
			argv[0] = ft_strdup(interp[ext]);
			argv[1] = ft_strdup(script_path);
			argv[2] = NULL;
		}
	}
	catch(const std::exception& e)
	{
		// error during malloc
		// set up approprieted error handling
	}
}

std::string	Cgi::run(const Request& req)
{
	// in the beginning of the server signal should be set
	signal(SIGPIPE, SIG_IGN);        // chang place

	// should throw exception in case the file doesn't exist or can't be run
	
	int	status;
	int pipe_in[2];
	int pipe_out[2];
	set_env(req);
	set_argv();
	if (pipe(pipe_in) == -1 || pipe(pipe_out) == -1)
		exit(1); // change to proper
	int	pid = fork();
	if (pid == -1)
		exit(1); // change to proper error handler
	if (pid == 0)
	{
		std::string	execution_dir = root + cgi_dir;
		if (chdir(execution_dir.c_str()) == -1)
			exit (1); // invalid path
		dup2(pipe_out[1], 1);
		dup2(pipe_in[0], 0);
		close(pipe_out[0]);
		close(pipe_in[1]);
		execve(argv[0], argv, env);
		exit(1); // change to proper error handler
	}
	child_pid = pid;
	close(pipe_in[0]);
	close(pipe_out[1]);
	// sleep(1);                      // ??????????????
	// past body to cgi
	if (req.getBodySize())
	{
		std::cout << "body found\n";
		std::ifstream	file(req.getBodyFilePath().c_str(), std::ios::binary);
		if (!file)
			return (""); // file not found
		std::cout << "file opened\n";
		std::vector<unsigned char>	buffer(1024);
		while (file.read(reinterpret_cast<char*>(&buffer[0]), buffer.size()) || file.gcount() > 0)
		{
			std::cout << "writing this buffer: ";
			for (std::vector<unsigned char>::iterator it = buffer.begin(); it < buffer.end(); it++)
			{
				std::cout << *it;
			}
			std::cout << std::endl;
			std::size_t	char_read = file.gcount();
			if (char_read > 0)
			{
				if (write(pipe_in[1], buffer.data(), char_read) == -1)
				{
					std::cout << "ERROR: pipe writing failed\n";
					return (""); // write failed (child can't accept date maybe abnormally terminated)
				}
			}
		}
		
		file.close();
	}
	close(pipe_in[1]);

	while (waitpid(child_pid, &status, WNOHANG) <= 0)
	{
		sleep(1);
	}
	

	// waitpid(child_pid, &status, WNOHANG); // dont wait for child process but avoid zombies
	// print_env();
	int	char_read;
	char	buff[1024];
	std::string	res;
	while ((char_read = read(pipe_out[0], buff, 1024)) > 0) // fd[0] is the file decriptor to be monitored in epoll
	{
		res.append(buff, char_read);
	}
	if (char_read == -1)
		exit(1); // proper error
	close(pipe_out[0]);

	std::cout << "-----------------cgi--------------\n" << "cgi execution finished! cgi output:\n" << res << "------------------end-----------------\n";
	return (res);
}

// to be deleted
void	Cgi::print_env(void)
{
	std::cout << "printing env...\n";
	for (std::size_t i = 0; env[i]; i++)
	{
		std::cout << env[i] << std::endl;
	}
	std::cout << "\n\n\n";
}
