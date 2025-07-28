/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   host.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kez-zoub <kez-zoub@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/19 17:25:56 by kez-zoub          #+#    #+#             */
/*   Updated: 2025/06/24 04:13:50 by kez-zoub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../httpParsingIncludes.hpp"
#include "../HTTPRequestParser.hpp"

bool	decOctetMatch(const Uvec &vec, Uvec::const_iterator &it)
{
	std::size_t	i = 0;
	
	while (it +i != vec.end() && HTTPRequestParser::DIGIT.has(*(it +i)) && i < 4)
		i++;
	if (i == 0 || i > 3)
		return (false);
	int	num = std::atoi(std::string(it, it +i).c_str());
	if (num > 255)
		return (false);
	it += i;
	return (true);
}

bool	IPv4addressMatch(const Uvec &vec, Uvec::const_iterator &it)
{
	Uvec::const_iterator	i = it;

	if (!decOctetMatch(vec, i))
		return (false);
	for (int j = 0; j < 3; j++)
	{
		if (i == vec.end() || *i != '.')
			return (false);
		i++;
		if (!decOctetMatch(vec, i))
			return (false);
	}
	it = i;
	return (true);
}

bool	h16Match(const Uvec &vec, Uvec::const_iterator &it)
{
	std::size_t	i = 0;

	while (it +i != vec.end() && HTTPRequestParser::HEXDIG.has(*(it +i)) && i < 5)
		i++;
	if (i == 0 || i > 4)
		return (false);
	it += i;
	return (true);
}

bool	ls32Match(const Uvec &vec, Uvec::const_iterator &it)
{
	Uvec::const_iterator	i = it;
	
	if (!IPv4addressMatch(vec, i))
	{
		if (!h16Match(vec, i))
			return (false);
		if (i == vec.end() || *i != ':')
			return (false);
		i++;
		if (!h16Match(vec, i))
			return (false);
	}
	it = i;
	return (true);
}

bool	countLeftSide(const Uvec &vec, Uvec::const_iterator &i, int &left, int &ls32)
{
	if (h16Match(vec, i))
	{
		// std::cout << "enter\n";
		left++;
		while (i +1 < vec.end() && *i == ':' && *(i+1) != ':')
		{
			i++;
			// std::cout << str.substr(i) << "left: " << left << std::endl;
			if (IPv4addressMatch(vec, i))
			{
				// std::cout << "match\n";
				ls32++;
				break;
			}
			else if (!h16Match(vec, i))
			{
				return (false);
			}
			left++;
		}
	}
	return (true);
}

bool	countRightSide(const Uvec &vec, Uvec::const_iterator &i, int &right, int &ls32)
{
	// if (!ls32 && dcolon)
	if (!ls32)
	{
		if (IPv4addressMatch(vec, i))
			ls32++;
		else if (h16Match(vec, i))
		{
			right++;
			while (i +1 < vec.end() && *i == ':' && *(i+1) != ':')
			{
				i++;
				if (IPv4addressMatch(vec, i))
				{
					ls32++;
					break;
				}
				else if (!h16Match(vec, i))
				{
					return (false);
				}
				right++;
			}
		}
	}
	return (true);
}

bool	IPv6addressMatch(const Uvec &vec, Uvec::const_iterator &it)
{
	Uvec::const_iterator	i = it;
	int		left = 0;
	int		dcolon = 0;
	int		right = 0;
	int		ls32 = 0;

	if (!countLeftSide(vec, i, left, ls32))
		return (false);
	if (i +1 < vec.end() && *i == ':' && *(i+1) == ':')
	{
		dcolon++;
		i += 2;
	}
	if (!dcolon)
	{
		right = left;
		left = 0;
	}
	if (!countRightSide(vec, i, left, ls32))
		return (false);
	if (!ls32 && right >= 2)
	{
		ls32++;
		right -= 2;
	}
	// std::cout << "left: " << left << ", dcolon: " << dcolon << ", right: " << right << ", ls32:" << ls32 << std::endl;
	if (
		(!left && !dcolon && right == 6 && ls32) ||
		(!left && dcolon && right == 5 && ls32) ||
		(left >= 0 && left <= 1 && dcolon && right == 4 && ls32) ||
		(left >= 0 && left <= 2 && dcolon && right == 3 && ls32) ||
		(left >= 0 && left <= 3 && dcolon && right == 2 && ls32) ||
		(left >= 0 && left <= 4 && dcolon && right == 1 && ls32) ||
		(left >= 0 && left <= 5 && dcolon && !right && ls32) ||
		(left >= 0 && left <= 6 && dcolon && right == 1 && !ls32) ||
		(left >= 0 && left <= 7 && dcolon && !right && !ls32)
	)
	{
		it = i;
		return (true);
	}
	return (false);
}

bool	IPvFutureMatch(const Uvec &vec, Uvec::const_iterator &it)
{
	Uvec::const_iterator	i = it;
	int							j = 0;
	
	if (i != vec.end() && *i != 'v' && *i != 'V')
		return (false);
	i++;
	while (i != vec.end() && HTTPRequestParser::HEXDIG.has(*i))
	{
		i++;
		j++;
	}
	if (!j)
		return (false);
	j = 0;
	if (i != vec.end() && *i != '.')
		return (false);
	i++;
	while (	i != vec.end() &&
			(HTTPRequestParser::UNRESERVED.has(*i) ||
			HTTPRequestParser::SUBDELIMS.has(*i) ||
			*i == ':'))
	{
		i++;
		j++;
	}
	if (!j)
		return (false);
	it = i;
	return (true);
}

bool	IPLiteralMatch(const Uvec &vec, Uvec::const_iterator &it)
{
	Uvec::const_iterator	i = it;

	if (i != vec.end() && *i != '[')
		return (false);
	i++;
	if (!IPvFutureMatch(vec, i) && !IPv6addressMatch(vec, i))
		return (false);
	if (i != vec.end() && *i != ']')
		return (false);
	it = i +1;
	return (true);
}

bool	regNameMatch(const Uvec &vec, Uvec::const_iterator &it)
{
	Uvec::const_iterator	i = it;

	while (true)
	{
		if (i != vec.end() && (HTTPRequestParser::UNRESERVED.has(*i) || HTTPRequestParser::SUBDELIMS.has(*i)))
			i++;
		else if (!pctEncodedMatch(vec, i))
			break ;
	}
	it = i;
	return (true);
}

bool	hostMatch(const Uvec &vec, Uvec::const_iterator &it)
{
	Uvec::const_iterator	i = it;
	
	if (!IPLiteralMatch(vec, i) && !IPv4addressMatch(vec, i) && !regNameMatch(vec, i))
		return (false);
	it = i;
	return (true);
}

bool	portMatch(const Uvec &vec, Uvec::const_iterator &it)
{
	while (it != vec.end() && HTTPRequestParser::DIGIT.has(*it))
		it++;
	return (true);
}