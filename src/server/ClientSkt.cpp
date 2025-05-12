/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientSkt.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/16 16:27:54 by laoubaid          #+#    #+#             */
/*   Updated: 2025/05/02 22:22:50 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ClientSkt.hpp"

ClientSkt::ClientSkt(int clt_fd) : Socket(clt_fd)
{
    request_ = "";
    recv_done_ = 0;
    std::cout << "ClientSkt constracteur called!" << std::endl;
}

ClientSkt::~ClientSkt()
{
}

int ClientSkt::handle_request(const char *buf) {
    request_ += buf;
    if (request_.find("\r\n\r\n") != std::string::npos) {
        print_request();
        request_.clear();
        recv_done_ = 1;
        return 1;
    }
    return 0;
}

void ClientSkt::print_request() {
    std::cout << YLW_CLR << ">\tthe client socket FD: " << this->get_fd() << DEF_CLR << std::endl;
    std::cout << "---------------------------------------------------------------------------------------" << std::endl;
    std::cout << "-------------------------------- THE START OF REQUEST! --------------------------------" << std::endl;
    std::cout << request_ << std::endl;
    std::cout << "--------------------------------- THE END OF REQUEST! ---------------------------------" << std::endl;
    std::cout << "size of request: " << request_.size() << std::endl;
    std::cout << "---------------------------------------------------------------------------------------" << std::endl;
    std::cout << "\n\n" <<std::endl;
}