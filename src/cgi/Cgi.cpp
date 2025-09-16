/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kez-zoub <kez-zoub@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/01 18:59:31 by kez-zoub          #+#    #+#             */
/*   Updated: 2025/09/16 18:18:28 by kez-zoub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cgi.hpp"
#include "../config/locationConf.hpp"

Cgi::Cgi(Request *req, std::string interp) : _req(req), argv(NULL), env(NULL), _state(WRITE)
{
	_interpreter = interp;
	root = _req->get_location().get_root();
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
	close(_write_in);
	close(_read_from);
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

void	Cgi::set_env(void)
{
	std::cout << "setting up env vars\n";
	std::vector<std::string>	env_vec;

	// values that are hardcoded and needs to be changed
	std::string	client_ip = "42.13.37.42"; //! replace with client ip address
	std::string	server_port = "8080"; //! get port from config file

	env_vec.push_back(std::string("AUTH_TYPE=") + get_auth_type(*_req));
	if (_req->getBodySize())
		env_vec.push_back(std::string("CONTENT_LENGTH=") + ft_itos(_req->getBodySize()));
	set_content_type(*_req, env_vec);
	env_vec.push_back(std::string("GATEWAY_INTERFACE=CGI/1.1"));
	env_vec.push_back(std::string("PATH_INFO=") + path_info);
	if (path_info.size())
		env_vec.push_back(std::string("PATH_TRANSLATED=") + root + path_info);
	env_vec.push_back(std::string("QUERY_STRING=") + _req->getTarget().query);
	env_vec.push_back(std::string("REMOTE_ADDR=") + client_ip);
	env_vec.push_back(std::string("REMOTE_HOST=") + client_ip);
	std::string	method;
	switch (_req->getMethod())
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
	env_vec.push_back(std::string("SERVER_NAME=") + get_server_name(*_req));
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
		cgi_err("[CGI ERROR] memory allocation failed", 500, RESP);
	}
}

void	Cgi::set_argv(void)
{
	// std::string	cgi_root_location = "/home/kamal/projects/42cursus-webserv/www"; // should be replace with cgi-bin location in config file
	std::string	script_path = root + script_name;

	try
	{
		argv = new char*[3];
		
		argv[0] = ft_strdup(_interpreter);
		argv[1] = ft_strdup(script_path);
		argv[2] = NULL;
	}
	catch(const std::exception& e)
	{
		cgi_err("[CGI ERROR] memory allocation failed", 500, RESP);
	}
}

void	Cgi::cgi_err(std::string throw_msg, int status_code, t_req_state req_state)
{
	_req->setParsingCode(status_code);
	_req->setReqState(req_state);
	throw std::runtime_error(throw_msg);
}

std::string	Cgi::run(void)
{
	// in the beginning of the server signal should be set
	signal(SIGPIPE, SIG_IGN);        //! change place
	/////////////////////////////////////////////////////

	// should throw exception in case the file doesn't exist or can't be run
	std::string file = root + script_name;
	if (!fileExists(file.c_str()))
		cgi_err("[CGI ERROR] script doesn't exist", 404, RESP);
	
	int	status;
	int pipe_in[2];
	int pipe_out[2];
	set_env();
	set_argv();
	if (pipe(pipe_in) == -1 || pipe(pipe_out) == -1)
		cgi_err("[CGI ERROR] pipe creation failed", 500, RESP);
	int	pid = fork();
	if (pid == -1)
		cgi_err("[CGI ERROR] fork failed", 500, RESP);
	if (pid == 0)
	{
		std::string	execution_dir = root + _req->get_location().get_path();
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
	_write_in = pipe_in[1];
	_read_from = pipe_out[0];
	close(pipe_in[0]);
	close(pipe_out[1]);

	if (_req->getBodySize())
		_file.open(_req->getBodyFilePath().c_str(), std::ios::binary);
	// _write_in to epoll
	// epoll_ctl(_write_in)
	// you need to write the body to the child process
	// then write the output of the child process
	// clean up using waitpid

	// sleep(1);                      // ??????????????
	// past body to cgi
	

	while (waitpid(child_pid, &status, WNOHANG) <= 0)
	{
		sleep(1);
	}
	
	if (WIFEXITED(status)) {
		printf("Child exited normally with status %d\n", WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
		printf("Child terminated by signal %d\n", WTERMSIG(status));
    }

	// waitpid(child_pid, &status, WNOHANG); // dont wait for child process but avoid zombies
	// print_env();
	
}

void	Cgi::write_body()
{
	if (_req->getBodySize())
	{
		if (!_file.is_open())
			cgi_err("[CGI ERROR] file not open", 403, RESP);
		std::cout << "body found\n";
		
		std::vector<unsigned char>	buffer(CGI_BUFFER);
		if (_file.read(reinterpret_cast<char*>(&buffer[0]), buffer.size()) || _file.gcount() > 0)
		{
			// std::cout << "writing this buffer: ";
			// for (std::vector<unsigned char>::iterator it = buffer.begin(); it < buffer.end(); it++)
			// {
			// 	std::cout << *it;
			// }
			// std::cout << std::endl;
			std::size_t	char_read = _file.gcount();
			if (char_read > 0)
			{
				if (write(_write_in, buffer.data(), char_read) == -1)
					cgi_err("[CGI ERROR] script terminated unexpectedly", 502, RESP);
			}
		}
		if (_file.eof())
		{
			_state = READ;
			// remove _write_in from epoll and add _read_from instead
		}
	}
}

const std::string&	Cgi::read_output()
{
	int	char_read;
	std::string	str;
	char	buff[CGI_BUFFER];
	if ((char_read = read(_read_from, buff, CGI_BUFFER)) > 0) // fd[0] is the file decriptor to be monitored in epoll
	{
		// buff read should be sent to client unless when 'status' is changed for response first line 'HTTP/1.1 200 OK' for exmple
		str = buff;
	}
	if (char_read == -1)
		cgi_err("[CGI ERROR] error during reading from pipe", 502, RESP);
	if (char_read == 0)
	{
		_state = DONE;
		// remove _read_from from epoll and call waitpid???
	}
	return (str);
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
