/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   httpParsingIncludes.hpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 21:35:31 by kez-zoub          #+#    #+#             */
/*   Updated: 2025/09/17 19:03:40 by laoubaid         ###   ########.fr       */
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
// # include "../cgi/Cgi.hpp"


# define RED_COLOR "\033[31m"
# define GREEN_COLOR "\033[32m"
# define RESET_COLOR "\033[0m"
// # define ALPHA "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
// # define DIGIT "0123456789"
// # define HEXDIG "0123456789ABCDEFabcdef"
# define HTAB "\t"
# define SP " "
# define CTL "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f\x7f"
// # define TCHAR "!#$%&'*+-.^_`|~0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
// # define VCHAR "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~"
# define TOKEN68 "-._~+/0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
// # define OBSTEXT "\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8a\x8b\x8c\x8d\x8e\x8f\x90\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9a\x9b\x9c\x9d\x9e\x9f\xa0\xa1\xa2\xa3\xa4\xa5\xa6\xa7\xa8\xa9\xaa\xab\xac\xad\xae\xaf\xb0\xb1\xb2\xb3\xb4\xb5\xb6\xb7\xb8\xb9\xba\xbb\xbc\xbd\xbe\xbf\xc0\xc1\xc2\xc3\xc4\xc5\xc6\xc7\xc8\xc9\xca\xcb\xcc\xcd\xce\xcf\xd0\xd1\xd2\xd3\xd4\xd5\xd6\xd7\xd8\xd9\xda\xdb\xdc\xdd\xde\xdf\xe0\xe1\xe2\xe3\xe4\xe5\xe6\xe7\xe8\xe9\xea\xeb\xec\xed\xee\xef\xf0\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8\xf9\xfa\xfb\xfc\xfd\xfe\xff"
// # define QDTEXT "\t !#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ]^`abcdefghijklmnopqrstuvwxyz{|}~\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8a\x8b\x8c\x8d\x8e\x8f\x90\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9a\x9b\x9c\x9d\x9e\x9f\xa0\xa1\xa2\xa3\xa4\xa5\xa6\xa7\xa8\xa9\xaa\xab\xac\xad\xae\xaf\xb0\xb1\xb2\xb3\xb4\xb5\xb6\xb7\xb8\xb9\xba\xbb\xbc\xbd\xbe\xbf\xc0\xc1\xc2\xc3\xc4\xc5\xc6\xc7\xc8\xc9\xca\xcb\xcc\xcd\xce\xcf\xd0\xd1\xd2\xd3\xd4\xd5\xd6\xd7\xd8\xd9\xda\xdb\xdc\xdd\xde\xdf\xe0\xe1\xe2\xe3\xe4\xe5\xe6\xe7\xe8\xe9\xea\xeb\xec\xed\xee\xef\xf0\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8\xf9\xfa\xfb\xfc\xfd\xfe\xff"
// # define QPAIR "\t !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~\x7f\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8a\x8b\x8c\x8d\x8e\x8f\x90\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9a\x9b\x9c\x9d\x9e\x9f\xa0\xa1\xa2\xa3\xa4\xa5\xa6\xa7\xa8\xa9\xaa\xab\xac\xad\xae\xaf\xb0\xb1\xb2\xb3\xb4\xb5\xb6\xb7\xb8\xb9\xba\xbb\xbc\xbd\xbe\xbf\xc0\xc1\xc2\xc3\xc4\xc5\xc6\xc7\xc8\xc9\xca\xcb\xcc\xcd\xce\xcf\xd0\xd1\xd2\xd3\xd4\xd5\xd6\xd7\xd8\xd9\xda\xdb\xdc\xdd\xde\xdf\xe0\xe1\xe2\xe3\xe4\xe5\xe6\xe7\xe8\xe9\xea\xeb\xec\xed\xee\xef\xf0\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8\xf9\xfa\xfb\xfc\xfd\xfe\xff"
# define ETAGC "!#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8a\x8b\x8c\x8d\x8e\x8f\x90\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9a\x9b\x9c\x9d\x9e\x9f\xa0\xa1\xa2\xa3\xa4\xa5\xa6\xa7\xa8\xa9\xaa\xab\xac\xad\xae\xaf\xb0\xb1\xb2\xb3\xb4\xb5\xb6\xb7\xb8\xb9\xba\xbb\xbc\xbd\xbe\xbf\xc0\xc1\xc2\xc3\xc4\xc5\xc6\xc7\xc8\xc9\xca\xcb\xcc\xcd\xce\xcf\xd0\xd1\xd2\xd3\xd4\xd5\xd6\xd7\xd8\xd9\xda\xdb\xdc\xdd\xde\xdf\xe0\xe1\xe2\xe3\xe4\xe5\xe6\xe7\xe8\xe9\xea\xeb\xec\xed\xee\xef\xf0\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8\xf9\xfa\xfb\xfc\xfd\xfe\xff"

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
// typedef std::vector<unsigned char> t_vec_uc;
typedef bool (*validatorFunc)(const Uvec &);
typedef struct s_target
{
	std::string	raw;
	std::string	path;
	std::string	query;
}	t_target;

// tests (to be deleted)
bool	testAllFields(void);

// tools
std::vector<Uvec>	ft_split(const Uvec &data, const Uvec &spliter);
// Uvec	str_to_vec(const unsigned char *str, std::size_t len);
// bool	ucharInVec(const Uvec &vec, unsigned char c);
// bool	vec_eq_str(const Uvec &vec, const unsigned char *str, std::size_t len);
Uvec	ft_trim(Uvec vec);
unsigned char toLowerChar(unsigned char c);
// Uvec createRange(unsigned char start, unsigned char end);
const Uvec&		getValue(const std::map<std::string, Uvec>& map, const std::string& key);
std::pair<unsigned long, Uvec>	process_chunked_body(Uvec data);
bool stringToUnsignedLong(const std::string& str, unsigned long& result);
std::string	ft_itos(std::size_t num);
std::string	decode_url(const std::string& url);
bool	has_shebang(std::string script_path);
char	*ft_strdup(const std::string& src);
std::string	get_ext(const std::string& path);
bool fileExists(const char* filename);

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

// testers to be deleted
void	printvec(const Uvec &vec, int printable);
bool	testIPv6address(const Uvec &vec);
bool	testIPvFuture(const Uvec &vec);
bool	testIPv4address(const Uvec &vec);
bool	testregName(const Uvec &vec);
bool	IPv6addressMatch(const Uvec &vec, Uvec::const_iterator &it);
bool	IPvFutureMatch(const Uvec &vec, Uvec::const_iterator &it);
bool	IPv4addressMatch(const Uvec &vec, Uvec::const_iterator &it);
bool	regNameMatch(const Uvec &vec, Uvec::const_iterator &it);

// to be refactored
// std::string resolve_path(const std::string& str);

// # include "../config/serverConf.hpp"

#endif