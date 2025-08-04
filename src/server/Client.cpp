/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/16 16:27:54 by laoubaid          #+#    #+#             */
/*   Updated: 2025/08/03 19:36:18 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client(int clt_fd) : Socket(clt_fd)
{
    request_ = NULL;
    std::cout << "Client constracteur called!" << std::endl;
}

Client::~Client()
{
}

// std::string tmp_response(int code) {
//     std::string response_;
//     if (code == 400) {
//         std::cout << RED_CLR << "400 Bad Request" << DEF_CLR << std::endl;
//         response_ = "HTTP/1.1 400 Bad Request\r\n\r\n";
//     }
//     else if (code == 200) {
//         std::cout << GRN_CLR << "200 OK" << DEF_CLR << std::endl;
//         std::string html = "<!DOCTYPE html><html><body><h1>Hello from the WebServer!</h1></body></html>";
//         response_ = "HTTP/1.1 200 OK\r\n"
//             "Content-Type: text/html\r\n"
//             "Content-Length: " + std::to_string(html.size()) + "\r\n"
//             "Connection: keep-alive\r\n"
//             "\r\n" + html;
//     }
//     else if (code == 413) {
//         std::cout << RED_CLR << "413 Payload Too Large" << DEF_CLR << std::endl;
//         std::string html = "<!DOCTYPE html><html><body><h1>413 Payload Too Large</h1></body></html>";
//         response_ = "HTTP/1.1 413 Payload Too Large\r\n"
//             "Content-Type: text/html\r\n"
//             "Content-Length: " + std::to_string(html.size()) + "\r\n"
//             "Connection: close\r\n"
//             "\r\n" + html;
//     }
//     else {
//         std::cout << RED_CLR << "500 Internal Server Error" << DEF_CLR << std::endl;
//         std::string html = "<!DOCTYPE html><html><body><h1>500 Internal Server Error</h1></body></html>";
//         response_ = "HTTP/1.1 500 Internal Server Error\r\n"
//             "Content-Type: text/html\r\n"
//             "Content-Length: " + std::to_string(html.size()) + "\r\n"
//             "Connection: close\r\n"
//             "\r\n" + html;
//     }

//     return response_;
// }


int Client::process_recv_data(Uvec &vec_buf, uint32_t event) {
    std::cout << "+++ process_recv_data called!" << std::endl;
    if (!request_) {
        std::cout << "-- create new request" << std::endl;
        try {
            request_ = new HTTPRequestParser(vec_buf);
            std::cout << "-- IDLE request state: " << request_->getReqState() << std::endl;
        } catch (const std::exception &e) {
            std::cerr << "Error creating HTTPRequestParser: " << e.what() << std::endl;
            return -1; // Handle error appropriately
        }
    }
    if (request_ && request_->getReqState() == PEND) {
        std::cout << "-- add to body\n";
        request_->addBody(vec_buf);
        std::cout << "-- PEND request state: " << request_->getReqState() << std::endl;
    }
    if (request_ && request_->getReqState() == CCLS) {
        std::cout << "-- CCLS request is complete!\n";
        delete request_;
        request_ = NULL;
        std::cout << "-- request state: " << request_->getReqState() << std::endl;
    }
    if (request_ && request_->getReqState() == RESP)
    {
        std::cout << "-- Send response\n";
        // the request is complete, we can send the response now
        // entering the response main function
        resbuf_.clear();
        resbuf_ = HttpResponse::generateResponse(*request_);
        send_response(event);
        std::cout << "-- RESP request state: " << request_->getReqState() << std::endl;
        delete request_;
        request_ = NULL; // Clean up the request pointer
    }
    
    // resbuf_ = tmp_response(request_->getParsingCode());
    // send_response(event);
    return 0;
}

void Client::send_response(uint32_t event) {
    if (event & EPOLLOUT) {
        std::cout << "EPOLLOUT event is detected!" << std::endl;
        if (send(this->get_fd(), resbuf_.c_str(), resbuf_.size(), MSG_NOSIGNAL) == -1) {
            close(this->get_fd());
            std::cout << DISC_CLR <<"\n$ Client disconnected! (epoll OUT) fd: " << this->get_fd() << DEF_CLR << std::endl;
            socket_related_err(" send() failed! , connection closed! ", 0);
        }
    }
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
