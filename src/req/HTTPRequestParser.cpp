/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequestParser.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/12 23:00:03 by kez-zoub          #+#    #+#             */
/*   Updated: 2025/08/11 18:21:24 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTPRequestParser.hpp"

std::map<std::string, validatorFunc>	HTTPRequestParser::stdFields;
std::vector<std::string>				HTTPRequestParser::invalidList;
Uvec	HTTPRequestParser::DIGIT;
Uvec	HTTPRequestParser::ALPHA;
Uvec	HTTPRequestParser::CRLF;
Uvec	HTTPRequestParser::HEXDIG;
Uvec	HTTPRequestParser::UNRESERVED;
Uvec	HTTPRequestParser::SUBDELIMS;
Uvec	HTTPRequestParser::TCHAR;
Uvec	HTTPRequestParser::VCHAR;
Uvec	HTTPRequestParser::OBSTEXT;
Uvec	HTTPRequestParser::QDTEXT;
Uvec	HTTPRequestParser::QPAIR;

HTTPRequestParser::StaticContainersInitializer	HTTPRequestParser::initializer;

HTTPRequestParser::StaticContainersInitializer::StaticContainersInitializer(void)
{
	// std::cout << "initializer called\n";

	// initialize standard fields with their appropriate validators
	stdFields["content-length"] = validateContentLength;
	stdFields["host"] = validateHost;
	stdFields["transfer-encoding"] = validateTransferEncoding;

	invalidList.push_back("host");

	// Assign vectors for nomalized characters (try to make adding vectors better)
	CRLF = Uvec((const unsigned char*)"\r\n", 2);
	DIGIT = Uvec((const unsigned char*)"0123456789", 10);
	ALPHA = Uvec((const unsigned char*)"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz", 52);
	HEXDIG = Uvec((const unsigned char*)"ABCDEFabcdef", 12) + DIGIT;
	UNRESERVED = Uvec((const unsigned char*)"-._~", 4) + DIGIT;
	UNRESERVED += ALPHA;
	SUBDELIMS = Uvec((const unsigned char*)"!$&'()*+,;=", 11);
	TCHAR = Uvec((const unsigned char*)"!#$%&'*+-.^_`|~", 15) + DIGIT;
	TCHAR += ALPHA;
	VCHAR = Uvec(0x21, 0x7e);
	OBSTEXT = Uvec(0x80, 0xff);
	QDTEXT = Uvec((const unsigned char*)"\t !#$%&'()*+,-./:;<=>?@]^`{|}~", 30);
	QDTEXT += DIGIT;
	QDTEXT += ALPHA;
	QDTEXT += OBSTEXT;
	QPAIR = Uvec((const unsigned char*)"\t !\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~\x7f", 35);
	QPAIR += DIGIT;
	QPAIR += ALPHA;
	QPAIR += OBSTEXT;
}

HTTPRequestParser::HTTPRequestParser(void)
{
}
HTTPRequestParser::~HTTPRequestParser(void)
{
}

void	HTTPRequestParser::badRequest(std::string err_msg)
{
	parsingCode = 400;
	std::cerr << RED_COLOR << "[BAD REQUEST][ERROR CODE 400]: " << err_msg << RESET_COLOR << std::endl;
}

void	HTTPRequestParser::processStartLine(Uvec startLine)
{
	Uvec	sp;
	sp.push_back(' ');
	std::vector<Uvec>	infos = ft_split(startLine, sp);
	
	if (infos.size() != 3)
		return (badRequest("invalid number of words in start line"));
	// method validity
	if (infos[0] == Uvec((const unsigned char*)"GET", 3))
		method = GET;
	else if (infos[0] == Uvec((const unsigned char*)"POST", 4))
		method = POST;
	else if (infos[0] == Uvec((const unsigned char*)"DELETE", 6))
		method = DELETE;
	else
	{
		parsingCode = 501;
		return;
	}
	// target validity
	if (validateTarget(infos[1]))
		target = std::string(infos[1].begin(), infos[1].end());
	else
		return (badRequest("invalid target"));
	// version validity
	if (infos[2] != Uvec((const unsigned char*)"HTTP/1.1", 8))
		return (badRequest("invalid http version"));
}

void	HTTPRequestParser::addField(std::string key, Uvec value)
{
	std::pair<
		std::map<std::string, Uvec>::iterator,
		bool
	>	result = fields.insert(std::make_pair(key, value));

	if (!result.second)
	{
		if (std::find(invalidList.begin(), invalidList.end(), key) != invalidList.end())
			return (badRequest("header field doesn't qualify as a list"));
		else
		{
			Uvec	sep((const unsigned char*)" , ", 3);
			result.first->second += sep;
			result.first->second += value;
		}
	}
}

void	HTTPRequestParser::processFields(std::vector<Uvec> lines)
{
	
	
	for (int i = 1; i < lines.size() && lines[i].size() != 0; i++)
	{
		Uvec	key;
		
		// t_vec_uc::iterator	colon = std::find(lines[i].begin(), lines[i].end(), ':');
		Uvec::iterator	colon = lines[i].find(':');
		if (colon == lines[i].end())
			return (badRequest("colon not found"));
		// std::transform(lines[i].begin(), colon, std::back_inserter(key), toLowerChar); // does it work???
		for (Uvec::iterator it = lines[i].begin(); it != colon; it++)
		{
			key.push_back(tolower(*it));
		}
		
		Uvec	value = ft_trim(Uvec(colon +1, lines[i].end()));
		if (!validateFieldName(key))
			return (badRequest("header field name not valid"));
		std::string	str_key(key.begin(), key.end());
		std::map<std::string, validatorFunc>::iterator it = stdFields.find(str_key);
		if (it == stdFields.end())
		{
			if (!validateFieldValue(value))
				return (badRequest("default header field value not valid"));
		}
		else if (!it->second(value))
			return (badRequest("header field value not valid"));
		addField(str_key, value);
	}
}

void	HTTPRequestParser::processTransferEncoding(const Uvec& transfer_encoding, const Uvec& raw_body)
{
	std::vector<Uvec>	transfer_encoding_list = ft_split(transfer_encoding, Uvec((const unsigned char *)",", 1));
	int	chunked = 0;
	Uvec	chunked_vec = Uvec((const unsigned char *)"chunked", 7);
	for (std::vector<Uvec>::iterator it = transfer_encoding_list.begin(); it != transfer_encoding_list.end(); it++)
	{
		if (ft_trim(*it) == chunked_vec)
		{
			chunked = 1;
			break;
		}
	}
	if (chunked)
	{
		if (!raw_body.size())
		{
			req_state = PEND;
			return;
		}
		try
		{
			std::pair<unsigned long, Uvec> processed = process_chunked_body(raw_body); // this function needs review
			body += processed.second;
			if (processed.first) // if the body size isn't 0 then we still expecting chunks of the body
				req_state = PEND;
			else
				req_state = RESP;
		}
		catch(const std::exception& e)
		{
			// std::cerr << "HERE " << e.what() << "\n";
			parsingCode = 400;
			req_state = CCLS;
			throw std::runtime_error("invalid chunked size number");
		}
	}
	else
	{
		parsingCode = 400;
		req_state = CCLS;
		throw std::runtime_error("body with transfer-encoding but no chunked");
	}
}

void	HTTPRequestParser::processBody(const Uvec& raw_body)
{
	Uvec	transfer_encoding, content_length;
	bool	transfer_encoding_found = getFieldValue("transfer-encoding", transfer_encoding);
	bool	content_length_found = getFieldValue("content-length", content_length);

	if (transfer_encoding_found && content_length_found)
	{
		parsingCode = 400;
		req_state = CCLS;
		throw std::runtime_error("bad request(400): both transfer-encoding and content-length found");
	}
	if (transfer_encoding_found)
	{
		processTransferEncoding(transfer_encoding, raw_body);
	}
	else if (content_length_found)
	{
		std::vector<Uvec>	lengths = ft_split(content_length, Uvec((const unsigned char *)",", 1));
		Uvec	trimmed_vec = ft_trim(lengths[0]);
		unsigned long  length;
		stringToUnsignedLong(std::string(trimmed_vec.begin(), trimmed_vec.end()), length);
		for (std::vector<Uvec>::iterator it = lengths.begin() +1; it != lengths.end(); it++)
		{
			trimmed_vec = ft_trim(*it);
			unsigned long	v;
			stringToUnsignedLong(std::string(trimmed_vec.begin(), trimmed_vec.end()), v);
			if (v != length)
				return (badRequest("content length list has different values"));
		}
		fields["content-length"] = trimmed_vec;

		std::cerr << "length: " << length << ", body size: " << raw_body.size();
		if (length > raw_body.size())
			req_state = PEND;
		else if (length == raw_body.size())
			req_state = RESP;
		else
		{
			parsingCode = 400;
			req_state = CCLS;
			throw std::runtime_error("content-length not valid");
		}
		body = raw_body;
	}
	else
	{
		if (!raw_body.size())
			req_state = RESP;
		else
		{
			parsingCode = 411;
			throw std::runtime_error("body with no content-length or transfer-encoding");
		}
	}
}

void	HTTPRequestParser::addBody(Uvec raw_body)
{
	Uvec	transfer_encoding, content_length;
	bool	transfer_encoding_found = getFieldValue("transfer-encoding", transfer_encoding);
	bool	content_length_found = getFieldValue("content-length", content_length);

	if (transfer_encoding_found)
	{
		processTransferEncoding(transfer_encoding, raw_body);
	}
	else if (content_length_found)
	{
		body += raw_body;
		unsigned long  length;
		stringToUnsignedLong(std::string(content_length.begin(), content_length.end()), length);
		if (length > body.size())
			req_state = PEND;
		else if (length == body.size())
			req_state = RESP;
		else
		{
			parsingCode = 400;
			req_state = CCLS;
			throw std::runtime_error("content-length not valid");
		}
	}
	else
	{
		parsingCode = 411;
		throw std::runtime_error("body with no content-length or transfer-encoding");
	}
}

HTTPRequestParser::HTTPRequestParser(Uvec httpRequest)
{
	// std::cout << "Precessing request: " << std::string(httpRequest.begin(), httpRequest.end()) << std::endl;
	// std::cout << "____________________________________________________________________________" << std::endl << std::endl;
	parsingCode = 200;
	
	// split with crlfcrlf to get 2 (headers and body)
	Uvec	DCRLF((const unsigned char *)"\r\n\r\n", 4);
	Uvec::iterator	pos = httpRequest.find(DCRLF);

	if (pos == httpRequest.end())
	{
		parsingCode = 400;
		return ;
	}
	Uvec	headers(httpRequest.begin(), pos);
	Uvec	rawBody(pos+4, httpRequest.end());

	// std::cout << "headers: ";
	// headers.print();
	// std::cout << "rawBody: ";
	// rawBody.print();
	std::vector<Uvec>	lines = ft_split(headers, CRLF);
	
	// for (std::vector<Uvec>::iterator it = lines.begin(); it < lines.end(); it++)
	// {
	// 	std::cout << "vec: ";
	// 	(*it).print();
	// }
	req_state = CCLS;
	if (lines.size() < 2) // the least that should be there are three lines (start-line, host header field at least, empty line)
	{
		parsingCode = 400;
		// std::cout << "HERE\n";
		return;
	}
	processStartLine(lines[0]);
	if (parsingCode == 400)
		return ;

	// field line parsing:
	processFields(lines);

	// std::cout << "rawBody: ";
	// rawBody.print();

	processBody(rawBody);
}

	// getter functions
int	HTTPRequestParser::getParsingCode(void) const
{
	return (parsingCode);
}

const t_method&		HTTPRequestParser::getMethod(void) const
{
	return (method);
}

const std::string&	HTTPRequestParser::getTarget(void) const
{
	return (target);
}

const std::map<std::string, Uvec>&	HTTPRequestParser::getFields(void) const
{
	return (fields);
}

bool		HTTPRequestParser::getFieldValue(const std::string& key, Uvec& value) const
{
	try
	{
		value = getValue(fields, key);
		return (true);
	}
	catch(const std::exception& e)
	{
		return (false);
	}
}

const t_req_state&		HTTPRequestParser::getReqState(void) const
{
	return (req_state);
}

const Uvec&			HTTPRequestParser::getBody(void) const
{
	return (body);
}
