/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serverConf.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/21 11:02:59 by laoubaid          #+#    #+#             */
/*   Updated: 2025/09/04 15:42:12 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_CONF_HPP
# define SERVER_CONF_HPP

# include "../include.hpp"
# include "locationConf.hpp"

// class locationConf;

class serverConf
{
    private:
        std::vector<sockaddr_in>            listens_;           // default 0.0.0.0:8080
        std::map<int, std::string>          err_pages_;         // default empty
        size_t                              clt_body_max_size_; // default 1M
        std::string                         root_;              // default ./www
        std::string                         index_;             // default could not exist
        bool                                is_indexed_;        // default false
        std::map<std::string, locationConf> locations_;         // default always "/" exist
        std::pair<int, std::string>         redirect_;          // default could not exist
        bool                                autoindex_;         // default off

    public:

        std::map<std::string, std::string> redirs;

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

        bool is_index() const { return is_indexed_; }
        bool is_autoindex() const { return autoindex_; }
        std::string get_index() const { return index_; }
        std::string get_root() const { return root_; }

        const locationConf& identifyie_location(const std::string& str) const ;

        bool is_location(std::string& str) const {
            std::map<std::string, locationConf>::const_iterator it;
            for (it = locations_.begin(); it != locations_.end(); ++it){
                if ((*it).first.compare(0, str.size(), str) == 0)
                    return true;
            }
            return false;
        }

        const std::vector<sockaddr_in>& get_addrs() const {
            return listens_;
        }

        const std::pair<int, std::string>& get_redirect() const {
            return redirect_;
        }

        const locationConf& get_location(const std::string& path) const {
            std::map<std::string, locationConf>::const_iterator it = locations_.find(path);
            if (it == locations_.end()) {
                throw std::out_of_range("location not found");
            }
            return (*it).second;
        }

        const std::string& get_err_page(int code) const {
            static const std::string empty;
            std::map<int, std::string>::const_iterator it = err_pages_.find(code);
            return (it == err_pages_.end()) ? empty : it->second;
        }

};


#endif

