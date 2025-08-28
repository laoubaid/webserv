/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/03 19:19:18 by laoubaid          #+#    #+#             */
/*   Updated: 2025/08/28 02:42:55 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTPResponse.hpp"


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
    // the following are redirects not erros change the name or somthing
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

bool HttpResponse::list_directory(const std::string& path) {
    std::string direname;
    DIR* dir = opendir(path.c_str());

    resp_buff_.clear();
    if (dir == NULL) {
        handle_error(403);
        return false;
    }
    size_t idx = path.rfind("/");
    if (idx != std::string::npos)
        direname = path.substr(idx + 1);

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

bool HttpResponse::read_file_continu() {
    // Open the file only once in the class member 
    if (!file_.is_open() || resp_stat_ == DONE) {
        return false;
    }

    char buffer[FILE_BUFFER_SIZE];     // change later
    file_.read(buffer, sizeof(buffer));
    resp_buff_ = std::string(buffer, file_.gcount());
    if (file_.eof()) {
        resp_stat_ = DONE;
        file_.close();
    }
    return true;
}

void    HttpResponse::handle_error(int err_code) {
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
        case 413:
            resp_buff_ =  ELRG_413_;
            break;
        case 500:
            resp_buff_ =  IERR_500_;
            break;
        // case 413:
        //     resp_buff_ =  ELRG_413_;
        //     break;
        
        default:
            break;
    }
    resp_stat_ = DONE;
}

void HttpResponse::process_path(const locationConf& location, std::string& path) {
    std::string target_path = path;
    path = location.get_root() + path;

    if (!access(path.c_str(), F_OK)) {
        if (!access(path.c_str(), R_OK)) {
            if (is_directory(path)) {
                // Directory + GET → serve index, else autoindex if on, else 403
                if (location.is_index()) {
                    path = target_path + location.get_index();
                    process_path(location, path);
                    return ;
                } else if (location.get_autoindex()) {
                    list_directory(path);
                } else {
                    handle_error(403);
                }
            } else {
                serveStaticContent(path, 200);
                return ;
            }
        } else
            handle_error(403);
    } else
        handle_error(404);
}

void HttpResponse::responesForGet(const locationConf& location, std::string& path) {
    if (resp_stat_ == STRT) {
        process_path(location, path);
    } else if (resp_stat_ == LOAD) {
        read_file_continu();
    }
}

void HttpResponse::responesForDelete(const locationConf& location, std::string& path) {
    // if (path.size() == 1 && path == "/")
    //     path += "index.html";  // index is hardcoded?
    path = location.get_root() + path;
    // std::cout << ">>>>>>>>>> final target path : " << path << std::endl;

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

const locationConf& HttpResponse::identifyie_location(const std::string& str) {
    std::stringstream ss(str);
    std::string part;
    std::vector<std::string> stack;

    while (std::getline(ss, part, '/')) {
        if (part.empty())
            continue;
        stack.push_back(part);
    }

    std::string tmp_path;
    while (stack.size()) {
        // form the path
        tmp_path.clear();
        for (size_t i = 0; i < stack.size(); ++i) {
            tmp_path += "/" + stack[i];
        }
        // check with location
        if (conf_.is_location(tmp_path)) {
            return conf_.get_location(tmp_path);
        } else {
            if (!stack.empty())
                stack.pop_back();
        }
    }
    tmp_path = "/";
    return conf_.get_location(tmp_path);
}

bool    HttpResponse::check_redirection(const locationConf& cfg) {
    const std::pair<int, std::string>& redir = cfg.get_redirect();
    
    if (redir.first != 0) {
        resp_buff_ = status_lines.at(redir.first);
        resp_buff_ += "Location: " + redir.second + "\r\n\r\n";
        resp_stat_ = DONE;
        return true;
    }
    return false;
}

const std::string HttpResponse::generateResponse() {
    int         status_code = request_->getParsingCode();

    resp_buff_.clear();
    std::cout << GRN_CLR << "Generating response ..." << DEF_CLR << std::endl;
    if (status_code == 400) {
        handle_error(400);
    } else if (status_code == 200) {
        t_method    method = request_->getMethod();
        std::string path = url_decode(request_->getTarget());
        path = resolve_path(path);
        const locationConf& location = identifyie_location(path);
        if (check_redirection(location)) {
            return resp_buff_;
        }
        if (method == GET) {
            responesForGet(location, path);
        } else if (method == DELETE) {
            responesForDelete(location, path);
        } else {  // this is for POST method, temporarily of course hhhh
            // std::cout << GRN_CLR << "200 OK" << DEF_CLR << std::endl;
            std::string html = "<!DOCTYPE html><html><body><h1>Hello from the WebServer!</h1></body></html>\n";
            resp_buff_ = "HTTP/1.1 200 OK\r\n"
                        "Content-Type: text/html\r\n"
                        "Content-Length: " + std::to_string(html.size()) + "\r\n"
                        "Connection: keep-alive\r\n"
                        "\r\n" + html;
        }
    } else if (status_code == 413) {
        handle_error(413);
    } else {
        std::cout << RED_CLR << status_code <<  " Internal Server Error!" << DEF_CLR << std::endl;
        handle_error(500);
    }
    return resp_buff_;
}

t_resp_state& HttpResponse::getRespState() {
    return resp_stat_;
}

std::string&   HttpResponse::getRespBuff() {
    return resp_buff_;
}

