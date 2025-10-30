/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/03 19:19:18 by laoubaid          #+#    #+#             */
/*   Updated: 2025/10/30 20:54:04 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTPResponse.hpp"
#include "../cgi/Cgi.hpp"


std::map<int, std::string> HttpResponse::status_lines;
std::map<int, std::string> HttpResponse::error_pages;

void init_status_lines() {
	HttpResponse::status_lines[200] = "HTTP/1.1 200 OK\r\n";
	HttpResponse::status_lines[201] = "HTTP/1.1 201 Created\r\n";
	HttpResponse::status_lines[204] = "HTTP/1.1 204 No Content\r\n";

	HttpResponse::status_lines[400] = "HTTP/1.1 400 Bad Request\r\n";
	HttpResponse::status_lines[401] = "HTTP/1.1 401 Unauthorized\r\n";
	HttpResponse::status_lines[403] = "HTTP/1.1 403 Forbidden\r\n";
	HttpResponse::status_lines[404] = "HTTP/1.1 404 Not Found\r\n";
	HttpResponse::status_lines[405] = "HTTP/1.1 405 Method Not Allowed\r\n";
	HttpResponse::status_lines[408] = "HTTP/1.1 408 Request Timeout\r\n";
	HttpResponse::status_lines[409] = "HTTP/1.1 409 Conflict\r\n";
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

void init_error_pages() {
	HttpResponse::error_pages[400] = BADR_400_;
	HttpResponse::error_pages[401] = UNAUTH_401_;
	HttpResponse::error_pages[403] = FORB_403_;
	HttpResponse::error_pages[404] = NOTF_404_;
	HttpResponse::error_pages[405] = METN_405_;
	HttpResponse::error_pages[408] = RTIM_408_;
	HttpResponse::error_pages[409] = CNFL_409_;
	HttpResponse::error_pages[413] = ELRG_413_;
	HttpResponse::error_pages[500] = IERR_500_;
	HttpResponse::error_pages[501] = NIMP_501_;
	HttpResponse::error_pages[502] = BGAT_502_;
	HttpResponse::error_pages[503] = SUNA_503_;
	HttpResponse::error_pages[504] = GTIM_504_;
}


HttpResponse::~HttpResponse() {
	
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
	// std::cout << "[INFO] RESP serving static file path: " << path << std::endl;
	file_.open(path, std::ios::in | std::ios::binary);
	if (!file_.is_open()) {
		handle_error(403);
		return false;
	}
	resp_buff_ = status_lines.at(code);
	resp_buff_ += "Content-Type: " + HttpResponse::getMimeType(path) + "\r\n" "Content-Length: " + std::to_string(get_file_size(file_)) + "\r\n\r\n";
	resp_stat_ = LOAD;
	request_.setMethod(GET);
	status_code_ = 200;
	return true;
}

bool HttpResponse::list_directory() {
	std::string target_path = location_.get_root() + uri_;
	std::string direname;
	DIR* dir = opendir(target_path.c_str());

	// std::cout << "[INFO] RESP target_path in list directory : " << target_path << std::endl;
	resp_buff_.clear();
	if (dir == NULL) {
		handle_error(403);
		return false;
	}
	size_t idx = target_path.rfind("/");
	if (idx != std::string::npos)
		direname = target_path.substr(idx + 1);

	std::string body = "<!DOCTYPE html>\n<html>\n<head>\n<title>Index of " + direname + \
	"</title>\n</head>\n<body>\n<h1>Index of " + direname + "</h1>\n<ul>\n";

	struct dirent *ptr;
	ptr = readdir(dir);
	while (ptr) {
		std::string filename = std::string(ptr->d_name);
		if (filename != "." && filename != "..") {
			std::string filepath = "/" + direname + "/" + filename;
			filepath = resolve_path(filepath);
			body += "<li><a href=\"" + filepath + "\">" + filename + "</a></li>\n";
		}
		ptr = readdir(dir);
	}
	body += "</ul>\n</body>\n</html>\n";
	resp_buff_ = status_lines[200];
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

	char buffer[FILE_BUFFER_SIZE];
	file_.read(buffer, sizeof(buffer));
	resp_buff_ = std::string(buffer, file_.gcount());
	if (file_.eof()) {
		resp_stat_ = DONE;
		// std::cout << "[INFO] RESP Done reading resource file\n";
		file_.close();
	}
	return true;
}

void	HttpResponse::handle_error(int err_code) {
	err_depth++;
	std::string path = conf_.get_err_page(err_code);
	if (!path.empty() && err_depth == 1) {
		// std::cout << "[INFO] RESP error page found in the config" << std::endl;
		path = conf_.get_root() + "/" + path;
		path = resolve_path(path);
		if (serveStaticContent(path, err_code)) {
			err_depth = 0;
			return ;
		}
	}
	// std::cout << "[INFO] RESP handle error numero : " << err_code << std::endl;

    if (error_pages.count(err_code)) {
		// std::cout << "[INFO] RESP found the error pages default" << std::endl;
        resp_buff_ = error_pages[err_code];
    }
	resp_stat_ = DONE;
	err_depth = 0;
}

void HttpResponse::process_path() {
	std::string target_path = location_.get_root() + uri_;
	target_path = resolve_path(target_path);

	// std::cout << "[INFO] RESP the file to look for : " << target_path << std::endl;
	if (!access(target_path.c_str(), F_OK)) {
		if (!access(target_path.c_str(), R_OK)) {
			if (is_directory(target_path)) {
				if (location_.is_index()) {
					uri_ += "/" + location_.get_index();
					process_path();
					return ;
				} else if (location_.get_autoindex()) {
					list_directory();
				} else {
					handle_error(403);
				}
			} else {
				serveStaticContent(target_path, 200);
				return ;
			}
		} else
			handle_error(403);
	} else
		handle_error(404);
}

void HttpResponse::responseForGet() {
	if (resp_stat_ == STRT) {
		process_path();
	} else if (resp_stat_ == LOAD) {
		read_file_continu();
	}
}

void HttpResponse::responseForDelete() {
	std::string path = location_.get_root() + uri_;

	if (!access(path.c_str(), F_OK)) {
		if (!access(path.c_str(), R_OK)) {
			if (is_directory(path)) {
				handle_error(403);
			} else {
				if (std::remove(path.c_str()) == 0) {
					resp_buff_ = NOCNT_204_;
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

void HttpResponse::responseForPost() {
	if (status_code_ == 201) {
		resp_buff_ = "HTTP/1.1 201 Created\r\n"
					 "Content-Type: text/html\r\n"
					 "Content-Length: 0\r\n"
					 "Connection: close\r\n"
					 "\r\n";
	} else if (status_code_ == 204) {
		resp_buff_ = "HTTP/1.1 204 No Content\r\n"
					 "Connection: close\r\n"
					 "\r\n";
	} else {
		std::string html =
			"<!DOCTYPE html>\r\n"
			"<html>\r\n"
			"<head><title>Post Success</title></head>\r\n"
			"<body>\r\n"
			"<h1>POST request processed successfully</h1>\r\n"
			"<p>Your data has been received by the server.</p>\r\n"
			"</body>\r\n"
			"</html>\r\n";

		resp_buff_ = "HTTP/1.1 200 OK\r\n"
					"Content-Type: text/html; charset=UTF-8\r\n"
					"Content-Length: " + std::to_string(html.size()) + "\r\n"
					"Connection: keep-alive\r\n"
					"\r\n" +
					html;

	}
	resp_stat_ = DONE;
}

bool    HttpResponse::check_redirection(const locationConf& cfg) {
	const std::pair<int, std::string>& redir = cfg.get_redirect();
	
	if (redir.first != 0) {
		resp_buff_ = status_lines.at(redir.first);
		resp_buff_ += "Location: " + redir.second + "\r\n";
		resp_buff_ += "Content-Length: 0\r\n\r\n";
		resp_stat_ = DONE;
		return true;
	}
	return false;
}

void HttpResponse::cgi_response() {
	Cgi *cgi_ = request_.getCgiObject();
	
	if (resp_stat_ == LOAD) {
		read_file_continu();
		return ;
	}
	
	std::string filename = cgi_->get_outfile_path();
	file_.open(filename.c_str());
	
	// std::cout << "[INFO] RESP cgi response\n";
	if (!file_.is_open()) {
		handle_error(403);
		// std::cout << RED_CLR << "cat open this file: " << filename << DEF_CLR << std::endl;
		return;
	}

	std::string line;
	bool headers_complete = false;
	std::vector<std::string> headers;
	
	while (std::getline(file_, line) && !headers_complete) {
		if (!line.empty() && line[line.length() - 1] == '\r') {
			line.erase(line.length() - 1);
		}
		if (line.empty()) {
			headers_complete = true;
			break;
		}
		if (line.find(':') != std::string::npos) {
			headers.push_back(line);
		}
	}

	bool status_sent = false;
	for (std::vector<std::string>::iterator it = headers.begin(); it != headers.end(); ) {  // No ++it here!
		if (it->empty()) {
			++it;
			continue; 
		}
		std::string header = *it;
		
		// Check for CGI Status header
		size_t colon_pos = header.find(':');
		if (colon_pos != std::string::npos) {
			for (size_t i = 0; i < colon_pos; ++i) {
				header[i] = std::tolower(header[i]);
			}
		} else {
			handle_error(502);
			return ;
		}

		if (header.find("status:", 0, 7) == 0) {
			if (colon_pos != std::string::npos && colon_pos + 1 < header.length()) {
				std::string status_value = header.substr(colon_pos + 1);
				// Trim leading whitespace
				size_t start = status_value.find_first_not_of(" \t");
				if (start != std::string::npos) {
					status_value = status_value.substr(start);
				}
				std::string status_line = "HTTP/1.1 " + status_value + "\r\n";
				resp_buff_ = status_line;
				status_sent = true;
			}
			it = headers.erase(it);
		} else {
			++it;
		}
	}
	
	// If no status header found, send default 200 OK
	if (!status_sent) {
		std::string default_status = "HTTP/1.1 200 OK\r\n";
		resp_buff_ = default_status;
	}
	
	// Send all other headers
	for (std::vector<std::string>::iterator it = headers.begin(); it != headers.end(); ++it) {
		resp_buff_ += (*it) + "\r\n";
	}
	resp_buff_ += "\r\n";
	resp_stat_ = LOAD;
}

const std::string HttpResponse::generateResponse() {
	// std::cout << GRN_CLR << "[INFO] RESP Generating response ... " << DEF_CLR << std::endl;

	resp_buff_.clear();
	if (status_code_ / 100 == 2) {
		t_method    method = request_.getMethod();
		if (check_redirection(location_)) {
			return resp_buff_;
		}
		if (request_.getCgiObject() != NULL && resp_stat_ != DONE)
			cgi_response();
		else if (method == GET)
			responseForGet();
		else if (method == DELETE)
			responseForDelete();
		else
			responseForPost();
	} else {
		// std::cout << RED_CLR <<  " request Error status code : " << status_code_ << DEF_CLR << std::endl;
		handle_error(status_code_);
	}
	return resp_buff_;
}

HttpResponse::HttpResponse(Request& request, const serverConf& conf)
	: request_(request),
	  conf_(conf),
	  uri_(request_.getTarget().path),
	  location_(request_.get_location()),
	  resp_stat_(STRT),
	  err_depth(0) {
	//* response constracteur!
	// std::cout << "[INFO] RESP response constracter" << std::endl;
	status_code_ = request_.getParsingCode();
}

t_resp_state& HttpResponse::getRespState() {
	return resp_stat_;
}

std::string&   HttpResponse::getRespBuff() {
	return resp_buff_;
}

void   HttpResponse::setRespState(t_resp_state tmp) {
	resp_stat_ = tmp;
}

