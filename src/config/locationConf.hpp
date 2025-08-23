/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   locationConf.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/21 12:00:41 by laoubaid          #+#    #+#             */
/*   Updated: 2025/08/23 10:49:43 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_CONF_HPP
# define LOCATION_CONF_HPP

# include "../include.hpp"

class serverConf;

enum MethodBits {
    DELETE_BIT = 1, // 001
    POST_BIT   = 2, // 010
    GET_BIT    = 4  // 100
};

class locationConf
{
    private:
        std::string                 path;       // error if not set
        unsigned char               methods;    // default GET, POST, DELETE
        bool                        is_methods; // default false
        std::map<int, std::string>  redirs;     // could not exist fall to server block
        bool                        autoindex;  // fall to server block
        std::string                 root;       // fall to server block
        std::string                 index;      // could not exist fall to server block
        bool                        is_indexed; // default false
        std::string                 up_store;   // could not exist
        bool                        is_upset;   // default false

    public:
        locationConf() {};
        locationConf(std::string& str, serverConf& cfg);
        ~locationConf();

        void set_methods(std::vector<std::string>& values);
        void add_redir(std::vector<std::string>& values);
        void set_root(std::vector<std::string>& values);
        void set_index(std::vector<std::string>& values);
        void set_auto_index(std::vector<std::string>& values);
        void set_up_store(std::vector<std::string>& values);

        bool is_get_allowed() {
            return methods & GET_BIT;
        }
        bool is_post_allowed() {
            return methods & POST_BIT;
        }
        bool is_delete_allowed() {
            return methods & DELETE_BIT;
        }

        const std::string& get_path() const { 
            return path;
        };
        const std::string& get_root() const { 
            return root;
        };
        const std::string& get_index() const { 
            return index;
        };
        bool    is_index() const {
            return is_indexed;
        };
        bool    get_autoindex() const {
            return autoindex;
        };
};


#endif
