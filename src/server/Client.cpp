/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/16 16:27:54 by laoubaid          #+#    #+#             */
/*   Updated: 2025/08/11 18:19:54 by laoubaid         ###   ########.fr       */
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
    std::cout << "Client destructor called!" << std::endl;
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
    // std::cout << "DEBUG\t\t\t\t\t\t\t\t\t\tDEBUG" << std::endl;
    Uvec tmp_vec_buf(buf, nread); // Convert the buffer to Uvec
    Uvec delimiter((const unsigned char*)"\r\n\r\n", 4);
    // delimiter.print();
    vec_buf_ += tmp_vec_buf;
    // print_whatever("test");
    if (vec_buf_.size() < RECV_BUF && vec_buf_.find(delimiter) == vec_buf_.end()) {
        return 0;
    }
    process_recv_data();
    return (request_) ? request_->getReqState() : -1;
}

int Client::process_recv_data() {
    std::cout << "process_recv_data called!" << std::endl;
    if (!request_) {
        // std::cout << "|\tcreate new request" << std::endl;
        // vec_buf_.print();
        try {
            request_ = new HTTPRequestParser(vec_buf_);
            // // std::cout << "|\t|\tIDLE request state: " << request_->getReqState() << std::endl;
            // // std::cout << "|\t|\tIDLE request CODE: [" << YLW_CLR << request_->getParsingCode()\
             << DEF_CLR << "]" << std::endl;
            
            if (request_->getReqState() == PEND) {
                vec_buf_ = Uvec((const unsigned char*)"", 0);  // ugly clear hhhh
            }
            
        } catch (const std::exception &e) {
            std::cerr << "Error creating HTTPRequestParser: " << e.what() << std::endl;
            return -1; // Handle error appropriately
        }
    }
    if (request_ && request_->getReqState() == PEND) {
        try {
            // std::cout << "|\tadd to body\n";
            vec_buf_.print();
            request_->addBody(vec_buf_);
            // // std::cout << "|\t|\tPEND request state: " << request_->getReqState() << std::endl;
        } catch (const std::exception &e) {
            std::cerr << "Error adding to body: " << e.what() << std::endl;
            return -1; // Handle error appropriately
        }
    }
    if (request_ && request_->getReqState() == CCLS) {
        // std::cout << "|\tCCLS somthing happened close connection!\n";
        // std::cout << "|\t|\tCCLS request state: " << request_->getReqState() << std::endl;
        delete request_;
        request_ = NULL;
    }
    if (request_ && request_->getReqState() == RESP) {
        response_ = new HttpResponse(request_);

        resbuf_.clear();
        resbuf_ = response_->generateResponse();
        // std::cout << "|\t|\tRESP request state: " << request_->getReqState() << std::endl;
    }

    return (request_) ? request_->getReqState() : -1;
}

int Client::send_response(int epoll_fd) {
    // std::cout << "|\tSend response" << std::endl;
    // print_whatever("test");
    if (send(this->get_fd(), resbuf_.c_str(), resbuf_.size(), MSG_NOSIGNAL) == -1) {
        std::cout << DISC_CLR <<"\n$ Client disconnected! (epoll OUT) fd: " << this->get_fd() << DEF_CLR << std::endl;
        socket_related_err(" send() failed! , connection closed! ", 0);
        return 1;
    }
    delete request_;
    request_ = NULL;
    set_event(epoll_fd, EPOLLIN);
    vec_buf_ = Uvec((const unsigned char*)"", 0);  // ugly clear hhhh
    return 0;
}

void Client::print_whatever(std::string whatever) {
    std::cout << whatever << std::endl;
    std::cout << YLW_CLR << ">\tthe client socket FD: " << this->get_fd() << DEF_CLR << std::endl;
    std::cout << "---------------------------------------------------------------------------------------" << std::endl;
    std::cout << "-------------------------------- THE START OF whatever! --------------------------------" << std::endl;
    vec_buf_.print();
    std::cout << "--------------------------------- THE END OF whatever! ---------------------------------" << std::endl;
    std::cout << "size of whatever: " << vec_buf_.size() << std::endl;
    std::cout << "---------------------------------------------------------------------------------------" << std::endl;
    std::cout << "\n\n" <<std::endl;
}