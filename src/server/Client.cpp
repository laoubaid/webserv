/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/16 16:27:54 by laoubaid          #+#    #+#             */
/*   Updated: 2025/09/23 16:25:51 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client(int clt_fd, const serverConf& conf, int ep_fd, sockaddr_in clt_adr) : Socket(clt_fd), conf_(conf), client_addr_(clt_adr)
{
	// std::cout << "Client constracteur called!" << std::endl;
	epoll_fd_ = ep_fd;
	request_ = NULL;
	response_ = NULL;
	cgi_ = NULL;
	timeout_= std::time(NULL);
}

Client::~Client()
{
	// std::cout << "Client destructor called!" << std::endl;
	if (cgi_ && cgi_->get_cgi_pid() > 0) {
        // Kill the CGI process if client disconnected
        kill(cgi_->get_cgi_pid(), SIGTERM);
        
        // Wait a bit, then force kill if needed
        sleep(1);
        if (!cgi_->is_process_finished()) {
            kill(cgi_->get_cgi_pid(), SIGKILL);
        }
    }
	delete request_;
	delete response_;
}

int Client::get_fd_client() {
	return get_fd();
}

sockaddr_in Client::get_client_addr() {
	return client_addr_;
}

bool Client::check_timeout() {
	std::time_t now = std::time(NULL);

	if (static_cast<size_t>(now - timeout_) > conf_.get_client_timeout())
		return true;
	return false;
}

int Client::receive(int epoll_fd) {
	// std::cout << "|\treceive request" << std::endl;
	int client_fd = this->get_fd();
	unsigned char buf[RECV_BUF];

	int nread = recv(client_fd, buf, RECV_BUF - 1, 0);
	
	if (nread <= 0) {
		if (nread == 0)
			std::cout << "Client disconnected! (recv() == 0)" << std::endl;
		else
			perror("recv() failed");
		epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
		std::cout << DISC_CLR << "\n$ Client disconnected! (epoll IN) fd: " << client_fd << DEF_CLR << std::endl;
		return -1;
	}
	reset_timeout();

	Uvec tmp_vec_buf(buf, nread); // Convert the buffer to Uvec
	Uvec delimiter((const unsigned char*)"\r\n\r\n", 4);
	vec_buf_ += tmp_vec_buf;
	if (!(request_ && request_->getReqState() == PEND)) {
		if (vec_buf_.size() < RECV_BUF && vec_buf_.find(delimiter) == vec_buf_.end())
			return 0;
	}
	process_recv_data();
	return (request_) ? request_->getReqState() : -1;
}

int Client::process_recv_data() {
	// std::cout << "|\tprocess_recv_data called!" << std::endl;
	if (!request_) {
		try {
			request_ = new Request(conf_, get_fd(), epoll_fd_, client_addr_);
			request_->ParseRequest(vec_buf_);
			if (request_->getReqState() == PEND)
				vec_buf_.clear();

		} catch (const std::exception &e) {
			std::string error = e.what();
			std::cerr << "Error creating HTTPRequestParser: " << error << std::endl;
			if (error.find("CGI") != std::string::npos)
				std::cout << "V A L I D\n";
			// return -1; //TODO Handle error appropriately
		}
	}
	else if (request_ && request_->getReqState() == PEND) {
		try {
			request_->addBody(vec_buf_);
			vec_buf_.clear();
		} catch (const std::exception &e) {
			std::cerr << "Error adding to body: " << e.what() << std::endl;
			// return -1; //TODO Handle error appropriately
		}
	}
	if (request_ && request_->getReqState() == CGI) {
		cgi_ = request_->getCgiObject();
	}
	if (request_ && request_->getReqState() == RESP) {
		std::cout << "=========================================\n";
		response_ = new HttpResponse(*request_, conf_);
		// log();
	}
	return (request_) ? request_->getReqState() : -1;
}

int Client::send_response() {
	// std::cout << "|\t|\tSend response: " << this->get_fd() << std::endl;

	resbuf_.clear();
	resbuf_ = response_->generateResponse();
	if (send(this->get_fd(), resbuf_.c_str(), resbuf_.size(), MSG_NOSIGNAL) == -1)
		return 1;
	std::cout << "[INFO] CLT data sent seccuessfuly!" << std::endl;
	// std::cout << "\n<>\n" << resbuf_ << "\n</>\n" << std::endl;
	resbuf_.clear();
	reset_timeout();
	if (response_ && response_->getRespState() == DONE) {
		std::cout << "[INFO] CLT end of connection" << std::endl;
		delete request_;
		request_ = NULL;
		delete response_;
		response_ = NULL;
		vec_buf_.clear();

		return 1;
	}
	return 0;
}

void Client::set_cgi_obj(std::map <int, Client*> &cgi_pipes, int flag) {
	int pipfd;

	if (request_){
		std::cout << "[INFO] CLT Set CGI object! " << std::endl;
		for (int i = 0; i != 3; ++i) {
			pipfd = request_->get_cgi_pipe(i);
			if (pipfd != -1){
				if (cgi_pipes.find(pipfd) == cgi_pipes.end() && flag) {
					std::cout << "[INFO] CLT the object has been Added cgi_fd: " << pipfd << std::endl;
					cgi_pipes[pipfd] = this;
				} else if (cgi_pipes.find(pipfd) != cgi_pipes.end() && !flag) {
					std::cout << "[INFO] CLT the object has been Removed cgi_fd: " << pipfd << std::endl;
					cgi_pipes.erase(pipfd);
				}
			}
		}
	}
}

int Client::cgi_pipe_io(int pipe_fd) {
	std::cout << "[INFO] CLT cgi pipe I/O operation\n";
	if (cgi_) {
		int cgi_state = cgi_->get_cgi_state();

		if (cgi_->check_process_status()) {
			response_ = new HttpResponse(*request_, conf_);
			request_->setReqState(RESP);
			request_->setParsingCode((cgi_->get_cgi_exit_status() == 0) ? 200 : 502);
			return 1;
		}
		if (cgi_state == END) {
			response_ = new HttpResponse(*request_, conf_);
			request_->setReqState(RESP);
			request_->setParsingCode((cgi_->get_cgi_exit_status() == 0) ? 200 : 502);
			return 1;
		}
		if (pipe_fd == cgi_->get_pipe(0)) {
			return cgi_->write_body();
		} else {
			if (cgi_->read_output() && response_ == NULL) {
				response_ = new HttpResponse(*request_, conf_);
				request_->setReqState(RESP);
				request_->setParsingCode((cgi_->get_cgi_exit_status() == 0) ? 200 : 502);
				return 1;
			}
		}
		return 0;
	}
	response_ = new HttpResponse(*request_, conf_);
	request_->setReqState(RESP);
	request_->setParsingCode((cgi_->get_cgi_exit_status() == 0) ? 200 : 502);
	return 1;
}

#include <chrono>
#include <iomanip>
#include <ctime>

void Client::log() const {
	// Colors
	const char* GREEN  = "\033[32m";
	const char* RED    = "\033[31m";
	const char* YELLOW = "\033[33m";
	const char* CYAN   = "\033[36m";
	const char* RESET  = "\033[0m";

	// Get current time
	auto now = std::chrono::system_clock::now();
	std::time_t now_time = std::chrono::system_clock::to_time_t(now);
	std::tm tm = *std::localtime(&now_time);
	std::ostringstream time_stream;
	time_stream << std::put_time(&tm, "[%Y-%m-%d %H:%M:%S]");

	if (request_) {
		const t_method& method = request_->getMethod();
		int status = request_->getParsingCode();

		// Pick method color
		std::string method_str;
		switch (method) {
			case GET: method_str = std::string(CYAN) + "GET" + RESET; break;
			case POST: method_str = std::string(YELLOW) + "POST" + RESET; break;
			case DELETE: method_str = std::string(RED) + "DELETE" + RESET; break;
			default: method_str = "UNKNOWN"; break;
		}

		// Pick status color
		std::string status_str;
		if (status >= 200 && status < 300)
			status_str = std::string(GREEN) + std::to_string(status) + RESET;
		else if (status >= 300 && status < 400)
			status_str = std::string(CYAN) + std::to_string(status) + RESET;
		else if (status >= 400 && status < 500)
			status_str = std::string(YELLOW) + std::to_string(status) + RESET;
		else if (status >= 500)
			status_str = std::string(RED) + std::to_string(status) + RESET;
		else
			status_str = std::to_string(status);

		// Print log
		std::cout << time_stream.str()
				  << " [fd " << this->get_fd() << "] "
				  << method_str << " "
				  << status_str << " "
				  << request_->getTarget().path
				  << std::endl;
	}
}

void Client::print_whatever(std::string whatever) {
	std::cout << whatever << std::endl;
	std::cout << YLW_CLR << ">\tthe client socket FD: " << this->get_fd() << DEF_CLR << std::endl;
	std::cout << "---------------------------------------------------------------------------------------" << std::endl;
	std::cout << "-------------------------------- THE START OF whatever! --------------------------------" << std::endl;
	std::cout << vec_buf_;
	std::cout << "--------------------------------- THE END OF whatever! ---------------------------------" << std::endl;
	std::cout << "size of whatever: " << vec_buf_.size() << std::endl;
	std::cout << "---------------------------------------------------------------------------------------" << std::endl;
	std::cout << "\n\n" <<std::endl;
}