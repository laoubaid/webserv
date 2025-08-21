/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/03 19:02:26 by laoubaid          #+#    #+#             */
/*   Updated: 2025/08/18 17:52:35 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

# include "../include.hpp"
# include "../req/HTTPRequestParser.hpp"

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
        
    public:
        HttpResponse(HTTPRequestParser *request) {
            resp_stat_ = STRT;
            request_ = request;
        }
        const std::string   generateResponse();
        void                responesForGet();
        void                responesForDelete();

        bool    read_file_continu();

        void    process_path(std::string& path);
        bool    serveStaticContent(const std::string& path);
        bool    list_directory(const std::string& path);

        void    delete_file(std::string& path);

        static const std::string& getMimeType(const std::string& ext);
        t_resp_state&   getRespState();
        std::string&    getRespBuff();
};

#endif

