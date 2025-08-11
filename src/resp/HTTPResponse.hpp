/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/03 19:02:26 by laoubaid          #+#    #+#             */
/*   Updated: 2025/08/11 10:39:50 by laoubaid         ###   ########.fr       */
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

# define OK_200_ "HTTP/1.1 200 OK\r\n" \
            "Connection: keep-alive\r\n"

# define BADR_400_ "HTTP/1.1 400 Bad Request\r\n\r\n"

# define FORB_403_ "HTTP/1.1 403 FORBIDDEN\r\n" \
            "Content-Type: text/html\r\n" \
            "Connection: close\r\n" \
            "Content-Length: 64\r\n\r\n" \
            "<html><body><center><h1>403 Forbidden</h1><center></body></html>"

# define NOTF_404_ "HTTP/1.1 404 Not Found\r\n" \
            "Content-Type: text/html\r\n" \
            "Connection: close\r\n" \
            "Content-Length: 64\r\n\r\n" \
            "<html><body><center><h1>404 Not Found</h1><center></body></html>"

# define ELRG_413_ "HTTP/1.1 413 Entity Too Large\r\n" \
            "Content-Type: text/html\r\n" \
            "Content-Length: 72\r\n" \
            "Connection: close\r\n\r\n" \
            "<!DOCTYPE html><html><body><h1>413 Payload Too Large</h1></body></html>";

# define IERR_500_ "HTTP/1.1 500 Internal Server Error\r\n" \
            "Content-Type: text/html\r\n" \
            "Content-Length: 76\r\n" \
            "Connection: close\r\n\r\n" \
            "<html><body><center><h1>500 Internal Server Error</h1><center></body></html>";


// create a static class HTTPResponse, so i dont need to create an instance of it
// but i can use its methods to generate responses based on the status code

typedef enum e_resp_state {
    STRT,
    BODY,
    DONE
}	t_resp_state;

class HttpResponse {
    private:
        t_resp_state resp_stat_;
    public:
        HttpResponse() {
            resp_stat_ = STRT;
        }
        static std::string generateResponse(HTTPRequestParser &request);
        static std::string responesForGet(HTTPRequestParser &request);
        static const std::string& getMimeType(const std::string& ext);
        t_resp_state& getRespState();
};

#endif

