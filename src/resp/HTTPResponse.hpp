/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/03 19:02:26 by laoubaid          #+#    #+#             */
/*   Updated: 2025/08/08 18:03:59 by laoubaid         ###   ########.fr       */
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

