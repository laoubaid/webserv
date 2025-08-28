/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/03 19:02:26 by laoubaid          #+#    #+#             */
/*   Updated: 2025/08/28 00:16:12 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

# include "../include.hpp"
# include "../req/HTTPRequestParser.hpp"
# include "../config/serverConf.hpp"
// # include "../config/serverConf.hpp"

# include <sys/stat.h>
# include <sys/types.h>
// # include <sys/syscall.h>
# include <dirent.h>

# include <fstream>

# define OK_200_ "HTTP/1.1 200 OK\r\n" \
            "Connection: keep-alive\r\n"

# define BADR_400_ "HTTP/1.1 400 Bad Request\r\n\r\n"

# define FORB_403_ "HTTP/1.1 403 FORBIDDEN\r\n" \
            "Content-Type: text/html\r\n" \
            "Connection: close\r\n" \
            "Content-Length: 65\r\n\r\n" \
            "<html><body><center><h1>403 Forbidden</h1><center></body></html>\n"

# define NOTF_404_ "HTTP/1.1 404 Not Found\r\n" \
            "Content-Type: text/html\r\n" \
            "Connection: close\r\n" \
            "Content-Length: 65\r\n\r\n" \
            "<html><body><center><h1>404 Not Found</h1><center></body></html>\n"

# define ELRG_413_ "HTTP/1.1 413 Entity Too Large\r\n" \
            "Content-Type: text/html\r\n" \
            "Content-Length: 73\r\n" \
            "Connection: close\r\n\r\n" \
            "<!DOCTYPE html><html><body><h1>413 Payload Too Large</h1></body></html>\n";

# define IERR_500_ "HTTP/1.1 500 Internal Server Error\r\n" \
            "Content-Type: text/html\r\n" \
            "Content-Length: 77\r\n" \
            "Connection: close\r\n\r\n" \
            "<html><body><center><h1>500 Internal Server Error</h1><center></body></html>\n";

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
        t_resp_state        resp_stat_;
        HTTPRequestParser   *request_;
        std::fstream        file_;
        std::string         resp_buff_;
        const serverConf&   conf_;

        
    public:
        static std::map<int, std::string> status_lines;
    
        HttpResponse(HTTPRequestParser *request, const serverConf&   conf) : conf_(conf) {
            resp_stat_ = STRT;
            request_ = request;
        }
        const std::string   generateResponse();
        void                responesForGet(const locationConf& location, std::string& path);
        void                responesForDelete(const locationConf& location, std::string& path);

        bool    read_file_continu();

        void    handle_error(int err_code);

        void    process_path(const locationConf& location, std::string& path);
        bool    serveStaticContent(const std::string& path, int code);
        bool    list_directory(const std::string& path);

        const locationConf& identifyie_location(const std::string& str);
        bool                check_redirection(const locationConf& cfg);

        static const std::string& getMimeType(const std::string& ext);
        t_resp_state&   getRespState();
        std::string&    getRespBuff();
};

size_t              get_file_size(std::fstream &file);
bool                is_directory(const std::string& path);
std::string         url_decode(const std::string& str);
std::string         resolve_path(const std::string& str);
// const locationConf& identifyie_location(const std::string& str, const serverConf& cfg);
// bool                check_redirection(const std::string& str, const locationConf& cfg);

void init_status_lines();

#endif
