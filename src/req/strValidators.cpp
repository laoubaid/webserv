/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   strValidators.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kez-zoub <kez-zoub@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 17:53:41 by kez-zoub          #+#    #+#             */
/*   Updated: 2025/10/30 21:24:49 by kez-zoub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "requestIncludes.hpp"
#include "Request.hpp"

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

bool	validateHttpV(const Uvec &vec)
{
	Uvec::const_iterator	it = vec.begin();

	if (vec.size() != 8)
		return (false);
	Uvec	http((const unsigned char*)"HTTP/", 5);
	if (Uvec(vec.begin(), vec.begin() +5) != http)
		return (false);
	it += 5;
	if (!Request::DIGIT.has(*it))
		return (false);
	it++;
	if (*it != '.')
		return (false);
	it++;
	if (!Request::DIGIT.has(*it))
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

bool	validateTransferEncoding(const Uvec &vec)
{
	Uvec::const_iterator	it = vec.begin();

	while (it != vec.end() && (it == vec.begin() || *it == ','))
	{
		if (*it == ',' && it != vec.begin())
			it++;
		OWSMatch(vec, it);
		if (!transferCodingMatch(vec, it))
			return (false);
		OWSMatch(vec, it);
	}
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
