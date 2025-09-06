/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/16 16:27:54 by laoubaid          #+#    #+#             */
/*   Updated: 2025/09/06 01:11:57 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client(int clt_fd, const serverConf& conf) : Socket(clt_fd), conf_(conf)
{
    // std::cout << "Client constracteur called!" << std::endl;
    request_ = NULL;
    response_ = NULL;
}

Client::~Client()
{
    // std::cout << "Client destructor called!" << std::endl;
    delete request_;
    delete response_;
}

void Client::set_event(int epoll_fd, uint32_t events) {
    // std::cout << "|\t|\tsetting events" << std::endl; 
    struct epoll_event clt_event;
    memset(&clt_event, 0, sizeof(clt_event));
    clt_event.data.fd = get_fd();
    clt_event.events = events;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, clt_event.data.fd, &clt_event) == -1) {
        perror("epoll_ctl");
        throw std::runtime_error("Failed to modify epoll event");
    }
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
    
    // std::cout << "nread : " << nread << std::endl;
    
    Uvec tmp_vec_buf(buf, nread); // Convert the buffer to Uvec
    Uvec delimiter((const unsigned char*)"\r\n\r\n", 4);
    vec_buf_ += tmp_vec_buf;

    // std::cout << "vec_buf_" << vec_buf_.size() << std::endl;
    
    if (request_ && request_->getReqState() == PEND) {
        process_recv_data(); 
        return request_->getReqState();
    }
    
    if (vec_buf_.size() < RECV_BUF && vec_buf_.find(delimiter) == vec_buf_.end()) {
        return 0;
    }
    process_recv_data();
    return (request_) ? request_->getReqState() : -1;
}

int Client::process_recv_data() {
    // std::cout << "|\tprocess_recv_data called!" << std::endl;
    if (!request_) {
        try {
            request_ = new Request(vec_buf_, conf_, get_fd());
            std::cout << "|\t|\tIDLE request CODE: [" << YLW_CLR << request_->getParsingCode() << DEF_CLR << "]";
            std::cout << " state: {" << request_->getReqState() << "}" << std::endl;
            
            if (request_->getReqState() == PEND) {
                vec_buf_.clear();
                // vec_buf_ = Uvec((const unsigned char*)"", 0);  // ugly clear hhhh
            }
        } catch (const std::exception &e) {
            std::cerr << "Error creating HTTPRequestParser: " << e.what() << std::endl;
            return -1; // Handle error appropriately
        }
    }
    else if (request_ && request_->getReqState() == PEND) {
        // std::cout << "|\t|\tadding to body\n" << std::endl;
        try {
            request_->addBody(vec_buf_);
            vec_buf_.clear();
        } catch (const std::exception &e) {
            std::cerr << "Error adding to body: " << e.what() << std::endl;
            return -1; // Handle error appropriately
        }
    }
    // if (request_ && request_->getReqState() == CCLS) {
    //     std::cout << "is this usefull?\n";
    //     delete request_;
    //     request_ = NULL;
    // }
    if (request_ && request_->getReqState() == RESP) {
        // Cgi* tmp = request_->get_cgi();
        // if (tmp)
	    //     tmp->run(*request_);
        response_ = new HttpResponse(request_, conf_);
        // log();
    }
    return (request_) ? request_->getReqState() : -1;
}

/* send_response function returns 1 in case of error (send failed), 0 otherwise */
int Client::send_response() {
    // std::cout << "|\tSend response" << std::endl;
    // print_whatever("test");
    resbuf_.clear();
    resbuf_ = response_->generateResponse();
    if (send(this->get_fd(), resbuf_.c_str(), resbuf_.size(), MSG_NOSIGNAL) == -1) {
        // std::cout << DISC_CLR <<"\n$ Client disconnected! (epoll OUT) fd: " << this->get_fd() << DEF_CLR << std::endl;
        // perror("send() failed!, connection closed ");
        return 1;
    }
    if (response_->getRespState() == DONE) {
        delete request_;
        request_ = NULL;
        delete response_;
        response_ = NULL;
        vec_buf_ = Uvec((const unsigned char*)"", 0);  // ugly clear hhhh
    }
    return 0;
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