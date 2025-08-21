/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serverConf.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/21 11:02:59 by laoubaid          #+#    #+#             */
/*   Updated: 2025/08/21 16:47:32 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_CONF_HPP
# define SERVER_CONF_HPP

# include "locationConf.hpp"

class serverConf
{
    private:
        std::map<int, sockaddr_in>                  listens;
        std::map<int, std::string>                  err_pages;
        size_t                                      clt_body_max_size;
        std::string                                 root;
        std::string                                 index;
        std::vector<locationConf>                   locations;
    public:
        serverConf(/* args */);
        ~serverConf();

        void add_listen(std::vector<std::string>& values);
        void add_err_page(std::vector<std::string>& values);
        void set_clt_max_body_size(std::vector<std::string>& values);
        void set_root(std::vector<std::string>& values);
        void set_index(std::vector<std::string>& values);
        void add_location(locationConf& lct);

        sockaddr_in&        get_addr(int port) {
            return listens[port];
        }
        std::vector<int>    get_ports() {
            std::vector<int> ports;
            std::map<int, sockaddr_in>::iterator it;
            for (it = listens.begin(); it != listens.end(); ++it) {
                ports.push_back((*it).first);
            }
            return ports;
        }
};


#endif

