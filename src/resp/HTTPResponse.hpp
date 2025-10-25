/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/03 19:02:26 by laoubaid          #+#    #+#             */
/*   Updated: 2025/10/25 11:40:34 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

# include "../include.hpp"
# include "../req/HTTPRequestParser.hpp"
# include "../config/serverConf.hpp"
# include "default_pages.hpp"

# include <sys/stat.h>
# include <sys/types.h>
// # include <sys/syscall.h>
# include <dirent.h>


// # define FILE_BUFFER_SIZE 4096
// # define FILE_BUFFER_SIZE 8192
# define FILE_BUFFER_SIZE 16384

// create a static class HTTPResponse, so i dont need to create an instance of it
// but i can use its methods to generate responses based on the status code

typedef enum e_resp_state {
	STRT,
	LOAD,
	DONE
}	t_resp_state;

class HttpResponse {
	private:
		Request&			request_;
		std::fstream		file_;
		std::string			resp_buff_;
		const serverConf&	conf_;
		std::string			uri_;
		const locationConf	location_;
		t_resp_state		resp_stat_;
		int					status_code_;
		int					err_depth;
		
	public:
		static std::map<int, std::string> status_lines;
		static std::map<int, std::string> error_pages;
	
		HttpResponse(Request& request, const serverConf&   conf);
		~HttpResponse();

		const std::string			generateResponse();
		void						responseForGet();
		void						responseForDelete();
		void						responseForPost();
		void						cgi_response();

		bool						read_file_continu();

		void						handle_error(int err_code);

		void						process_path();
		// bool						handle_cgi();
		bool						serveStaticContent(const std::string& path, int code);
		bool						list_directory();

		bool						check_redirection(const locationConf& cfg);

		static const std::string&	getMimeType(const std::string& ext);
		t_resp_state&				getRespState();
		std::string&				getRespBuff();
		void				setRespState(t_resp_state tmp);
};

size_t		get_file_size(std::fstream &file);
bool		is_directory(const std::string& path);
std::string	url_decode(const std::string& str);


void init_status_lines();
void init_error_pages();

#endif
