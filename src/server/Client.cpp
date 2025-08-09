/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/16 16:27:54 by laoubaid          #+#    #+#             */
/*   Updated: 2025/08/09 01:24:43 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client(int clt_fd) : Socket(clt_fd)
{
    request_ = NULL;
    response_ = NULL;
    std::cout << "Client constracteur called!" << std::endl;
}

Client::~Client()
{
}


int Client::receive(int epoll_fd) {

    std::cout << "++ ++ ++ recv request ++ ++ ++" << std::endl;
    int client_fd = this->get_fd();
    unsigned char buf[RECV_BUF];

    int nread = recv(client_fd, buf, RECV_BUF - 1, 0);
    
    if (nread <= 0) {
        if (nread == 0)
        std::cout << "Client disconnected! (recv() == 0)" << std::endl;
        else
        perror("recv() failed");
        close(client_fd);
        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
        std::cout << DISC_CLR << "\n$ Client disconnected! (epoll IN) fd: " << client_fd << DEF_CLR << std::endl;
        return -1;
    }
    // std::cout << "DEBUG\t\t\t\t\t\t\t\t\t\tDEBUG" << std::endl;
    Uvec tmp_vec_buf(buf, nread); // Convert the buffer to Uvec
    Uvec delimiter((const unsigned char*)"\r\n\r\n", 4);
    vec_buf_ += tmp_vec_buf;
    if (vec_buf_.size() < RECV_BUF && tmp_vec_buf.find(delimiter) == tmp_vec_buf.end()) {
        return 0;
    }
    process_recv_data();
    return (request_) ? request_->getReqState() : -1;
}

int Client::process_recv_data() {//Uvec &vec_buf) { //, uint32_t event) {
    std::cout << "+++ process_recv_data called!" << std::endl;
    if (!request_) {
        std::cout << "-- create new request" << std::endl;
        try {
            request_ = new HTTPRequestParser(vec_buf_);
            std::cout << "-- IDLE request state: " << request_->getReqState() << std::endl;
            std::cout << "-- IDLE request CODE: [" << request_->getParsingCode() << "]" << std::endl;
            
        } catch (const std::exception &e) {
            std::cerr << "Error creating HTTPRequestParser: " << e.what() << std::endl;
            return -1; // Handle error appropriately
        }
    }
    if (request_ && request_->getReqState() == PEND) {
        std::cout << "-- add to body\n";
        request_->addBody(vec_buf_);
        std::cout << "-- PEND request state: " << request_->getReqState() << std::endl;
    }
    if (request_ && request_->getReqState() == CCLS) {
        std::cout << "-- CCLS request is complete!\n";
        delete request_;
        request_ = NULL;
        std::cout << "-- request state: " << request_->getReqState() << std::endl;
    }
    if (request_ && request_->getReqState() == RESP )
    {
        response_ = new HttpResponse();

        resbuf_.clear();
        resbuf_ = HttpResponse::generateResponse(*request_);
        std::cout << "-- RESP request state: " << request_->getReqState() << std::endl;
        
        // send_response(event, 0);
        // delete request_;+
        // request_ = NULL; // Clean up the request pointer
    }

    return request_->getReqState();
}

void Client::send_response(uint32_t event, int epoll_fd) {
    std::cout << "-- Send response" << std::endl;
    if (send(this->get_fd(), resbuf_.c_str(), resbuf_.size(), MSG_NOSIGNAL) == -1) {
        close(this->get_fd());
        std::cout << DISC_CLR <<"\n$ Client disconnected! (epoll OUT) fd: " << this->get_fd() << DEF_CLR << std::endl;
        socket_related_err(" send() failed! , connection closed! ", 0);
    }
    delete request_;
    vec_buf_ = Uvec((const unsigned char*)"", 0);
    request_ = NULL;
}

// void Client::print_request() {
//     std::cout << YLW_CLR << ">\tthe client socket FD: " << this->get_fd() << DEF_CLR << std::endl;
//     std::cout << "---------------------------------------------------------------------------------------" << std::endl;
//     std::cout << "-------------------------------- THE START OF REQUEST! --------------------------------" << std::endl;
//     std::cout << init_req_ << std::endl;
//     std::cout << "--------------------------------- THE END OF REQUEST! ---------------------------------" << std::endl;
//     std::cout << "size of request: " << init_req_.size() << std::endl;
//     std::cout << "---------------------------------------------------------------------------------------" << std::endl;
//     std::cout << "\n\n" <<std::endl;
// }
