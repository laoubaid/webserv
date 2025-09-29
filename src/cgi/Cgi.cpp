/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/01 18:59:31 by kez-zoub          #+#    #+#             */
/*   Updated: 2025/09/29 09:56:25 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cgi.hpp"
#include "../config/locationConf.hpp"

Cgi::Cgi(Request *req, std::string interp, sockaddr_in clt_addr) : _req(req), argv(NULL), env(NULL), _state(IDLE), _clt_addr(clt_addr)
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
	if (_out_file.is_open())
		_out_file.close();
	if (_in_file.is_open())
		_in_file.close();
	if (_write_in != -1)
		close(_write_in);
	if (_write_in != -1)
		close(_read_from);

	std::cout << "[INFO] CGI removing the tmp file for output \n";
	std::remove(_out_file_path.c_str());
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


std::string toupperstring(const std::string& input) {
	std::string res;

	for (size_t i = 0; i < input.size(); ++i) {
		res += toupper(input.at(i));
		if (res[i] == '-')
			res[i] = '_';
	}
	return res;
}

void	Cgi::set_env(void)
{
	std::cout << "setting up env vars\n";
	std::vector<std::string>	env_vec;

	// values that are hardcoded and needs to be changed

	char ip_str[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &_clt_addr.sin_addr, ip_str, sizeof(ip_str));
	int port = ntohs(_clt_addr.sin_port);

	std::string	client_ip(ip_str);
	std::string	server_port = std::to_string(port);

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
	env_vec.push_back(std::string("REDIRECT_STATUS=200"));
	env_vec.push_back(std::string("SCRIPT_NAME=") + script_name);
	env_vec.push_back(std::string("SCRIPT_FILENAME=") + _req->get_location().get_root() + script_name);
	env_vec.push_back(std::string("SERVER_NAME=") + get_server_name(*_req));
	env_vec.push_back(std::string("SERVER_PORT=") + server_port);
	env_vec.push_back(std::string("SERVER_PROTOCOL=HTTP/1.1"));
	env_vec.push_back(std::string("SERVER_SOFTWARE=webserv/1.0.0"));

	std::map<std::string, Uvec>::const_iterator it;
	for (it = _req->getFields().begin(); it != _req->getFields().end(); ++it) {
		std::string tmp = "HTTP_" + toupperstring(it->first);
		if (tmp == "HTTP_CONTENT_TYPE" || tmp == "HTTP_CONTENT_TYPE")
			continue;
		// if (tmp == "HTTP_COOKIE")  //* testing cookies
			// continue;
		tmp += "=" + std::string(it->second.begin(), it->second.end());
		env_vec.push_back(tmp);
	}

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
	_req->setReqState(CGI);

	std::cout << "[INFO] -- RUNNING CGI --" << std::endl;

	// should throw exception in case the file doesn't exist or can't be run
	std::string file = root + script_name;
	if (!fileExists(file.c_str()))
		cgi_err("[CGI ERROR] script doesn't exist", 404, RESP);
	
	// int	status;
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
		close(STDERR_FILENO);																			//* delete this so errors of cgi get printed!
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

	if (_req->getBodySize()) {
		_state = WRITE;
		_in_file.open(_req->getBodyFilePath().c_str(), std::ios::binary);
		
		// _write_in to epoll
		struct epoll_event clt_event;
		memset(&clt_event, 0, sizeof(clt_event));
		clt_event.data.fd = _write_in;
		clt_event.events = EPOLLOUT;																	//* must check for both in and out at same time (subject requirements).
		epoll_ctl(_req->getEpollFd(), EPOLL_CTL_ADD, _write_in, &clt_event);
		std::cout << CONN_CLR <<"\n$ New Pipe created! fd: " << _write_in << DEF_CLR << std::endl;
	} else {
		_state = READ;

		// _read_from to epoll
		epoll_ctl(_req->getEpollFd(), EPOLL_CTL_DEL, _write_in, NULL);									//* deleting the write_in pipe end from epoll
		struct epoll_event clt_event;
		memset(&clt_event, 0, sizeof(clt_event));
		clt_event.data.fd = _read_from;
		clt_event.events = EPOLLIN;																		//* must check for both in and out at same time (subject requirements).
		epoll_ctl(_req->getEpollFd(), EPOLL_CTL_ADD, _read_from, &clt_event);
		std::cout << CONN_CLR <<"\n$ New Pipe created! fd: " << _write_in << DEF_CLR << std::endl;

		{
			std::ostringstream oss;
			oss << "/tmp/outfile_" << this;
			_out_file_path = oss.str();
		}
		_out_file.open(_out_file_path, std::ios::binary | std::ios::app);
	}
	return "";
}

int		Cgi::get_pipe(int flag) {
		if (flag == 0)
			return _write_in;
		return _read_from;
}

int	Cgi::write_body()
{
	std::cout << "[INFO] CGI write body in pipe\n";
	if (_req->getBodySize())
	{
		if (!_in_file.is_open())
			cgi_err("[CGI ERROR] file not open while writing body", 403, RESP);
		std::cout << "[INFO] CGI body found\n";
		
		std::vector<unsigned char>	buffer(CGI_BUFFER);
		if (_in_file.read(reinterpret_cast<char*>(&buffer[0]), buffer.size()) || _in_file.gcount() > 0)
		{
			std::cout << "[INFO] CGI writing this buffer: ";
			for (std::vector<unsigned char>::iterator it = buffer.begin(); it < buffer.end(); it++) {
				std::cout << *it;
			}
			std::cout << std::endl;
			
			std::size_t	char_read = _in_file.gcount();
			if (char_read > 0)
			{
				if (write(_write_in, buffer.data(), char_read) == -1)
					cgi_err("[CGI ERROR] script terminated unexpectedly", 502, RESP);
			}
		}
		if (_in_file.eof())
		{
			_state = READ;
			epoll_ctl(_req->getEpollFd(), EPOLL_CTL_DEL, _write_in, NULL);								//* deleting the write_in pipe end from epoll
			struct epoll_event clt_event;
			memset(&clt_event, 0, sizeof(clt_event));
			clt_event.data.fd = _read_from;
			clt_event.events = EPOLLIN;																	//* must check for both in and out at same time (subject requirements).
			epoll_ctl(_req->getEpollFd(), EPOLL_CTL_ADD, _read_from, &clt_event);
			std::cout << CONN_CLR <<"\n$ New Pipe created! fd: " << _write_in << DEF_CLR << std::endl;
			close(_write_in);
			_write_in = -1;
			std::cout << "[INFO] CGI file ends switching pipes in epoll\n";

			{
				std::ostringstream oss;
				oss << "/tmp/outfile_" << this;
				_out_file_path = oss.str();
			}
			_out_file.open(_out_file_path, std::ios::binary | std::ios::app);

			return 1;
		}
		return 0;
	}
	return 1;
}

int	Cgi::read_output()
{
	std::cout << "[INFO] CGI read output from pipe : " << _out_file_path << std::endl;
	char	buff[CGI_BUFFER];

	if (!_out_file.is_open())
		cgi_err("[CGI ERROR] file not open while reading output", 500, RESP);
	ssize_t char_read = read(_read_from, buff, CGI_BUFFER);
	if (char_read == -1)
		cgi_err("[CGI ERROR] error during reading from pipe", 502, RESP);
	if (char_read == 0)
	{
		std::cout << "[INFO] CGI scripts ended!\n";
		_state = END;
		epoll_ctl(_req->getEpollFd(), EPOLL_CTL_DEL, _read_from, NULL);									//* deleting the read_from pipe end from epoll
		close(_read_from);
		_read_from = -1;
		_out_file.close();
		return 1;
	}
	// buff read should be sent to client unless when 'status' is changed for response first line 'HTTP/1.1 200 OK' for exmple
	std::string str(buff, char_read); // build directly from buffer and length
	// std::cout << "\n-\n" << str << "\n-"<< std::endl;
	_out_file.write(str.c_str(), char_read);
	return 0;
}

bool Cgi::check_process_status() {
    if (child_pid <= 0) {
        return false; // No valid process
    }
    
    int status;
    pid_t result = waitpid(child_pid, &status, WNOHANG);
	std::cout << "[INFO] CGI calling waitpid and gets: " << result << std::endl;
    
    if (result == 0) {
        // Process is still running
        return false;
    } else if (result == child_pid) {
        // Process has terminated
        if (WIFEXITED(status)) {
            exit_status_ = WEXITSTATUS(status);
            std::cout << "[INFO] CGI process exited with status: " << exit_status_ << std::endl;
        } else if (WIFSIGNALED(status)) {
            int signal = WTERMSIG(status);
            exit_status_ = -signal; // Negative to indicate signal termination
            std::cout << "[INFO] CGI process terminated by signal: " << signal << std::endl;
        }
        _state = END;
        return true;
    } else {
        _state = END;
        exit_status_ = -1;
        return true;
    }
}

int Cgi::get_cgi_exit_status() {
	return exit_status_;
}

int Cgi::get_cgi_pid() {
	return child_pid;
}

std::string Cgi::get_outfile_path() {
	return _out_file_path;
}

// to be deleted
void	Cgi::print_env(void)
{
	std::cout << "[INFO] CGI printing env...\n";
	for (std::size_t i = 0; env[i]; i++)
	{
		std::cout << env[i] << std::endl;
	}
	std::cout << "\n\n\n";
}
