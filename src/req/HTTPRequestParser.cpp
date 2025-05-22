/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequestParser.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/12 23:00:03 by kez-zoub          #+#    #+#             */
/*   Updated: 2025/05/13 06:00:52 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTPRequestParser.hpp"

std::map<std::string, validatorFunc>	HTTPRequestParser::stdFields;

void	HTTPRequestParser::fieldsInit(void)
{
	stdFields["accept"] = validateAccept;
	stdFields["accept-charset"] = validateAcceptCharset;
	stdFields["accept-encoding"] = validateAcceptEncoding;
}

HTTPRequestParser::HTTPRequestParser(void)
{
}

void	HTTPRequestParser::processStartLine(std::string startLine)
{
	std::vector<std::string>	infos = ft_split(startLine, " ");
	
	if (infos.size() != 3)
	{
		parsingCode = 400;
		return;
	}
	// method validity
	if (infos[0] == "GET")
		method = GET;
	else if (infos[0] == "POST")
		method = POST;
	else if (infos[0] == "DELETE")
		method = DELETE;
	else
	{
		parsingCode = 405;
		return;
	}
	// target validity
	if (!valid_target(infos[2]))
		requestTarget = infos[2];
	else
	{
		parsingCode = 400;
		return;
	}
	// version validity
	if (infos[2] == "HTTP/1.1")
		HTTPVersion = "HTTP/1.1";
	else
		parsingCode = 400;
}

void	HTTPRequestParser::processFields(std::vector<std::string> lines)
{
	std::string	key;
	std::string	value;
	std::size_t	colon;
	int			i = 1;
	// std::map<std::string, valid>::iterator it;
	fieldsInit();
	
	for (; i < lines.size() && lines[i] != ""; i++)
	{
		// std::cout << "here\n";
		colon = lines[i].find(':');
		if (colon == std::string::npos)
		{
			parsingCode = 400;
			return ;
		}
		key = str_tolower(lines[i].substr(0, colon));
		value = ft_trim(lines[i].substr(colon +1));
		if (!validateFieldName(key))
		{
			parsingCode = 400;
			return ;
		}
		std::map<std::string, validatorFunc>::iterator it = stdFields.find(key);
		if (it == stdFields.end())
		{
			if (!validateFieldValue(value))
			{
				parsingCode = 400;
				return ;
			}
		}
		else if (!it->second(value))
		{
			parsingCode = 400;
			return ;
		}
		// should check if the feild already in map to add it as a list or throw error if it doesn't accept multiple lists
		fields[key] = value;
	}
	if (i >= lines.size() -1 || lines[i] != "" || lines[i+1] != "")
		parsingCode = 400;
}

HTTPRequestParser::HTTPRequestParser(std::string httpRequest)
{
	std::cout << "Precessing request: " << httpRequest << std::endl;
	parsingCode = 200;
	std::vector<std::string>	lines = ft_split(httpRequest, "\r\n");
	
	if (lines.size() < 3)
	{
		parsingCode = 400;
		return;
	}
	processStartLine(lines[0]);
	if (parsingCode == 400)
		return ;

	// field line parsing:
		// field-line   = field-name ":" OWS field-value OWS (ows: optional whitespace) (can put as mean spaces as you want) (5. Field Syntax rfc9112)
		// No whitespace is allowed between the field name and colon (if it happens response status code of 400 (Bad Request)) (5.1. Field Line Parsing rfc9112)
		// there's some field names that can be repeated (like: Accept, Cache-Control, Connection... can be merged using commas exept cookies with ;)
			// empl: Accept: text/html\r\nAccept: application/json  => Accept: text/html, application/json
		// Host field is mandatory "A client MUST send a Host header field in all HTTP/1.1 request messages" "A server MUST respond with a 400 (Bad Request) status code to any HTTP/1.1 request message that lacks a Host header field and to any request message that contains more than one Host header field line or a Host header field with an invalid field value" (rfc9112#name-request-target)
	processFields(lines);



	if (parsingCode == 200)
		std::cout << "\n\n[VALID HTTP REQUEST]\n\n";
	else
		std::cout << "\n\n[INVALID HTTP REQUEST]\n\n";
}

int	HTTPRequestParser::getParsingCode(void) const
{
	return (parsingCode);
}

HTTPRequestParser::~HTTPRequestParser(void)
{
}
