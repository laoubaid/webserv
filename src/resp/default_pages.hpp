/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   default_pages.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/23 17:11:44 by laoubaid          #+#    #+#             */
/*   Updated: 2025/09/23 17:31:32 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DEF_PAGES_HPP
# define DEF_PAGES_HPP

# define OK_200_ "HTTP/1.1 200 OK\r\n" \
			"Connection: close\r\n"

#define CRTD_201_ "HTTP/1.1 201 Created\r\n" \
			"Connection: close\r\n\r\n"

#define NOCNT_204_ "HTTP/1.1 204 No Content\r\n" \
			"Connection: close\r\n\r\n"

# define BADR_400_ "HTTP/1.1 400 Bad Request\r\n\r\n"

# define UNAUTH_401_ "HTTP/1.1 401 Unauthorized\r\n" \
			"Content-Type: text/html\r\n" \
			"Connection: close\r\n" \
			"Content-Length: 70\r\n\r\n" \
			"<html><body><center><h1>401 Unauthorized</h1></center></body></html>\n"

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

# define METN_405_ "HTTP/1.1 405 Method Not Allowed\r\n" \
			"Content-Type: text/html\r\n" \
			"Connection: close\r\n" \
			"Content-Length: 74\r\n\r\n" \
			"<html><body><center><h1>405 Method Not Allowed</h1><center></body></html>\n"

# define RTIM_408_ "HTTP/1.1 408 Request Timeout\r\n" \
			"Content-Type: text/html\r\n" \
			"Connection: close\r\n" \
			"Content-Length: 74\r\n\r\n" \
			"<html><body><center><h1>408 Request Timeout</h1></center></body></html>\n"

# define CONFL_409_ "HTTP/1.1 409 Conflict\r\n" \
			"Content-Type: text/html\r\n" \
			"Connection: close\r\n" \
			"Content-Length: 67\r\n\r\n" \
			"<html><body><center><h1>409 Conflict</h1></center></body></html>\n"

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

# define NIMP_501_ "HTTP/1.1 501 Not Implemented\r\n" \
			"Content-Type: text/html\r\n" \
			"Content-Length: 71\r\n" \
			"Connection: close\r\n\r\n" \
			"<html><body><center><h1>501 Not Implemented</h1><center></body></html>\n";

# define BGAT_502_ "HTTP/1.1 502 Bad Gateway\r\n" \
			"Content-Type: text/html\r\n" \
			"Connection: close\r\n" \
			"Content-Length: 68\r\n\r\n" \
			"<html><body><center><h1>502 Bad Gateway</h1></center></body></html>\n"

# define SUNA_503_ "HTTP/1.1 503 Service Unavailable\r\n" \
			"Content-Type: text/html\r\n" \
			"Connection: close\r\n" \
			"Content-Length: 79\r\n\r\n" \
			"<html><body><center><h1>503 Service Unavailable</h1></center></body></html>\n"

# define GTIM_504_ "HTTP/1.1 504 Gateway Timeout\r\n" \
			"Content-Type: text/html\r\n" \
			"Connection: close\r\n" \
			"Content-Length: 72\r\n\r\n" \
			"<html><body><center><h1>504 Gateway Timeout</h1></center></body></html>\n"

#endif