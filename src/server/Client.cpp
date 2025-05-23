/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/16 16:27:54 by laoubaid          #+#    #+#             */
/*   Updated: 2025/05/23 19:31:24 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client(int clt_fd) : Socket(clt_fd)
{
    request_ = NULL;
    req_stat_ = IDLE;
    std::cout << "Client constracteur called!" << std::endl;
}

Client::~Client()
{
}

std::string tmp_response(int code) {
    std::string response_;
    if (code == 400) {
        std::cout << RED_CLR << "400 Bad Request" << DEF_CLR << std::endl;
        response_ = "HTTP/1.1 400 Bad Request\r\n\r\n";
    }
    else if (code == 200) {
        std::cout << GRN_CLR << "200 OK" << DEF_CLR << std::endl;
        std::string html = "<!DOCTYPE html><html><body><h1>Hello from the WebServer!</h1></body></html>";
        response_ = "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: " + std::to_string(html.size()) + "\r\n"
            "Connection: keep-alive\r\n"
            "\r\n" + html;
    }
    else if (code == 413) {
        std::cout << RED_CLR << "413 Payload Too Large" << DEF_CLR << std::endl;
        std::string html = "<!DOCTYPE html><html><body><h1>413 Payload Too Large</h1></body></html>";
        response_ = "HTTP/1.1 413 Payload Too Large\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: " + std::to_string(html.size()) + "\r\n"
            "Connection: close\r\n"
            "\r\n" + html;
    }
    else {
        std::cout << RED_CLR << "500 Internal Server Error" << DEF_CLR << std::endl;
        std::string html = "<!DOCTYPE html><html><body><h1>500 Internal Server Error</h1></body></html>";
        response_ = "HTTP/1.1 500 Internal Server Error\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: " + std::to_string(html.size()) + "\r\n"
            "Connection: close\r\n"
            "\r\n" + html;
    }

    return response_;
}


int Client::process_recv_data(std::vector <char> buf, uint32_t event) {
    
    if (req_stat_ == IDLE) {
        request_ = new HTTPRequestParser(buf.data()); // len
    }
    if (req_stat_ == PEND) {
        std::cout << "add to body\n";
        // request_->addtobody(buf, len);
    }
    if (req_stat_ == CCLS)
        delete request_;
    if (req_stat_ == RESP)
    {
        // HTTPResponse()
        send_response(event);
    }

    // wait for kamal
    
    resbuf_ = tmp_response(request_->getParsingCode());
    send_response(event);
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
