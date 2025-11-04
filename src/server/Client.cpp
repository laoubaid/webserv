/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/16 16:27:54 by laoubaid          #+#    #+#             */
/*   Updated: 2025/11/01 02:59:58 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client(int clt_fd, const serverConf& conf, int ep_fd, sockaddr_in clt_adr) : Socket(clt_fd), conf_(conf), client_addr_(clt_adr)
{
	// std::cout << "[INFO] CLT Client constracteur called!" << std::endl;
	epoll_fd_ = ep_fd;
	request_ = NULL;
	response_ = NULL;
	cgi_ = NULL;
	req_timeout_= std::time(NULL);
	state_timout_ = 0;
}

Client::~Client()
{
	// std::cout << "[INFO] CLT Client destructor called!" << std::endl;
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
	int status_code = 0;

	if (state_timout_ == 0 && static_cast<size_t>(now - req_timeout_) > conf_.get_recv_timeout()) {
		if (request_ && request_->getBodyFilePath().size())
		{
			// std::cout << "[INFO] REQ removing created file for body \n";
			std::remove(request_->getBodyFilePath().c_str());
		}
		delete request_;
		request_ = new Request(conf_, get_fd(), epoll_fd_, client_addr_);
		status_code = 408;
	} else if (state_timout_ == 1 && static_cast<size_t>(now - cgi_timeout_) > conf_.get_cgi_timeout()) {
		status_code = 504;
	} else if (state_timout_ == 2 && static_cast<size_t>(now - resp_timeout_) > conf_.get_send_timeout()) {
		// std::cout << "[INFO] CLT response timeout: " << resp_timeout_ << std::endl;  // for response
		return true;
	}
	if (status_code) {
		request_->setParsingCode(status_code);
		request_->setReqState(RESP);
		// std::cout << "[INFO] CLT timout check state: " << state_timout_ << std::endl;
		delete response_;
		response_ = new HttpResponse(*request_, conf_);
		state_timout_ = 2;
	}
	return false;
}

int Client::receive(int epoll_fd) {
	int client_fd = this->get_fd();
	unsigned char buf[RECV_BUF];

	int nread = recv(client_fd, buf, RECV_BUF - 1, 0);
	
	if (nread <= 0) {
		if (nread == -1)
			perror("recv() failed");
		epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
		// std::cout << DISC_CLR << "\n$ Client disconnected! (epoll IN) fd: " << client_fd << DEF_CLR << std::endl;
		return -1;
	}
	Uvec tmp_vec_buf(buf, nread);
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
	// std::cout << "[INFO] CLT processing received data!" << std::endl;
	if (!request_) {
		try {
			request_ = new Request(conf_, get_fd(), epoll_fd_, client_addr_);
			request_->ParseRequest(vec_buf_);
			if (request_->getReqState() == PEND)
				vec_buf_.clear();

		} catch (const std::exception &e) {
			std::string error = e.what();
		}
	}
	else if (request_ && request_->getReqState() == PEND) {
		try {
			request_->addBody(vec_buf_);
			vec_buf_.clear();
		} catch (const std::exception &e) {
			std::string error = e.what();
		}
	}
	if (request_ && request_->getReqState() == CGI) {
		cgi_ = request_->getCgiObject();
	}
	if (request_ && request_->getReqState() == RESP) {
		// std::cout << "[INFO] CLT create a new response" << std::endl;
		if (!response_)
			response_ = new HttpResponse(*request_, conf_);
		state_timout_ = 2;
	}
	return (request_) ? request_->getReqState() : -1;
}

int Client::send_response() {
	resbuf_.clear();
	resbuf_ = response_->generateResponse();
	ssize_t	sent = send(this->get_fd(), resbuf_.c_str(), resbuf_.size(), MSG_NOSIGNAL);
	if (sent > 0) {
		reset_resp_timeout();
		if (response_->getRespState() == DONE) {
			// std::cout << "[INFO] CLT end of connection" << std::endl;
			return 1;
		}
	} else if (sent == 0) {
		return 1;
	} else {
		std::cerr << "[ERROR] send() failed!"<< std::endl;
		return 1;
	}
	return 0;
}

void Client::set_cgi_obj(std::map <int, Client*> &cgi_pipes, int flag) {
	int pipfd = 0;

	if (request_){
		if (state_timout_ == 0) {
			// std::cout << "[INFO] CLT Set CGI object! " << std::endl;
			state_timout_ = 1;
			reset_cgi_timeout();
		}
		for (int i = 0; i != 3; ++i) {
			pipfd = request_->get_cgi_pipe(i);
			if (pipfd != -1) {
				if (cgi_pipes.find(pipfd) == cgi_pipes.end() && flag) {
					// std::cout << "[INFO] CLT the object has been Added cgi_fd: " << pipfd << std::endl;
					cgi_pipes[pipfd] = this;
				} else if (cgi_pipes.find(pipfd) != cgi_pipes.end() && !flag) {
					// std::cout << "[INFO] CLT the object has been Removed cgi_fd: " << pipfd << std::endl;
					cgi_pipes.erase(pipfd);
				}
			}
		}
		if (!flag && cgi_ && cgi_->get_cgi_pid() > 0) {
			kill(cgi_->get_cgi_pid(), SIGTERM);
			// std::cout << "[INFO] CLT killing cgi child process\n";
		}
	}
}

int Client::cgi_pipe_io(int pipe_fd) {
	// std::cout << "[INFO] CLT cgi pipe I/O operation\n";
	if (cgi_) {
		if (pipe_fd == cgi_->get_pipe(0)) {
			return cgi_->write_body();
		} else {
			if (cgi_->read_output()) {
				request_->setReqState(RESP);
				cgi_->check_process_status();
				request_->setParsingCode((cgi_->get_cgi_exit_status() == 0) ? 200 : 500);
				response_ = new HttpResponse(*request_, conf_);
				return 1;
			}
		}
		return 0;
	}
	return 1;
}

int Client::kill_cgi() {
	// std::cout << "[INFO] CLT cgi child process\n";
	if (cgi_ && cgi_->get_cgi_pid()) {
		if (cgi_->check_process_status())
			return 1;
		kill(cgi_->get_cgi_pid(), SIGKILL);
		return 0;
	}
	return 1;
}
