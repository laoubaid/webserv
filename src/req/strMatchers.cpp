/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   strMatchers.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kez-zoub <kez-zoub@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 21:38:37 by kez-zoub          #+#    #+#             */
/*   Updated: 2025/10/17 10:37:33 by kez-zoub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "httpParsingIncludes.hpp"
#include "HTTPRequestParser.hpp"

bool	pctEncodedMatch(const Uvec &vec, Uvec::const_iterator &it)
{
	Uvec::const_iterator	i = it;

	if (i == vec.end() || *i != '%')
		return (false);
	i++;
	if (i == vec.end() || !Request::HEXDIG.has(*i))
		return (false);
	i++;
	if (i == vec.end() || !Request::HEXDIG.has(*i))
		return (false);
	it = i +1;
	return (true);
}

bool	pcharMatch(const Uvec &vec, Uvec::const_iterator &it)
{
	Uvec::const_iterator	i = it;

	if (	i != vec.end() && 
		(	Request::UNRESERVED.has(*i) || 
			Request::SUBDELIMS.has(*i) ||
			*i == ':' || *i == '@'))
		i++;
	else if (i != vec.end() &&!pctEncodedMatch(vec, i))
		return (false);
	it = i;
	return (true); 
}

bool	absolutePathMatch(const Uvec &vec, Uvec::const_iterator &it)
{
	Uvec::const_iterator	i = it;
	int					counter = 0;
	
	while (i != vec.end() && *i == '/')
	{
		counter++;
		i++;
		Uvec::const_iterator	j = i;
		while (pcharMatch(vec, i) && j != i)
			j = i;
	}
	if (!counter)
		return (false);
	it = i;
	return (true);
}

bool	queryMatch(const Uvec &vec, Uvec::const_iterator &it)
{
	Uvec::const_iterator	i = it;

	while (i != vec.end())
	{
		if (*i ==  '/' || *i == '?')
			i++;
		else if (!pcharMatch(vec, i))
			break ;
	}
	it = i;
	return (true);
}

bool	OWSMatch(const Uvec &vec, Uvec::const_iterator &it)
{
	while (it != vec.end() && (*it == ' ' || *it == '\t'))
		it++;
	return (true);
}


bool	tokenMatch(const Uvec &vec, Uvec::const_iterator &it)
{
	Uvec::const_iterator	i = it;

	while (i != vec.end() && Request::TCHAR.has(*i))
		i++;
	if (i == it)
		return (false);
	it = i;
	return (true);
}

bool	quotedStringMatch(const Uvec &vec, Uvec::const_iterator &it)
{
	Uvec::const_iterator	i = it;

	if (i == vec.end() || *i != '"')
		return (false);
	i++;
	while (i != vec.end() && *i != '"')
	{
		if (*i == '\\')
		{
			i++;
			if (i == vec.end() || !Request::QPAIR.has(*i))
				return (false);
		}
		else
		{
			if (i == vec.end() || !Request::QDTEXT.has(*i))
				return (false);
		}
		i++;
	}
	if (i == vec.end() || *i != '"')
		return (false);
	it = i +1;
	return (true);
}

bool	transferCodingMatch(const Uvec &vec, Uvec::const_iterator &it)
{
	Uvec::const_iterator	i = it;

	if (!tokenMatch(vec, i))
		return (false);
	OWSMatch(vec, i);
	while (i != vec.end() && *i == ';')
	{
		i++;
		OWSMatch(vec, i);
		if (!tokenMatch(vec, i))
			return (false);
		OWSMatch(vec, i);
		if (i == vec.end() || *i != '=')
			return (false);
		i++;
		OWSMatch(vec, i);
		if (!quotedStringMatch(vec, i) && !tokenMatch(vec, i))
			return (false);
		OWSMatch(vec, i);
	}
	it = i;
	return (true);
}

