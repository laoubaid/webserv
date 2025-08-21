/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   locationConf.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/21 12:00:41 by laoubaid          #+#    #+#             */
/*   Updated: 2025/08/21 13:27:16 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_CONF_HPP
# define LOCATION_CONF_HPP

# include "../include.hpp"

class locationConf
{
    private:
        std::string                 path;
        unsigned char               methods;
        std::map<int, std::string>  redirs;
        bool                        autoindex;
        std::string                 root;
        std::string                 index;
        std::string                 up_store;

    public:
        locationConf(std::string& str);
        ~locationConf();

        void set_methods(std::vector<std::string>& values);
        void add_redir(std::vector<std::string>& values);
        void set_root(std::vector<std::string>& values);
        void set_index(std::vector<std::string>& values);
        void set_auto_index(std::vector<std::string>& values);
        void set_up_store(std::vector<std::string>& values);
};


#endif
