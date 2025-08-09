/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/16 16:27:54 by laoubaid          #+#    #+#             */
/*   Updated: 2025/08/09 19:12:03 by laoubaid         ###   ########.fr       */
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
    delete request_;
    delete response_;
}

void Client::set_event(int epoll_fd, uint32_t events) {
    std::cout << "|\t|\tsetting events" << std::endl; 
    epoll_event ev{};
    ev.data.fd = get_fd();
    ev.events = events;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, ev.data.fd, &ev) == -1) {
        perror("epoll_ctl");
        throw std::runtime_error("Failed to modify epoll event");
    }
}

int Client::receive(int epoll_fd) {

    std::cout << "|\treceive request" << std::endl;
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
    // std::cout << "DEBUG\t\t\t\t\t\t\t\t\t\tDEBUG" << std::endl;
    Uvec tmp_vec_buf(buf, nread); // Convert the buffer to Uvec
    Uvec delimiter((const unsigned char*)"\r\n\r\n", 4);
    vec_buf_ += tmp_vec_buf;
    print_whatever("test");
    if (vec_buf_.size() < RECV_BUF && tmp_vec_buf.find(delimiter) == tmp_vec_buf.end()) {
        return 0;
    }
    process_recv_data();
    return (request_) ? request_->getReqState() : -1;
}

int Client::process_recv_data() {
    std::cout << "process_recv_data called!" << std::endl;
    if (!request_) {
        std::cout << "|\tcreate new request" << std::endl;
        try {
            request_ = new HTTPRequestParser(vec_buf_);
            std::cout << "|\t|\tIDLE request state: " << request_->getReqState() << std::endl;
            std::cout << "|\t|\tIDLE request CODE: [" << request_->getParsingCode() << "]" << std::endl;
            
        } catch (const std::exception &e) {
            std::cerr << "Error creating HTTPRequestParser: " << e.what() << std::endl;
            return -1; // Handle error appropriately
        }
    }
    if (request_ && request_->getReqState() == PEND) {
        std::cout << "|\tadd to body\n";
        request_->addBody(vec_buf_);
        std::cout << "|\t|\tPEND request state: " << request_->getReqState() << std::endl;
    }
    if (request_ && request_->getReqState() == CCLS) {
        std::cout << "|\tCCLS request is complete!\n";
        delete request_;
        request_ = NULL;
        std::cout << "|\t|\trequest state: " << request_->getReqState() << std::endl;
    }
    if (request_ && request_->getReqState() == RESP) {
        response_ = new HttpResponse();

        resbuf_.clear();
        resbuf_ = HttpResponse::generateResponse(*request_);
        std::cout << "|\t|\tRESP request state: " << request_->getReqState() << std::endl;
        
        // send_response(event, 0);
        // delete request_;
        // request_ = NULL; // Clean up the request pointer
    }

    return request_->getReqState();
}

int Client::send_response(int epoll_fd) {
    std::cout << "|\tSend response" << std::endl;
    // print_whatever("test");
    if (send(this->get_fd(), resbuf_.c_str(), resbuf_.size(), MSG_NOSIGNAL) == -1) {
        std::cout << DISC_CLR <<"\n$ Client disconnected! (epoll OUT) fd: " << this->get_fd() << DEF_CLR << std::endl;
        socket_related_err(" send() failed! , connection closed! ", 0);
        return 1;
    }
    set_event(epoll_fd, EPOLLIN | EPOLLET);
    delete request_;
    vec_buf_ = Uvec((const unsigned char*)"", 0);
    request_ = NULL;
    return 0;
}

void Client::print_whatever(std::string whatever) {
    std::cout << YLW_CLR << ">\tthe client socket FD: " << this->get_fd() << DEF_CLR << std::endl;
    std::cout << "---------------------------------------------------------------------------------------" << std::endl;
    std::cout << "-------------------------------- THE START OF whatever! --------------------------------" << std::endl;
    vec_buf_.print();
    std::cout << "--------------------------------- THE END OF whatever! ---------------------------------" << std::endl;
    std::cout << "size of whatever: " << vec_buf_.size() << std::endl;
    std::cout << "---------------------------------------------------------------------------------------" << std::endl;
    std::cout << "\n\n" <<std::endl;
}