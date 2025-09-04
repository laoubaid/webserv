/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequestParser.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kez-zoub <kez-zoub@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/12 23:00:03 by kez-zoub          #+#    #+#             */
/*   Updated: 2025/09/04 10:17:59 by kez-zoub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include.hpp"

std::map<std::string, validatorFunc>	Request::stdFields;
std::vector<std::string>				Request::invalidList;
Uvec	Request::DIGIT;
Uvec	Request::ALPHA;
Uvec	Request::CRLF;
Uvec	Request::HEXDIG;
Uvec	Request::UNRESERVED;
Uvec	Request::SUBDELIMS;
Uvec	Request::TCHAR;
Uvec	Request::VCHAR;
Uvec	Request::OBSTEXT;
Uvec	Request::QDTEXT;
Uvec	Request::QPAIR;

Request::StaticContainersInitializer	Request::initializer;

Request::StaticContainersInitializer::StaticContainersInitializer(void)
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

Request::Request(void)
{
}
Request::~Request(void)
{
	if (cgi)
	{
		delete cgi;
		std::remove(body_file_path.c_str());
	}
	// delete file if it is tmp
}

void	Request::badRequest(std::string err_msg)
{
	parsingCode = 400;
	std::cerr << RED_COLOR << "[BAD REQUEST][ERROR CODE 400]: " << err_msg << RESET_COLOR << std::endl;
}

void	Request::is_cgi(std::string cgi_dir, std::vector<std::string> extensions)
{
	std::string::iterator	start = target.path.begin();
	start++;
	std::string::iterator it = std::find(start, target.path.end(), '/');
	if (std::string(start, it) == cgi_dir) // cgi dir found
	{
		if (it == target.path.end() || it +1 == target.path.end()) // in case "/bin-dir" or "/bin-dir/"
			return ;
		cgi = new Cgi;
		it = std::find(it+1, target.path.end(), '/');
		cgi->set_script_name(std::string(target.path.begin(), it));
		if (it != target.path.end())
			it++;
		cgi->set_path_info(std::string(it, target.path.end()));
		return ;
	}
	it = std::find(target.path.begin(), target.path.end(), '.');
	if (it != target.path.end())
	{
		std::string::iterator	ext_end = std::find(it +1, target.path.end(), '/');
		std::string	ext = std::string(it+1, ext_end);
		for (std::vector<std::string>::iterator it = extensions.begin(); it < extensions.end(); it++)
		{
			if (*it == ext) // extension found
			{
				cgi = new Cgi;
				cgi->set_script_name(std::string(target.path.begin(), ext_end));
				if (ext_end != target.path.end())
					ext_end++;
				cgi->set_path_info(std::string(ext_end, target.path.end()));
				return ;
			}
		}
	}
}

void	Request::processStartLine(Uvec startLine)
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
	{
		target.raw = std::string(infos[1].begin(), infos[1].end());
		// process target and assign the the target struct
		Uvec::iterator	it = infos[1].find('?');
		target.path = decode_url(resolve_path(std::string(infos[1].begin(), it)));
		if (it != infos[1].end())
			it++; 
		target.query = std::string(it, infos[1].end());
		// check permission for upload
		std::vector<std::string>	ext = {"cgi", "py", "php"};
		is_cgi("cgi-dir", ext); // should throw exception in case the file doesn't exist or can't be run
	}
	else
		return (badRequest("invalid target"));
	// if (!(method | get.location()))
	// 	throw 405;
	// version validity
	if (infos[2] != Uvec((const unsigned char*)"HTTP/1.1", 8))
		return (badRequest("invalid http version"));
}

void	Request::addField(std::string key, Uvec value)
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

void	Request::processFields(std::vector<Uvec> lines)
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

void	Request::processTransferEncoding(const Uvec& transfer_encoding, const Uvec& raw_body)
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
			// add to file
			// creation of the file based on the purpose
			std::ofstream	file(body_file_path.c_str(), std::ios::binary | std::ios::app);
			file.write(reinterpret_cast<char *>(&transfer_encoding[0]), transfer_encoding.size());
			file.close();
			body_size += processed.second.size();
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

void	Request::processBody(const Uvec& raw_body)
{
	Uvec	transfer_encoding, content_length;
	bool	transfer_encoding_found = getFieldValue("transfer-encoding", transfer_encoding);
	bool	content_length_found = getFieldValue("content-length", content_length);

	if ((transfer_encoding_found || content_length_found) && (method == GET || method == DELETE))
	{
		parsingCode = 400;
		req_state = CCLS;
		throw std::runtime_error("bad request(400): method doesn't accept body");
	}
	if (transfer_encoding_found && content_length_found)
	{
		parsingCode = 400;
		req_state = CCLS;
		throw std::runtime_error("bad request(400): both transfer-encoding and content-length found");
	}
	// set up file
	if (method == POST)
	{
		if (cgi)
			body_file_path = "/tmp/webserv/webserv_cgi_body_[number of client's fd]";
		else
		{
			// check premission
				// upload allowed?
			
				// check if file exists
			std::string	file_path = "/tmp/webserv"; // root of the target path
			file_path += target.path;
			std::ifstream	i_file(file_path.c_str());
			if (i_file.good()) // i_ already exists
			{
				i_file.close();
				parsingCode = 400;
				req_state = CCLS;
				throw std::runtime_error("bad request(400): uploading existing file");
			}
			body_file_path = file_path;
			std::ofstream	o_file(body_file_path.c_str());
			if (!o_file.is_open()) // directory doesn't exist
			{
				parsingCode = 400;
				req_state = CCLS;
				throw std::runtime_error("bad request(400): uploading directory doesn't exist");
			}
			o_file.close();
		}
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

		// std::cerr << "length: " << length << ", body size: " << raw_body.size();
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
		// add to file
		std::ofstream	file(body_file_path.c_str(), std::ios::binary | std::ios::app);
		file.write(reinterpret_cast<char *>(&raw_body[0]), raw_body.size());
		file.close();
		body_size += raw_body.size();
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

void	Request::addBody(Uvec raw_body)
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
		// add to file
		std::ofstream	file(body_file_path.c_str(), std::ios::binary | std::ios::app);
		file.write(reinterpret_cast<char *>(&raw_body[0]), raw_body.size());
		file.close();
		body_size += raw_body.size();
		unsigned long  length;
		stringToUnsignedLong(std::string(content_length.begin(), content_length.end()), length);
		if (length > body_size)
			req_state = PEND;
		else if (length == body_size)
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
	if (cgi && req_state == RESP)
		cgi->run(*this);
}

Request::Request(Uvec httpRequest)
{
	std::cout << "Precessing request: " << std::string(httpRequest.begin(), httpRequest.end()) << std::endl;
	std::cout << "____________________________________________________________________________" << std::endl << std::endl;
	parsingCode = 200;
	body_size = 0;
	req_state = PEND;
	cgi = NULL;
	
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
	
	std::vector<Uvec>	lines = ft_split(headers, CRLF);
	
	// for (std::vector<Uvec>::iterator it = lines.begin(); it < lines.end(); it++)
	// {
	// 	std::cout << "vec: ";
	// 	(*it).print();
	// }
	
	if (lines.size() < 2) // the least that should be there are three lines (start-line, host header field at least, empty line)
	{
		parsingCode = 400;
		return;
	}
	processStartLine(lines[0]);
	if (parsingCode == 400)
		return ;
	
	// field line parsing:
	processFields(lines);

	processBody(rawBody);
	if (cgi && req_state == RESP)
		cgi->run(*this);
}

	// getter functions
int	Request::getParsingCode(void) const
{
	return (parsingCode);
}

const t_method&		Request::getMethod(void) const
{
	return (method);
}

const t_target&	Request::getTarget(void) const
{
	return (target);
}

const std::map<std::string, Uvec>&	Request::getFields(void) const
{
	return (fields);
}

bool		Request::getFieldValue(const std::string& key, Uvec& value) const
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

const t_req_state&		Request::getReqState(void) const
{
	return (req_state);
}

const std::string&			Request::getBodyFilePath(void) const
{
	return (body_file_path);
}

std::size_t			Request::getBodySize(void) const
{
	return (body_size);
}
