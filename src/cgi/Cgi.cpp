/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kez-zoub <kez-zoub@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/01 18:59:31 by kez-zoub          #+#    #+#             */
/*   Updated: 2025/07/28 12:57:32 by kez-zoub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cgi.hpp"

Cgi::Cgi(void)
{
}

Cgi::~Cgi(void)
{
	// std::cout << "cgi destructor called\n";
	for (std::size_t i = 0; env[i]; i++)
	{
		// std::cout << "deleting " << env[i] << std::endl;
		delete[] env[i];
	}
	delete[] env;

	for (std::size_t i = 0; argv[i]; i++)
	{
		// std::cout << "deleting " << env[i] << std::endl;
		delete[] argv[i];
	}
	delete[] argv;
}

std::string	get_auth_type(const HTTPRequestParser& req)
{
	Uvec	auth_value;
	if (req.getFieldValue("authorization", auth_value))
	{
		Uvec::iterator	it = auth_value.begin();

		for (Uvec::iterator it = auth_value.begin(); it != auth_value.end(); it++)
		{
			if (*it == ' ')
				return (std::string(auth_value.begin(), it));
		}
		return (std::string(""));
	}
	return (std::string(""));
}

void	set_content_type(const HTTPRequestParser& req, std::vector<std::string>& env_vec)
{
	Uvec	content_type_value;
	if (req.getFieldValue("content-type", content_type_value))
		env_vec.push_back(std::string("CONTENT_TYPE=") + std::string(content_type_value.begin(), content_type_value.end()));
}

std::string	get_path_info(const std::string& target)
{
	std::string::const_iterator	start = target.begin();

	while (start != target.end() && *start != '.')
		start++;
	while (start != target.end() && *start != '/')
		start++;
	
	std::string::const_iterator	end = start;
	while (end != target.end() && *end != '?')
		end++;
	return (decode_url(std::string(start, end)));
}

std::string	get_query_string(const std::string& target)
{
	std::string::const_iterator	it = target.begin();

	while (it != target.end() && *it != '?')
		it++;
	if (it != target.end() && *it == '?')
		it++;
	return (std::string(it, target.end()));
}

std::string	get_script_name(const std::string& target)
{
	std::string::const_iterator	it = target.begin();

	while (it != target.end() && *it != '.')
		it++;
	while (it != target.end() && *it != '/')
		it++;
	return (decode_url(std::string(target.begin(), it)));
}

std::string	get_server_name(const HTTPRequestParser& req)
{
	Uvec	host;
	if (req.getFieldValue("host", host))
	{
		Uvec::iterator	it = host.begin();
		while (it != host.end() && *it != ':')
			it++;
		return (std::string(host.begin(), it));
	}
	return (std::string(""));
}

void	Cgi::set_env(const HTTPRequestParser& req)
{
	std::cout << "setting up env vars\n";
	const char	*methods[3] = {"GET", "POST", "DELETE"};
	std::vector<std::string>	env_vec;
	
	env_vec.push_back(std::string("AUTH_TYPE=") + get_auth_type(req));
	if (req.getBody().size())
		env_vec.push_back(std::string("CONTENT_LENGTH=") + ft_itos(req.getBody().size()));
	set_content_type(req, env_vec);
	env_vec.push_back(std::string("GATEWAY_INTERFACE=CGI/1.1"));
	std::string	path_info = get_path_info(req.getTarget());
	env_vec.push_back(std::string("PATH_INFO=") + path_info);
	if (path_info.size())
		env_vec.push_back(std::string("PATH_TRANSLATED=") + "/usr/local/www/for/example" + path_info); // get physical location of path info (maybe from config file)
	env_vec.push_back(std::string("QUERY_STRING=") + get_query_string(req.getTarget()));
	env_vec.push_back(std::string("REMOTE_ADDR=") + "42.13.37.42"); // replace with client ip address
	env_vec.push_back(std::string("REMOTE_HOST=") + "42.13.37.42"); // replace with client ip address
	env_vec.push_back(std::string("REQUEST_METHOD=") + methods[req.getMethod()]);
	env_vec.push_back(std::string("SCRIPT_NAME=") + get_script_name(req.getTarget()));
	env_vec.push_back(std::string("SERVER_NAME=") + get_server_name(req));
	env_vec.push_back(std::string("SERVER_PORT=") + "8080"); // get post from config file
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

void	Cgi::set_argv(const std::string& target)
{
	std::string	cgi_root_location = "/home/kamal/projects/42cursus-webserv/www"; // should be replace with cgi-bin location in config file
	std::string	script_path = cgi_root_location + get_script_name(target);

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

std::string	Cgi::run_cgi(const HTTPRequestParser& req)
{
	int	status;
	int fd[2];
	set_env(req);
	set_argv(req.getTarget());
	if (pipe(fd) == -1)
		exit(1); // change to proper
	int	pid = fork();
	if (pid == -1)
		exit(1); // change to proper error handler
	if (pid == 0)
	{
		dup2(fd[1], 1);
		close(fd[0]);
		execve(argv[0], argv, env);
		exit(1); // change to proper error handler
	}
	close(fd[1]);
	fd;

	waitpid(pid, &status, 0); // dont wait for child process but avoid zombies
	// print_env();
	int	char_read;
	char	buff[1024];
	std::string	res;
	while ((char_read = read(fd[0], buff, 1024)) > 0) // fd[0] is the file decriptor to be monitored in epoll
	{
		res.append(buff, char_read);
	}
	if (char_read == -1)
		exit(1); // proper error
	close(fd[0]);
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
