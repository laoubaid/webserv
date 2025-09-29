/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serverConf.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/21 11:02:59 by laoubaid          #+#    #+#             */
/*   Updated: 2025/09/30 00:09:32 by laoubaid         ###   ########.fr       */
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
		size_t                              recv_timeout_;		// default 5s
		size_t                              send_timeout_;		// default 5s
		size_t                              cgi_timeout_;		// default 5s
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
		void set_timeout(std::vector<std::string>& values, int flag);
		void set_default();

		bool		is_index() const { return is_indexed_; }
		bool		is_autoindex() const { return autoindex_; }
		std::string	get_index() const { return index_; }
		std::string	get_root() const { return root_; }
		size_t		get_recv_timeout() const { return recv_timeout_; }
		size_t		get_cgi_timeout() const { return cgi_timeout_; }
		size_t		get_send_timeout() const { return send_timeout_; }

		const locationConf& identifie_location(const std::string& str) const ;

		bool is_location(std::string& str) const;
		const locationConf& get_location(const std::string& path) const;

		const std::vector<sockaddr_in>& get_addrs() const {
			return listens_;
		}

		const std::pair<int, std::string>& get_redirect() const {
			return redirect_;
		}

		const std::string get_err_page(int code) const;

};


#endif

