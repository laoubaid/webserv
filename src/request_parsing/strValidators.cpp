/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   strValidators.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kez-zoub <kez-zoub@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 17:53:41 by kez-zoub          #+#    #+#             */
/*   Updated: 2025/05/08 02:14:16 by kez-zoub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "httpParsingIncludes.hpp"

bool	validateFieldName(std::string str)
{
	std::size_t	pos = 0;

	if (!tokenMatch(str, pos))
		return (false);
	if (str[pos])
		return (false);
	return (true);
}

bool	validateFieldValue(std::string str)
{
	std::size_t	pos = 0;
	std::string	fieldVChar = std::string(VCHAR) + OBSTEXT;

	while (fieldVChar.find(str[pos]) != std::string::npos)
	{
		pos++;
		while (str[pos] == ' ' || str[pos] == '\t')
			pos++;
	}
	if (str[pos] || (pos && (str[pos -1] == ' ' || str[pos -1] == '\t')))
		return (false);
	return (true);
}

bool	validateA_IM(std::string str)
{
	std::size_t	pos = 0;

	while (str[pos] && (pos == 0 || str[pos] == ','))
	{
		if (str[pos] == ',')
			pos++;
		OWSMatch(str, pos);
		if (!tokenMatch(str, pos))
			return (false);
		if (str[pos] == ';')
		{
			pos++;
			if (str[pos] == 'q' && str[pos +1] == '=')
			{
				pos += 2;
				if (!qvalueMatch(str, pos))
					return (false);
			}
			else
			{
				if (!tokenMatch(str, pos))
					return (false);
				if (str[pos] == '=')
				{
					pos++;
					if (!quotedStringMatch(str, pos) && !tokenMatch(str, pos))
						return (false);
				}
			}
		}
		OWSMatch(str, pos);
	}
	if (str[pos])
		return (false);
	return (true);
}

bool	validateAccept(std::string str)
{
	std::size_t	pos = 0;

	while (str[pos] && (pos == 0 || str[pos] == ','))
	{
		if (str[pos] == ',')
			pos++;
		OWSMatch(str, pos);
		if (!tokenMatch(str, pos))
			return (false);
		if (str[pos] != '/')
			return (false);
		pos++;
		if (!tokenMatch(str, pos))
			return (false);
		if (!parametersMatch(str, pos))
			return (false);
		OWSMatch(str, pos);
			//this will never run I think
			if (str[pos] == ';')
			{
				if (!weightMatch(str, pos))
					return (false);
			}
		OWSMatch(str, pos);
	}
	if (str[pos])
		return (false);
	return (true);
}

bool	validateAcceptCharset(std::string str)
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
		if (str[pos] == ';')
		{
			if (!weightMatch(str, pos))
				return (false);
		}
		OWSMatch(str, pos);
	}
	if (str[pos])
		return (false);
	return (true);
}

bool	validateAcceptEncoding(std::string str) // same code as validateAcceptCharset
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
		if (str[pos] == ';')
		{
			if (!weightMatch(str, pos))
				return (false);
		}
		OWSMatch(str, pos);
	}
	if (str[pos])
		return (false);
	return (true);
}

bool	validateAcceptLanguage(std::string str)
{
	std::size_t	pos = 0;

	while (str[pos] && (pos == 0 || str[pos] == ','))
	{
		if (str[pos] == ',')
			pos++;
		OWSMatch(str, pos);
		if (!languageRangeMatch(str, pos))
			return (false);
		OWSMatch(str, pos);
		if (str[pos] == ';')
		{
			if (!weightMatch(str, pos))
				return (false);
		}
		OWSMatch(str, pos);
	}
	if (str[pos])
		return (false);
	return (true);
}

bool	validateAuthorization(std::string str)
{
	std::size_t	pos = 0;

	if (!tokenMatch(str, pos))
		return (false);
	if (str[pos] == ' ')
	{
		while (str[pos] == ' ')
			pos++;
		if (!authParamListMatch(str, pos) && !token68Match(str, pos))
			return (false);
	}
	if (str[pos])
		return (false);
	return (true);
}

bool	validateCacheControl(std::string str)
{
	std::size_t	pos = 0;

	while (str[pos] && (pos == 0 || str[pos] == ','))
	{
		if (str[pos] == ',')
			pos++;
		OWSMatch(str, pos);
		if (!tokenMatch(str, pos))
			return (false);
		if (str[pos] == '=')
		{
			pos++;
			if (!quotedStringMatch(str, pos) && !tokenMatch(str, pos))
				return (false);
		}
		OWSMatch(str, pos);
	}
	if (str[pos])
		return (false);
	return (true);
}

bool	validateConnection(std::string str)
{
	return (tokenListMatch(str));
}

bool	validateContentEncoding(std::string str)
{
	return (tokenListMatch(str));
}

bool	validateContentLength(std::string str)
{
	std::size_t	pos = 0;
	std::string	digit = DIGIT;

	while (digit.find(str[pos]) != std::string::npos)
		pos++;
	if (str[pos] || pos == 0)
		return (false);
	return (true);
}

bool	validateContentType(std::string str)
{
	std::size_t	pos = 0;

	if (!tokenMatch(str, pos))
		return (false);
	if (str[pos] != '/')
		return (false);
	pos++;
	if (!tokenMatch(str, pos))
		return (false);
	if (!parametersMatch(str, pos))
		return (false);
	if (str[pos])
		return (false);
	return (true);
}

bool	validateExpect(std::string str)
{
	std::size_t	pos = 0;

	while (str[pos] && (pos == 0 || str[pos] == ','))
	{
		if (str[pos] == ',')
			pos++;
		OWSMatch(str, pos);
		if (!tokenMatch(str, pos))
			return (false);
		if (str[pos] == '=')
		{
			pos++;
			if (!quotedStringMatch(str, pos) && !tokenMatch(str, pos))
				return (false);
			if (!parametersMatch(str, pos))
				return (false);
		}
		OWSMatch(str, pos);
	}
	if (str[pos])
		return (false);
	return (true);
}

bool	validateIfMatch(std::string str)
{
	size_t	pos = 0;
	
	if (str == "*")
		return (true);
	while (str[pos] && (pos == 0 || str[pos] == ','))
	{
		if (str[pos] == ',')
			pos++;
		OWSMatch(str, pos);
		if (!entityTagMatch(str, pos))
			return (false);
		OWSMatch(str, pos);
	}
	if (str[pos])
		return (false);
	return (true);
}

bool	validateMaxForwards(std::string str)
{
	return (validateContentLength(str));
}

bool	validateProxyAuthorization(std::string str)
{
	return (validateAuthorization(str));
}

bool	validateTE(std::string str)
{
	std::size_t	pos = 0;

	while (str[pos] && (pos == 0 || str[pos] == ','))
	{
		if (str[pos] == ',')
			pos++;
		OWSMatch(str, pos);
		if (!str.compare(pos, 8, "trailers"))
			pos += 8;
		else
		{
			if (!transferCodingMatch(str, pos))
				return (false);

				//this will never run I think
				OWSMatch(str, pos);
				if (str[pos] == ';')
				{
					if (!weightMatch(str, pos))
						return (false);
				}
		}
		OWSMatch(str, pos);
	}
	if (str[pos])
		return (false);
	return (true);
}

bool	validateTrailer(std::string str)
{
	return (tokenListMatch(str));
}

bool	validateTransferEncoding(std::string str)
{
	std::size_t	pos = 0;

	while (str[pos] && (pos == 0 || str[pos] == ','))
	{
		if (str[pos] == ',')
			pos++;
		OWSMatch(str, pos);
		if (!transferCodingMatch(str, pos))
			return (false);
		OWSMatch(str, pos);
	}
	if (str[pos])
		return (false);
	return (true);
}
