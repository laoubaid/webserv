/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/03 19:19:18 by laoubaid          #+#    #+#             */
/*   Updated: 2025/09/11 22:19:47 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTPResponse.hpp"
#include "../cgi/Cgi.hpp"


std::map<int, std::string> HttpResponse::status_lines;

void init_status_lines() {
	HttpResponse::status_lines[200] = OK_200_;

	HttpResponse::status_lines[400] = "HTTP/1.1 400 Bad Request\r\n";
	HttpResponse::status_lines[401] = "HTTP/1.1 401 Unauthorized\r\n";
	HttpResponse::status_lines[403] = "HTTP/1.1 403 Forbidden\r\n";
	HttpResponse::status_lines[404] = "HTTP/1.1 404 Not Found\r\n";
	HttpResponse::status_lines[405] = "HTTP/1.1 405 Method Not Allowed\r\n";
	HttpResponse::status_lines[408] = "HTTP/1.1 408 Request Timeout\r\n";
	HttpResponse::status_lines[413] = "HTTP/1.1 413 Entity Too Large\r\n";

	HttpResponse::status_lines[500] = "HTTP/1.1 500 Internal Server Error\r\n";
	HttpResponse::status_lines[501] = "HTTP/1.1 501 Not Implemented\r\n";
	HttpResponse::status_lines[502] = "HTTP/1.1 502 Bad Gateway\r\n";
	HttpResponse::status_lines[503] = "HTTP/1.1 503 Service Unavailable\r\n";
	HttpResponse::status_lines[504] = "HTTP/1.1 504 Gateway Timeout\r\n";

	HttpResponse::status_lines[301] = "HTTP/1.1 301 Moved Permanently\r\n";
	HttpResponse::status_lines[302] = "HTTP/1.1 302 Found\r\n";
	HttpResponse::status_lines[303] = "HTTP/1.1 303 See Other\r\n";
	HttpResponse::status_lines[307] = "HTTP/1.1 307 Temporary Redirect\r\n";
	HttpResponse::status_lines[308] = "HTTP/1.1 308 Permanent Redirect\r\n";
}

const std::string& HttpResponse::getMimeType(const std::string& path) {
	static const std::map<std::string, std::string> mime_types = {
		{".html", "text/html"},
		{".htm",  "text/html"},
		{".css",  "text/css"},
		{".js",   "application/javascript"},
		{".json", "application/json"},
		{".png",  "image/png"},
		{".jpg",  "image/jpeg"},
		{".jpeg", "image/jpeg"},
		{".gif",  "image/gif"},
		{".ico",  "image/x-icon"},
		{".txt",  "text/plain"},
		{".pdf",  "application/pdf"},
		{".svg",  "image/svg+xml"}
	};

	static const std::string default_type = "application/octet-stream";

	
	size_t idx = path.rfind("/");
	std::string tmp;
	if (idx != std::string::npos) {
		tmp = path.substr(idx + 1);
		idx = tmp.rfind('.');
		if (idx != std::string::npos) {
			tmp = tmp.substr(idx);
		}
	}
	
	std::map<std::string, std::string>::const_iterator it = mime_types.find(tmp);
	if (it != mime_types.end())
		return it->second;
	return default_type;
}

bool HttpResponse::serveStaticContent(const std::string& path, int code) {
	file_.open(path, std::ios::in | std::ios::binary);
	if (!file_.is_open()) {
		handle_error(403);
		return false;
	}
	resp_buff_ = status_lines.at(code);
	resp_buff_ += "Content-Type: " + HttpResponse::getMimeType(path) + "\r\n" "Content-Length: " + std::to_string(get_file_size(file_)) + "\r\n\r\n";
	resp_stat_ = LOAD;
	return true;
}

bool HttpResponse::list_directory() {
	std::string direname;
	DIR* dir = opendir(uri_.c_str());

	resp_buff_.clear();
	if (dir == NULL) {
		handle_error(403);
		return false;
	}
	size_t idx = uri_.rfind("/");
	if (idx != std::string::npos)
		direname = uri_.substr(idx + 1);

	std::string body = "<!DOCTYPE html><html><head><title>Index of " + direname + \
	"</title></head><body><h1>Index of " + direname + "</h1><ul>";

	struct dirent *ptr;
	ptr = readdir(dir);
	while (ptr) {
		std::string tmp = std::string(ptr->d_name);
		if (tmp != "." && tmp != "..")
			body += "<li><a href=\"/" + direname + "/" + tmp + "\">" + tmp + "</a></li>";
		ptr = readdir(dir);
	}
	body += "</ul></body></html>";
	resp_buff_ = OK_200_;
	resp_buff_ += "Content-Type: text/html\r\n"
		"Content-Length: " + std::to_string(body.size()) + "\r\n\r\n" + body;

	closedir(dir);
	resp_stat_ = DONE;
	return true;
}

bool	HttpResponse::read_file_continu() {
	// Open the file only once in the class member 
	if (!file_.is_open() || resp_stat_ == DONE) {
		return false;
	}

	char buffer[FILE_BUFFER_SIZE];     //! change later
	file_.read(buffer, sizeof(buffer));
	resp_buff_ = std::string(buffer, file_.gcount());
	if (file_.eof()) {
		resp_stat_ = DONE;
		file_.close();
	}
	return true;
}

void	HttpResponse::handle_error(int err_code) {
	std::string path = conf_.get_err_page(err_code);
	if (!path.empty()) {
		path = conf_.get_root() + "/" + path;
		if (serveStaticContent(path, err_code))
			return ;
	}
	switch (err_code)
	{
		case 400:
			resp_buff_ =  BADR_400_;
			break;
		case 403:
			resp_buff_ =  FORB_403_;
			break;
		case 404:
			resp_buff_ =  NOTF_404_;
			break;
		case 405:
			resp_buff_ =  METN_405_;
			break;
		case 413:
			resp_buff_ =  ELRG_413_;
			break;
		case 500:
			resp_buff_ =  IERR_500_;
			break;
		case 501:
			resp_buff_ =  NIMP_501_;
			break;
		// case 413:
		//     resp_buff_ =  ELRG_413_;
		//     break;
		
		default:
			break;
	}
	resp_stat_ = DONE;
}

// bool	HttpResponse::handle_cgi() {

// 	if (location_.has_cgi()) {
// 		std::size_t pos = uri_.rfind('.');
// 		if (pos != std::string::npos) {
// 			std::string extension = uri_.substr(pos);
// 			std::cout << "\t|\tExtension: " << extension << std::endl;
// 			if (location_.valid_cgi_ext(extension)) {
// 				// std::cout << "\t|\t|\tvalid cgi extention! : ";
// 				// std::cout << location_.get_cgi_path(extension) << std::endl;
// 				//? now we need to run the cgi /bin-path uri e.g:  /bin/python3  /home/user/webserv/www/cgi-bin/test.py 
// 				//* create a cgi object init with location
// 				Cgi test(location_, uri_);

// 				//* run the cgi to begin with
// 				test.run(request_, extension);

// 				//? after this we should give it input until it ends then take output

// 			}
// 			// return 0;        // this should decied wither i do server static files or not
// 		} else {
// 			std::cout << "No extension found" << std::endl;
// 		}
// 	}
// 	return 1;
// }

void HttpResponse::process_path() {
	std::string target_path = uri_;
	uri_ = location_.get_root() + uri_;

	if (!access(uri_.c_str(), F_OK)) {
		if (!access(uri_.c_str(), R_OK)) {
			if (is_directory(uri_)) {
				//info std::cout << "Directory + GET → serve index, else autoindex if on, else 403\n"; // delme
				if (location_.is_index()) {
					uri_ = target_path + "/" + location_.get_index();
					process_path();
					return ;
				} else if (location_.get_autoindex()) {
					list_directory();
				} else {
					handle_error(403);
				}
			} else {
				// if (handle_cgi())
				// 	serveStaticContent(uri_, 200);
				serveStaticContent(uri_, 200);
				return ;
			}
		} else
			handle_error(403);
	} else
		handle_error(404);
}

void HttpResponse::responesForGet() {
	if (resp_stat_ == STRT) {
		process_path();
	} else if (resp_stat_ == LOAD) {
		read_file_continu();
	}
}

void HttpResponse::responesForDelete() {
	std::string path = location_.get_root() + uri_;

	if (!access(path.c_str(), F_OK)) {
		if (!access(path.c_str(), R_OK)) {
			if (is_directory(path)) {
				handle_error(403);
			} else {
				if (std::remove(path.c_str()) == 0) {
					resp_buff_ = OK_200_;
					resp_buff_ += "\r\n";
					resp_stat_ = DONE;
				} else {
					handle_error(403);
				}
			}
		} else {
			handle_error(403);
		}
	} else {
		handle_error(404);
	}
}

bool    HttpResponse::check_redirection(const locationConf& cfg) {
	const std::pair<int, std::string>& redir = cfg.get_redirect();
	
	if (redir.first != 0) {
		// std::cout << "redirection detected!\n";
		resp_buff_ = status_lines.at(redir.first);
		// resp_buff_ += "Location: " + redir.second + "\r\n";
		resp_buff_ += "Location: ";
		resp_buff_ += "https:/youtu.be/dQw4w9WgXcQ?si=Faj1lWWybcBb6y3d";
		resp_buff_ += "\r\n";
		resp_buff_ += "Content-Length: 0\r\n\r\n";
		resp_stat_ = DONE;
		return true;
	}
	return false;
}

const std::string HttpResponse::generateResponse() {
	std::cout << GRN_CLR << "Generating response ..." << DEF_CLR << std::endl;

	resp_buff_.clear();
	if (status_code_ == 200) {
		t_method    method = request_.getMethod();
		if (check_redirection(location_)) {
			return resp_buff_;
		}
		if (method == GET) {
			responesForGet();
		} else if (method == DELETE) {
			responesForDelete();
		} else {  // this is for POST method, temporarily of course hhhh
			// std::cout << GRN_CLR << "200 OK" << DEF_CLR << std::endl;
			std::string html = "<!DOCTYPE html><html><body><h1>Hello from the WebServer!</h1></body></html>\n";
			resp_buff_ = "HTTP/1.1 200 OK\r\n"
						"Content-Type: text/html\r\n"
						"Content-Length: " + std::to_string(html.size()) + "\r\n"
						"Connection: keep-alive\r\n"
						"\r\n" + html;
		}
	} else {
		// std::cout << RED_CLR <<  " request Error status code : " << status_code << DEF_CLR << std::endl;
		handle_error(status_code_);
	}
	return resp_buff_;
}

HttpResponse::HttpResponse(Request& request, const serverConf& conf)
	: request_(request),
	  conf_(conf),
	  uri_(request_.getTarget().path),
	  location_(request_.get_location()),
	  resp_stat_(STRT) {
	//* response constracteur!
	status_code_ = request_.getParsingCode();
}

t_resp_state& HttpResponse::getRespState() {
	return resp_stat_;
}

std::string&   HttpResponse::getRespBuff() {
	return resp_buff_;
}

