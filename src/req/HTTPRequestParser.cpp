/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequestParser.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/12 23:00:03 by kez-zoub          #+#    #+#             */
/*   Updated: 2025/09/26 02:21:58 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include.hpp"
#include "../config/serverConf.hpp"

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
	if (_cgi)
	{
		delete _cgi;
		std::cout << "[INFO] REQ removing the tmp file for body \n";
		std::remove(_body_file_path.c_str());
	}
	if (_file.is_open())
		_file.close();
}

int	Request::req_err(std::string throw_msg, int status_code, t_req_state state)
{
	_parsingCode = status_code;
	_req_state = state;
	if (throw_msg.size())
		throw std::runtime_error(throw_msg);
	if (_body_file_path.size())
		std::remove(_body_file_path.c_str());
	return (1);
}

void	Request::is_cgi()
{
	///////////////////// hard coded /////////////////////////////
	//	cgi_dir = "/cgi-dir/";                                  //
	//	extensions = {"cgi", "py", "php"};                      //
	//////////////////////////////////////////////////////////////
	// std::string		location = _loc->get_path(); //   /cgi-dir/scriptname.php/

	
	if (_loc->has_cgi())
	{
		std::cout << "[INFO] REQ location has cgi\n";
		std::string::iterator	a = _target.path.begin() + _loc->get_path().size();
		if (*a == '/')
			a++;
		std::string::iterator b = std::find(a, _target.path.end(), '/');
		std::string	filename(a, b);
		std::size_t	pos = filename.rfind('.');
		if (pos != std::string::npos)
		{
			std::string	ext = std::string(a + pos, b);
			if (_loc->valid_cgi_ext(ext)) // extension found
			{
				std::cout << "[INFO] REQ cgi ext found\n";
				_cgi = new Cgi(this, _loc->get_cgi_path(ext), _clt_addr);
				_cgi->set_script_name(std::string(_target.path.begin(), b));
				if (b != _target.path.end())
					b++;
				_cgi->set_path_info(std::string(b, _target.path.end()));
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
		req_err("invalid number of words in start line", 400, RESP);
	// method validity
	if (infos[0] == Uvec((const unsigned char*)"GET", 3))
		_method = GET;
	else if (infos[0] == Uvec((const unsigned char*)"POST", 4))
		_method = POST;
	else if (infos[0] == Uvec((const unsigned char*)"DELETE", 6))
		_method = DELETE;
	else
		req_err("method not supported", 501, RESP);
	// target validity
	if (validateTarget(infos[1]))
	{
		_target.raw = std::string(infos[1].begin(), infos[1].end());
		// process target and assign the the target struct
		Uvec::iterator	it = infos[1].find('?');
		_target.path = decode_url(resolve_path(std::string(infos[1].begin(), it)));
		if (it != infos[1].end())
			it++; 
		_target.query = std::string(it, infos[1].end());
		// check permission for upload
		_loc = &_conf->identifie_location(_target.path);
		is_cgi();
	}
	else
		req_err("invalid target", 400, RESP);

    if (!(_loc->get_methods() & _method)) 
		req_err("method not allowed", 405, RESP);

	// version validity
	if (!validateHttpV(infos[2]))
		req_err("invalid http version format", 400, RESP);
}

void	Request::addField(std::string key, Uvec value)
{
	std::pair<
		std::map<std::string, Uvec>::iterator,
		bool
	>	result = _fields.insert(std::make_pair(key, value));

	if (!result.second)
	{
		if (std::find(invalidList.begin(), invalidList.end(), key) != invalidList.end())
			req_err("header field doesn't qualify as a list", 400, RESP);
			// return (badRequest("header field doesn't qualify as a list"));
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
	for (std::size_t i = 1; i < lines.size() && lines[i].size() != 0; i++)
	{
		Uvec	key;
		
		Uvec::iterator	colon = lines[i].find(':');
		if (colon == lines[i].end())
			req_err("bad header field: colon not found", 400, RESP);
		for (Uvec::iterator it = lines[i].begin(); it != colon; it++)
		{
			key.push_back(tolower(*it));
		}
		
		Uvec	value = ft_trim(Uvec(colon +1, lines[i].end()));
		if (!validateFieldName(key))
			req_err("bad header field: invalid header field name", 400, RESP);
		std::string	str_key(key.begin(), key.end());
		std::map<std::string, validatorFunc>::iterator it = stdFields.find(str_key);
		if (it == stdFields.end())
		{
			if (!validateFieldValue(value))
				req_err("bad header field: invalid header field value (default)", 400, RESP);
		}
		else if (!it->second(value))
			req_err("bad header field: invalid header field value", 400, RESP);
		addField(str_key, value);
	}
}

void	Request::setup_body(const Uvec& raw_body)
{
	Uvec	transfer_encoding, content_length;
	bool	transfer_encoding_found = getFieldValue("transfer-encoding", transfer_encoding);
	bool	content_length_found = getFieldValue("content-length", content_length);

	if ((transfer_encoding_found || content_length_found) && (_method == GET || _method == DELETE))
		req_err("method doesn't accept body", 400, RESP);
	if (transfer_encoding_found && content_length_found)
		req_err("both transfer-encoding and content-length found", 400, RESP);
	// set up file
	if (_method == POST)
	{
		if (_cgi)
			_body_file_path = "/tmp/webserv_cgi_body_" + std::to_string(_client_fd);
		else
		{
			const locationConf& tmp = *_loc;
			std::string file_path = tmp.get_path();
			if (tmp.is_upset()) {
				int tmpidx = _target.path.rfind("/");
				file_path = _target.path.substr(tmpidx + 1);
				file_path = tmp.get_upstore() + "/" + file_path;
			} else 
				req_err("upload not set", 403, RESP); // ! what is the status code of this one???
		
			std::cout << "[INFO] REQ : " << file_path << std::endl; 
			std::ifstream	i_file(file_path.c_str());
			if (i_file.good()) // i_ already exists
			{
				i_file.close();
				req_err("uploading existing file", 409, RESP);
			}
			_body_file_path = file_path;
		}
		_file.open(_body_file_path.c_str(), std::ios::binary | std::ios::app);
		if (!_file.is_open()) // directory doesn't exist
			req_err("file can't be created", 403, RESP);
	}
	if (transfer_encoding_found)
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
			chunked = true;
		else
			req_err("body with transfer-encoding but no chunked", 400, RESP);
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
				req_err("content length list has different values", 400, RESP);
		}
		_fields["content-length"] = trimmed_vec;
		_content_length = length;
	}
	else
	{
		if (!raw_body.size())
			_req_state = RESP;
		else
			req_err("body with no content-length or transfer-encoding", 411, RESP);
	}
}

void	Request::processTransferEncoding(const Uvec& raw_body)
{
	try
	{
		std::pair<unsigned long, Uvec> processed = process_chunked_body(raw_body); // this function needs review
		// add to file
		std::ofstream	file(_body_file_path.c_str(), std::ios::binary | std::ios::app);
		if (processed.first)
			file.write(reinterpret_cast<char *>(&processed.second[0]), processed.second.size());
		file.close();
		_body_size += processed.first;
		if (processed.first) // if the body size isn't 0 then we still expecting chunks of the body
			_req_state = PEND;
		else
			_req_state = RESP;
	}
	catch(const std::exception& e)
	{
		req_err(std::string("invalid chunked body: ") + e.what(), 400, RESP);
	}
}

void	Request::addBody(Uvec raw_body)
{
	if (_chunked)
	{
		processTransferEncoding(raw_body);
	}
	else
	{
		// add to file
		// std::ofstream	file(_body_file_path.c_str(), std::ios::binary | std::ios::app);
		if (!_file.is_open())
			req_err("file closed unexpectedly", 403, RESP);
		if (raw_body.size())
			_file.write(reinterpret_cast<char *>(&raw_body[0]), raw_body.size());
		_body_size += raw_body.size();
		if (_content_length > _body_size)
			_req_state = PEND;
		else if (_content_length == _body_size)
			_req_state = RESP;
		else
			req_err("body received is bigger than the content-length", 400, RESP);
	}
	if (_cgi && _req_state == RESP) {
		if (_file.is_open())
			_file.close();
		_cgi->run();
	}
}

Request::Request(const serverConf& cfg, int fd_, int ep_fd, sockaddr_in clt_addr) :
_parsingCode(200), _chunked(false), _body_size(0), _req_state(PEND), _cgi(NULL), _conf(&cfg), _client_fd(fd_), _epoll_fd(ep_fd), _clt_addr(clt_addr)
{
	_loc = &_conf->identifie_location("/");
}

void Request::ParseRequest(Uvec httpRequest) {
	// conf_ = &cfg;
	// client_fd_ = fd_;
	std::cout << "Precessing request: " << std::string(httpRequest.begin(), httpRequest.end()) << std::endl;
	std::cout << "____________________________________________________________________________" << std::endl << std::endl;
	// parsingCode = 200;
	// body_size = 0;
	// req_state = PEND;
	// cgi = NULL;
	
	// split with crlfcrlf to get 2 (headers and body)
	Uvec	DCRLF((const unsigned char *)"\r\n\r\n", 4);
	Uvec::iterator	pos = httpRequest.find(DCRLF);

	if (pos == httpRequest.end())
		req_err("Req empty line not found", 400, RESP);
	
	Uvec	headers(httpRequest.begin(), pos);
	Uvec	rawBody(pos+4, httpRequest.end());
	
	std::vector<Uvec>	lines = ft_split(headers, CRLF);
	
	// for (std::vector<Uvec>::iterator it = lines.begin(); it < lines.end(); it++)
	// {
	// 	std::cout << "vec: ";
	// 	(*it).print();
	// }
	
	if (lines.size() < 2) // the least that should be there are three lines (start-line, host header field at least, empty line)
		req_err("start line or mandatory header not found", 400, RESP);
	
	processStartLine(lines[0]);
	// if (parsingCode == 400) {
	// 	req_state = RESP;
	// 	return ;
	// }
	
	// field line parsing:
	processFields(lines);

	setup_body(rawBody);

	if (rawBody.size())
		addBody(rawBody);
	if (_cgi && _req_state == RESP)
		_cgi->run();
}

	// getter functions
int	Request::getParsingCode(void) const
{
	return (_parsingCode);
}

const t_method&		Request::getMethod(void) const
{
	return (_method);
}

const t_target&	Request::getTarget(void) const
{
	return (_target);
}

const std::map<std::string, Uvec>&	Request::getFields(void) const
{
	return (_fields);
}

bool		Request::getFieldValue(const std::string& key, Uvec& value) const
{
	try
	{
		value = getValue(_fields, key);
		return (true);
	}
	catch(const std::exception& e)
	{
		return (false);
	}
}

const t_req_state&		Request::getReqState(void) const
{
	return (_req_state);
}

const std::string&			Request::getBodyFilePath(void) const
{
	return (_body_file_path);
}

std::size_t			Request::getBodySize(void) const
{
	return (_body_size);
}

void	Request::setParsingCode(int code)
{
	_parsingCode = code;
}

void	Request::setReqState(t_req_state state)
{
	_req_state = state;
}

int Request::get_cgi_pipe(int flag) {
	return _cgi ? _cgi->get_pipe(flag) : -1;
}