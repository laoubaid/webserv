/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequestParserTools.cpp                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kez-zoub <kez-zoub@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/13 20:28:30 by kez-zoub          #+#    #+#             */
/*   Updated: 2025/05/03 19:07:10 by kez-zoub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTPRequestParser.hpp"

// to distinguish either the spliter is at the end or not, I will add an empty string to the array if it is at the end
std::vector<std::string>	ft_split(const std::string &str, const std::string &spliter)
{
	std::vector<std::string>	str_vec;
	std::size_t start = 0;
    std::size_t pos = str.find(spliter);

	while (pos != std::string::npos)
	{
		// std::cout << "pos: " << pos << std::endl;
		// std::cout << "start: " << start << std::endl;
		// std::cout << "str to push: " << str.substr(start, pos - start) << std::endl;
		str_vec.push_back(str.substr(start, pos -start));
		start = pos + spliter.size();
		pos = str.find(spliter, start);
	}
	str_vec.push_back(str.substr(start));
	return (str_vec);
}

bool	valid_dig(std::string str)
{
	// std::cout << "check dig: " << str;
	std::string dig = "0123456789";
	
	for (std::size_t i = 0; str[i]; i++)
	{
		if (dig.find(str[i]) == std::string::npos)
			return (false);
	}
	return (true);
}

bool	valid_target(std::string target)
{
	int	offset = 1;
	std::string	hexdig = "0123456789ABCDEFabcdef";
	std::string	unreserved = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz-._~";
	std::string	subdelims = "!$&'()*+,;=";
	std::string	others = ":@/?";

	if (target[0] != '/')
		return (false);
	if (target[1] == '/')
		return (false);
	for (; target[offset]; offset++)
	{
		if (target[offset] == '%')
		{
			offset++;
			if (hexdig.find(target[offset]) == std::string::npos)
				return (false);
			offset++;
			if (hexdig.find(target[offset]) == std::string::npos)
				return (false);
			continue ;
		}
		if (	unreserved.find(target[offset]) == std::string::npos
			|| 	subdelims.find(target[offset]) == std::string::npos
			|| 	others.find(target[offset]) == std::string::npos)
			return (false);
	}
	return (true);
	// rules:
	// 	only origin form is valid in our project (because we only working with 3 methods that require only origin form (3.2.1. origin-form rfc9112)) and its rules are:
	// 	"invalid request-line SHOULD respond with either a 400 (Bad Request) error" (3.2. Request Target rfc9112)
	// 	starts with /
	// 	origin-form = absolute-path [ "?" query ] (3.2.1. origin-form rfc9112) [] means optional and "?" means exactly that char, so the query is optional
	// 		for the absolute path: (all below from (3.3.  Path rfc3986))
	// 			The path is terminated by the first question mark ("?") or number sign ("#") character, or by the end of the URI 
	// 			path-absolute   ; begins with "/" but not "//"
	// 			path-absolute = "/" [ segment-nz *( "/" segment ) ]    (this rule mean the first segment should be just / or /segment/... never starts with // but // can be found in the rest of the path)
	// 			segment       = *pchar (means zero or more pchars )
    // 			segment-nz    = 1*pchar (means at least one character)
	// 			pchar         = unreserved / pct-encoded / sub-delims / ":" / "@"
	// 				unreserved  = ALPHA / DIGIT / "-" / "." / "_" / "~" (rfc3986#section-2.3)
	// 				pct-encoded = "%" HEXDIG HEXDIG (The uppercase hexadecimal digits 'A' through 'F' are equivalent to the lowercase digits 'a' through 'f' => means it's case insensitive) (rfc3986#section-2.1)
	// 				sub-delims  = "!" / "$" / "&" / "'" / "(" / ")" / "*" / "+" / "," / ";" / "=" (rfc3986#section-2.2)
	// 		for the query (rfc3986#section-3.4)
	// 			The query component is indicated by the first question mark ("?") character and terminated by a number sign ("#") character or by the end of the URI
	// 			 query       = *( pchar / "/" / "?" )     (means the query can be empty or include any number of characters from the pchar set, as well as / and ?) (The asterisk * denotes that the sequence can be of any length, including zero, which means an empty query is syntactically valid)
	// summary:
	// 	should start with '/'
	// 	should not start with "//"
	// 	all characters should be *( pchar / "/" / "?" ) until the end of the string	
}

std::string	ft_trim(std::string str)
{
	int	start = 0;

	while (str[start] == ' ')
		start++;
	int	end = str.size() -1;
	while (str[end] == ' ')
		end--;
	return (str.substr(start, end -start +1));
}

std::string	str_tolower(std::string str)
{
	std::string	newStr = "";

	for (size_t i = 0; str[i]; i++)
	{
		newStr += std::tolower(str[i]);
	}
	return (newStr);
}
