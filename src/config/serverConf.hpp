/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serverConf.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/21 11:02:59 by laoubaid          #+#    #+#             */
/*   Updated: 2025/08/28 16:45:58 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_CONF_HPP
# define SERVER_CONF_HPP

// # include "../include.hpp"
# include "locationConf.hpp"

class serverConf
{
    private:
        std::map<int, sockaddr_in>          listens;            // default 0.0.0.0:8080
        std::map<int, std::string>          err_pages;          // default empty
        size_t                              clt_body_max_size;  // default 1M
        std::string                         root;               // default ./www
        std::string                         index;              // default could not exist
        bool                                is_indexed;         // default false
        std::map<std::string, locationConf> locations;          // default could not exist
        std::pair<int, std::string>         redirect;           // default could not exist
        bool                                autoindex;          // default off
        
    public:
        serverConf(/* args */);
        serverConf(const serverConf& obj);
        ~serverConf();

        void add_listen(std::vector<std::string>& values);
        void add_err_page(std::vector<std::string>& values);
        void set_clt_max_body_size(std::vector<std::string>& values);
        void set_root(std::vector<std::string>& values);
        void set_index(std::vector<std::string>& values);
        void add_location(locationConf& lct);
        void set_redirect(std::vector<std::string>& values);
        void set_auto_index(std::vector<std::string>& values);
        void set_default();

        bool is_index() const { return is_indexed; }
        bool is_autoindex() const { return autoindex; }
        std::string get_index() const { return index; }
        std::string get_root() const { return root; }

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

        bool is_location(std::string& str) const {
            std::map<std::string, locationConf>::const_iterator it;
            for (it = locations.begin(); it != locations.end(); ++it){
                if ((*it).first.compare(0, str.size(), str) == 0)
                    return true;
            }
            return false;
        }

        const std::pair<int, std::string>& get_redirect() const {
            return redirect;
        }

        const locationConf& get_location(const std::string& path) const {
            std::map<std::string, locationConf>::const_iterator it = locations.find(path);
            if (it == locations.end()) {
                throw std::out_of_range("location not found");
            }
            return (*it).second;
        }

        const std::string& get_err_page(int code) const {
            static const std::string empty;
            std::map<int, std::string>::const_iterator it = err_pages.find(code);
            return (it == err_pages.end()) ? empty : it->second;
        }
};


#endif

