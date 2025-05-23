/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   strMatchers.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kez-zoub <kez-zoub@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 21:38:37 by kez-zoub          #+#    #+#             */
/*   Updated: 2025/05/08 01:56:08 by kez-zoub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "httpParsingIncludes.hpp"

bool	OWSMatch(std::string str, size_t &pos)
{
	while (str[pos] == ' ' || str[pos] == '\t')
		pos++;
	return (true);
}

bool	tokenMatch(std::string str, size_t &pos)
{
	std::string	tchar = TCHAR;
	size_t	i = pos;

	while (tchar.find(str[i]) != std::string::npos)
		i++;
	if (i == pos)
		return (false);
	pos = i;
	return (true);
}

bool	quotedStringMatch(std::string str, size_t &pos)
{
	std::string	qdtext = QDTEXT;
	std::string	qpair = QPAIR;
	size_t	i = pos;

	if (str[i] != '"')
		return (false);
	i++;
	while (str[i] && str[i] != '"')
	{
		if (str[i] == '\\')
		{
			i++;
			if (qpair.find(str[i]) == std::string::npos)
				return (false);
		}
		else
		{
			if (qdtext.find(str[i]) == std::string::npos)
				return (false);
		}
		i++;
	}
	if (str[i] != '"')
		return (false);
	pos = i +1;
	return (true);
}

bool	qvalueMatch(std::string str, size_t &pos)
{
	std::string	digit = DIGIT;
	size_t	i = pos;

	if (str[i] != '0' && str[i] != '1')
		return (false);
	i++;
	if (str[i] == '.')
	{
		i++;
		if (str[i -2] == '0')
		{
			for (int i = 0; i < 3; i++)
			{
				if (digit.find(str[i]) == std::string::npos)
					break;
				i++;
			}
		}
		else
		{
			for (int i = 0; i < 3; i++)
			{
				if (str[i] != '0')
					break;
				i++;
			}
		}
	}
	pos = i;
	return (true);
}

bool	weightMatch(std::string str, size_t &pos)
{
	size_t	i = pos;
	OWSMatch(str, i);
	if (str[i] != ';')
		return (false);
	i++;
	OWSMatch(str, i);
	if (str[i] != 'q' || str[i +1] != '=')
		return (false);
	i += 2;
	if (!qvalueMatch(str, i))
		return (false);
	pos = i;
	return (true);
}

bool	parametersMatch(std::string str, size_t &pos)
{
	size_t	i = pos;
	OWSMatch(str, i);
	while (str[i] == ';')
	{
		i++;
		OWSMatch(str, i);
		// optional parameter
		if (tokenMatch(str, i))
		{
			if (str[i] != '=')
				return (false);
			i++;
			if (!quotedStringMatch(str, i) && !tokenMatch(str, i))
				return (false);
		}
		OWSMatch(str, i);
	}
	pos = i;
	return (true);
}

bool	languageRangeMatch(std::string str, size_t &pos)
{
	std::string	alpha = ALPHA;
	std::string	digit = DIGIT;
	size_t	i = pos;
	
	if (str[i] == '*')
		i++;
	else
	{
		int	j = 0;
		while (alpha.find(str[i]) != std::string::npos)
		{
			i++;
			j++;
		}
		if (j == 0 || j > 8)
			return (false);
		while (str[i] == '-')
		{
			i++;
			j = 0;
			while (alpha.find(str[i]) != std::string::npos || digit.find(str[i]) != std::string::npos)
			{
				i++;
				j++;
			}
			if (j == 0 || j > 8)
				return (false);
		}
	}
	pos = i;
	return (true);
}

bool	token68Match(std::string str, size_t &pos)
{
	std::string	token68 = TOKEN68;
	size_t	i = pos;

	if (token68.find(str[i]) == std::string::npos)
		return (false);
	while (token68.find(str[i]) != std::string::npos)
		i++;
	while (str[i] == '=')
		i++;
	pos = i;
	return (true);
}

bool	authParamListMatch(std::string str, size_t &pos)
{
	size_t	i = pos;

	while (str[i] && (i == pos || str[i] == ','))
	{
		if (str[i] == ',')
			i++;
		OWSMatch(str, i);
		if (!tokenMatch(str, i))
			return (false);
		OWSMatch(str, i);
		if (str[i] != '=')
			return (false);
		i++;
		OWSMatch(str, i);
		if (!quotedStringMatch(str, i) && !tokenMatch(str, i))
			return (false);
		OWSMatch(str, i);
	}
	pos = i;
	return (true);
}

bool	entityTagMatch(std::string str, size_t &pos)
{
	size_t	i = pos;
	std::string	etagc = ETAGC;

	if (!str.compare(i, 2, "W/"))
		i += 2;
	if (str[i] != '"')
		return (false);
	i++;
	while (etagc.find(str[i]) != std::string::npos)
		i++;
	if (str[i] != '"')
		return (false);
	pos = i +1;
	return (true);
}

bool	transferCodingMatch(std::string str, size_t &pos)
{
	size_t	i = pos;

	if (!tokenMatch(str, i))
		return (false);
	OWSMatch(str, i);
	while (str[i] == ';')
	{
		i++;
		OWSMatch(str, i);
		if (!tokenMatch(str, i))
			return (false);
		OWSMatch(str, i);
		if (str[i] != '=')
			return (false);
		i++;
		OWSMatch(str, i);
		if (!quotedStringMatch(str, i) && !tokenMatch(str, i))
			return (false);
		OWSMatch(str, i);
	}
	return (true);
}

bool	tokenListMatch(std::string str)
{
	std::size_t	pos = 0;

	while (str[pos] && (pos == 0 || str[pos] == ','))
	{
		if (str[pos] == ',')
			pos++;
		OWSMatch(str, pos);
		if (!tokenMatch(str, pos))
			return (false);
		OWSMatch(str, pos);
	}
	if (str[pos])
		return (false);
	return (true);
}
