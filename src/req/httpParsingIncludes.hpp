/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   httpParsingIncludes.hpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 21:35:31 by kez-zoub          #+#    #+#             */
/*   Updated: 2025/11/01 09:58:42 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPPARSINGINCLUDES_HPP
# define HTTPPARSINGINCLUDES_HPP

# include <iostream>
# include <vector>
# include <map>
# include <algorithm>
# include <stdexcept>
# include <sstream>
# include <fstream>
# include <cstring>
# include <cstdio>
# include <sys/stat.h>
# include "../uvec/Uvec.hpp"

# define CGI_BUFFER 4096

typedef enum e_method
{
	DELETE = 1 << 0,
	POST = 1 << 1,
	GET = 1 << 2,
}	t_method;
typedef enum e_req_state {
    PEND,
	CGI,
	ACGI,
    RESP,
}	t_req_state;
typedef enum e_cgi_state {
	IDLE,
	WRITE,
	READ,
	END
}	t_cgi_state;

typedef bool (*validatorFunc)(const Uvec &);
typedef struct s_target
{
	std::string	raw;
	std::string	path;
	std::string	query;
}	t_target;

// tools
std::vector<Uvec>	ft_split(const Uvec &data, const Uvec &spliter);
Uvec	ft_trim(Uvec vec);
unsigned char toLowerChar(unsigned char c);
const Uvec&		getValue(const std::map<std::string, Uvec>& map, const std::string& key);
std::pair<unsigned long, Uvec>	process_chunked_body(Uvec data);
bool stringToUnsignedLong(const std::string& str, unsigned long& result);
std::string	ft_itos(std::size_t num);
std::string	decode_url(const std::string& url);
char	*ft_strdup(const std::string& src);
std::string	get_ext(const std::string& path);
bool fileExists(const char* filename);
bool hexStringToUnsignedLong(const std::string& hexStr, unsigned long& result);

//vec matchers
bool	pctEncodedMatch(const Uvec &vec, Uvec::const_iterator &it);
bool	absolutePathMatch(const Uvec &vec, Uvec::const_iterator &it);
bool	queryMatch(const Uvec &vec, Uvec::const_iterator &it);
bool	tokenMatch(const Uvec &vec, Uvec::const_iterator &it);
bool	OWSMatch(const Uvec &vec, Uvec::const_iterator &it);
bool	hostMatch(const Uvec &vec, Uvec::const_iterator &it);
bool	portMatch(const Uvec &vec, Uvec::const_iterator &it);
bool	transferCodingMatch(const Uvec &vec, Uvec::const_iterator &it);

// vec validators
bool		validateTarget(const Uvec &vec);
bool	validateHttpV(const Uvec &vec);
bool	validateFieldName(const Uvec &vec);
bool	validateFieldValue(const Uvec &vec);
bool	validateContentLength(const Uvec &vec);
bool	validateHexDigit(const Uvec &vec);
bool	validateHost(const Uvec &vec);
bool	validateTransferEncoding(const Uvec &vec);

bool	IPv6addressMatch(const Uvec &vec, Uvec::const_iterator &it);
bool	IPvFutureMatch(const Uvec &vec, Uvec::const_iterator &it);
bool	IPv4addressMatch(const Uvec &vec, Uvec::const_iterator &it);
bool	regNameMatch(const Uvec &vec, Uvec::const_iterator &it);

#endif
