/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   locationConf.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/21 12:00:41 by laoubaid          #+#    #+#             */
/*   Updated: 2025/10/28 10:57:19 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_CONF_HPP
# define LOCATION_CONF_HPP

# include "../include.hpp"

class serverConf;

enum MethodBits {
	DELETE_BIT	= 1,	//* 001
	POST_BIT	= 2,	//* 010
	GET_BIT		= 4 	//* 100
};

class locationConf
{
	private:
		std::string							path_;          // error if not set
		unsigned int						methods_;       // default GET, POST, DELETE
		size_t                              clt_body_max_size_; // default fall to server block
		bool								has_methods_;   // default false
		std::pair<int, std::string> 		redirect_;      // could not exist fall to server block
		bool								autoindex_;     // fall to server block
		std::string							root_;          // fall to server block
		std::string							index_;         // could not exist fall to server block
		bool								is_indexed_;    // default false
		std::string							up_store_;      // could not exist
		bool								is_upset_;      // default false
		bool								has_redirect_;  // default false
		bool								has_cgi_;       // default false
		std::map<std::string, std::string>	cgis_;			// <extention, bin-path> can be overridden default is empty

	public:
		locationConf() {};
		locationConf(std::string& str, serverConf& cfg);
		~locationConf();

		void set_methods(std::vector<std::string>& values);
		void set_clt_max_body_size(std::vector<std::string>& values);
		void set_redirect(std::vector<std::string>& values);
		void set_root(std::vector<std::string>& values);
		void set_index(std::vector<std::string>& values);
		void set_auto_index(std::vector<std::string>& values);
		void set_up_store(std::vector<std::string>& values);
		void set_cgi(std::vector<std::string>& values);

		bool								is_get_allowed() { return methods_ & GET_BIT; }
		bool								is_post_allowed() { return methods_ & POST_BIT; }
		bool								is_delete_allowed() { return methods_ & DELETE_BIT; }
		unsigned int						get_methods() const { return methods_; }
		size_t								get_clt_body_max_size() const { return clt_body_max_size_; }
		bool								is_upset() const { return is_upset_; }
		const std::string&					get_upstore() const { return up_store_; }
		bool								has_redirect() const { return has_redirect_; }
		const std::string&					get_path() const { return path_; };
		const std::string&					get_root() const { return root_; };
		const std::string&					get_index() const { return index_; };
		const std::pair<int, std::string>&	get_redirect() const { return redirect_; }
		bool								is_index() const { return is_indexed_; };
		bool								get_autoindex() const { return autoindex_; };
		bool								has_cgi() const { return has_cgi_; }
		bool								valid_cgi_ext(const std::string& ext) const { return (cgis_.find(ext) != cgis_.end()) ? true : false; }
		const std::string&					get_cgi_path(const std::string& ext) const { return cgis_.at(ext); }

};


#endif
