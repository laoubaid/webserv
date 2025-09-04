/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   strValidators.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kez-zoub <kez-zoub@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 17:53:41 by kez-zoub          #+#    #+#             */
/*   Updated: 2025/08/19 23:51:33 by kez-zoub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "httpParsingIncludes.hpp"
#include "HTTPRequestParser.hpp"

bool	validateTarget(const Uvec &vec)
{
	Uvec::const_iterator	it = vec.begin();

	if (!absolutePathMatch(vec, it))
		return (false);
	while (it != vec.end() && *it == '?')
	{
		it++;
		queryMatch(vec, it);
	}
	if (it != vec.end())
		return (false);
	return (true);
}

bool	validateFieldName(const Uvec &vec)
{
	Uvec::const_iterator	it = vec.begin();

	if (!tokenMatch(vec, it))
		return (false);
	if (it != vec.end())
		return (false);
	return (true);
}

bool	validateFieldValue(const Uvec &vec)
{
	Uvec::const_iterator	it = vec.begin();
	
	while (it != vec.end() && (Request::VCHAR.has(*it) || Request::OBSTEXT.has(*it)))
	{
		it++;
		while (it != vec.end() && (*it == ' ' || *it == '\t'))
			it++;
	}
	if (it != vec.end() || (it != vec.begin() && (*(it -1) == ' ' || *(it -1) == '\t')))
		return (false);
	return (true);
}

bool	validateHexDigit(const Uvec &vec)
{
	Uvec::const_iterator	it = vec.begin();

	while (it != vec.end() && Request::HEXDIG.has(*it))
		it++;
	if (it != vec.end() || it == vec.begin())
		return (false);
	return (true);
}

// bool	validateA_IM(std::string str)
// {
// 	std::size_t	pos = 0;

// 	while (str[pos] && (pos == 0 || str[pos] == ','))
// 	{
// 		if (str[pos] == ',')
// 			pos++;
// 		OWSMatch(str, pos);
// 		if (!tokenMatch(str, pos))
// 			return (false);
// 		if (str[pos] == ';')
// 		{
// 			pos++;
// 			if (str[pos] == 'q' && str[pos +1] == '=')
// 			{
// 				pos += 2;
// 				if (!qvalueMatch(str, pos))
// 					return (false);
// 			}
// 			else
// 			{
// 				if (!tokenMatch(str, pos))
// 					return (false);
// 				if (str[pos] == '=')
// 				{
// 					pos++;
// 					if (!quotedStringMatch(str, pos) && !tokenMatch(str, pos))
// 						return (false);
// 				}
// 			}
// 		}
// 		OWSMatch(str, pos);
// 	}
// 	if (str[pos])
// 		return (false);
// 	return (true);
// }

// bool	validateAccept(std::string str)
// {
// 	std::size_t	pos = 0;

// 	while (str[pos] && (pos == 0 || str[pos] == ','))
// 	{
// 		if (str[pos] == ',')
// 			pos++;
// 		OWSMatch(str, pos);
// 		if (!tokenMatch(str, pos))
// 			return (false);
// 		if (str[pos] != '/')
// 			return (false);
// 		pos++;
// 		if (!tokenMatch(str, pos))
// 			return (false);
// 		if (!parametersMatch(str, pos))
// 			return (false);
// 		OWSMatch(str, pos);
// 			//this will never run I think
// 			if (str[pos] == ';')
// 			{
// 				if (!weightMatch(str, pos))
// 					return (false);
// 			}
// 		OWSMatch(str, pos);
// 	}
// 	if (str[pos])
// 		return (false);
// 	return (true);
// }

// bool	validateAcceptCharset(std::string str)
// {
// 	std::size_t	pos = 0;

// 	while (str[pos] && (pos == 0 || str[pos] == ','))
// 	{
// 		if (str[pos] == ',')
// 			pos++;
// 		OWSMatch(str, pos);
// 		if (!tokenMatch(str, pos))
// 			return (false);
// 		OWSMatch(str, pos);
// 		if (str[pos] == ';')
// 		{
// 			if (!weightMatch(str, pos))
// 				return (false);
// 		}
// 		OWSMatch(str, pos);
// 	}
// 	if (str[pos])
// 		return (false);
// 	return (true);
// }

// bool	validateAcceptEncoding(std::string str) // same code as validateAcceptCharset
// {
// 	std::size_t	pos = 0;

// 	while (str[pos] && (pos == 0 || str[pos] == ','))
// 	{
// 		if (str[pos] == ',')
// 			pos++;
// 		OWSMatch(str, pos);
// 		if (!tokenMatch(str, pos))
// 			return (false);
// 		OWSMatch(str, pos);
// 		if (str[pos] == ';')
// 		{
// 			if (!weightMatch(str, pos))
// 				return (false);
// 		}
// 		OWSMatch(str, pos);
// 	}
// 	if (str[pos])
// 		return (false);
// 	return (true);
// }

// bool	validateAcceptLanguage(std::string str)
// {
// 	std::size_t	pos = 0;

// 	while (str[pos] && (pos == 0 || str[pos] == ','))
// 	{
// 		if (str[pos] == ',')
// 			pos++;
// 		OWSMatch(str, pos);
// 		if (!languageRangeMatch(str, pos))
// 			return (false);
// 		OWSMatch(str, pos);
// 		if (str[pos] == ';')
// 		{
// 			if (!weightMatch(str, pos))
// 				return (false);
// 		}
// 		OWSMatch(str, pos);
// 	}
// 	if (str[pos])
// 		return (false);
// 	return (true);
// }

// bool	validateAuthorization(std::string str)
// {
// 	std::size_t	pos = 0;

// 	if (!tokenMatch(str, pos))
// 		return (false);
// 	if (str[pos] == ' ')
// 	{
// 		while (str[pos] == ' ')
// 			pos++;
// 		if (!authParamListMatch(str, pos) && !token68Match(str, pos))
// 			return (false);
// 	}
// 	if (str[pos])
// 		return (false);
// 	return (true);
// }

// bool	validateCacheControl(std::string str)
// {
// 	std::size_t	pos = 0;

// 	while (str[pos] && (pos == 0 || str[pos] == ','))
// 	{
// 		if (str[pos] == ',')
// 			pos++;
// 		OWSMatch(str, pos);
// 		if (!tokenMatch(str, pos))
// 			return (false);
// 		if (str[pos] == '=')
// 		{
// 			pos++;
// 			if (!quotedStringMatch(str, pos) && !tokenMatch(str, pos))
// 				return (false);
// 		}
// 		OWSMatch(str, pos);
// 	}
// 	if (str[pos])
// 		return (false);
// 	return (true);
// }

// bool	validateConnection(std::string str)
// {
// 	return (tokenListMatch(str));
// }

// bool	validateContentEncoding(std::string str)
// {
// 	return (tokenListMatch(str));
// }

bool	validateContentLength(const Uvec &vec)
{
	Uvec::const_iterator	it = vec.begin();

	while (it != vec.end() && (it == vec.begin() || *it == ','))
	{
		if (*it == ',' && it != vec.begin())
			it++;
		OWSMatch(vec, it);
		size_t	counter = 0;
		while (it != vec.end() && Request::DIGIT.has(*it))
		{
			it++;
			counter++;
		}
		if (!counter)
			return (false);
		OWSMatch(vec, it);
	}
	if (it != vec.end() || it == vec.begin())
		return (false);
	return (true);
}

// bool	validateContentType(std::string str)
// {
// 	std::size_t	pos = 0;

// 	if (!tokenMatch(str, pos))
// 		return (false);
// 	if (str[pos] != '/')
// 		return (false);
// 	pos++;
// 	if (!tokenMatch(str, pos))
// 		return (false);
// 	if (!parametersMatch(str, pos))
// 		return (false);
// 	if (str[pos])
// 		return (false);
// 	return (true);
// }

// bool	validateExpect(std::string str)
// {
// 	std::size_t	pos = 0;

// 	while (str[pos] && (pos == 0 || str[pos] == ','))
// 	{
// 		if (str[pos] == ',')
// 			pos++;
// 		OWSMatch(str, pos);
// 		if (!tokenMatch(str, pos))
// 			return (false);
// 		if (str[pos] == '=')
// 		{
// 			pos++;
// 			if (!quotedStringMatch(str, pos) && !tokenMatch(str, pos))
// 				return (false);
// 			if (!parametersMatch(str, pos))
// 				return (false);
// 		}
// 		OWSMatch(str, pos);
// 	}
// 	if (str[pos])
// 		return (false);
// 	return (true);
// }

// bool	validateIfMatch(std::string str)
// {
// 	size_t	pos = 0;
	
// 	if (str == "*")
// 		return (true);
// 	while (str[pos] && (pos == 0 || str[pos] == ','))
// 	{
// 		if (str[pos] == ',')
// 			pos++;
// 		OWSMatch(str, pos);
// 		if (!entityTagMatch(str, pos))
// 			return (false);
// 		OWSMatch(str, pos);
// 	}
// 	if (str[pos])
// 		return (false);
// 	return (true);
// }

// bool	validateMaxForwards(std::string str)
// {
// 	return (validateContentLength(str));
// }

// bool	validateProxyAuthorization(std::string str)
// {
// 	return (validateAuthorization(str));
// }

// bool	validateTE(std::string str)
// {
// 	std::size_t	pos = 0;

// 	while (str[pos] && (pos == 0 || str[pos] == ','))
// 	{
// 		if (str[pos] == ',')
// 			pos++;
// 		OWSMatch(str, pos);
// 		if (!str.compare(pos, 8, "trailers"))
// 			pos += 8;
// 		else
// 		{
// 			if (!transferCodingMatch(str, pos))
// 				return (false);

// 				//this will never run I think
// 				OWSMatch(str, pos);
// 				if (str[pos] == ';')
// 				{
// 					if (!weightMatch(str, pos))
// 						return (false);
// 				}
// 		}
// 		OWSMatch(str, pos);
// 	}
// 	if (str[pos])
// 		return (false);
// 	return (true);
// }

// bool	validateTrailer(std::string str)
// {
// 	return (tokenListMatch(str));
// }

bool	validateTransferEncoding(const Uvec &vec)
{
	Uvec::const_iterator	it = vec.begin();

	while (it != vec.end() && (it == vec.begin() || *it == ','))
	{
		if (*it == ',' && it != vec.begin())
			it++;
		OWSMatch(vec, it);
		// if transfercodingmatch false (return false)
		if (!transferCodingMatch(vec, it))
			return (false);
		OWSMatch(vec, it);
	}
	// vec.print();
	if (it != vec.end() || it == vec.begin())
		return (false);
	return (true);
}

bool	validateHost(const Uvec &vec)
{
	Uvec::const_iterator	it = vec.begin();

	if (!hostMatch(vec, it))
		return (false);
	if (it != vec.end() && *it == ':')
	{
		it++;
		portMatch(vec, it);
	}
	if (it != vec.end())
		return (false);
	return (true);
}

//to be deleted
bool	testIPv6address(const Uvec &vec)
{
	Uvec::const_iterator	it = vec.begin();

	if (!IPv6addressMatch(vec, it))
		return (false);
	if (it != vec.end())
		return (false);
	return (true);
}

bool	testIPvFuture(const Uvec &vec)
{
	Uvec::const_iterator	it = vec.begin();

	if (!IPvFutureMatch(vec, it))
		return (false);
	if (it != vec.end())
		return (false);
	return (true);
}

bool	testIPv4address(const Uvec &vec)
{
	Uvec::const_iterator	it = vec.begin();

	if (!IPv4addressMatch(vec, it))
		return (false);
	if (it != vec.end())
		return (false);
	return (true);
}

bool	testregName(const Uvec &vec)
{
	Uvec::const_iterator	it = vec.begin();

	if (!regNameMatch(vec, it))
		return (false);
	if (it != vec.end())
		return (false);
	return (true);
}