/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequestParser.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kez-zoub <kez-zoub@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/12 23:00:07 by kez-zoub          #+#    #+#             */
/*   Updated: 2025/09/11 22:32:18 by kez-zoub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUESTPARSER_HPP
# define HTTPREQUESTPARSER_HPP

# include "httpParsingIncludes.hpp"

class Cgi;
class serverConf;
class locationConf;

class Request
{
	private:
		static std::map<std::string, validatorFunc>	stdFields;
		static std::vector<std::string>				invalidList;

		

		struct StaticContainersInitializer
		{
			StaticContainersInitializer();
		};
		static  StaticContainersInitializer	initializer;

		int									_parsingCode;
		t_method							_method;
		t_target							_target;
		std::map<std::string, Uvec>			_fields;
		bool								_chunked;
		std::size_t							_content_length;
		std::size_t							_body_size;
		std::string							_body_file_path;
		t_req_state							_req_state;
		Cgi									*_cgi;
		const serverConf					*_conf;
		const locationConf					*_loc;
		int									_client_fd;
		
		void								processStartLine(Uvec startLine);
		void								processFields(std::vector<Uvec> lines);
		void								addField(std::string key, Uvec value);
		int									req_err(std::string throw_msg, int status_code, t_req_state req_state);
		void								setup_body(const Uvec& raw_body);
		void								processTransferEncoding(const Uvec& raw_body);
		void								is_cgi(std::string cgi_dir, std::vector<std::string> extensions);
	public:
		// vectors for nomalized characters
		static Uvec	CRLF;
		static Uvec	DIGIT;
		static Uvec	ALPHA;
		static Uvec	HEXDIG;
		static Uvec	UNRESERVED;
		static Uvec	SUBDELIMS;
		static Uvec	TCHAR;
		static Uvec	VCHAR;
		static Uvec	OBSTEXT;
		static Uvec	QDTEXT;
		static Uvec	QPAIR;

		Request(void);
		Request(Uvec httpRequest, const serverConf& cfg, int fd_);
		~Request(void);
		
		void				addBody(Uvec raw_body);
		// getters
		int					getParsingCode(void) const;
		const t_method&		getMethod(void) const;
		const t_target&		getTarget(void) const;
		const std::map<std::string, Uvec>&	getFields(void) const;
		bool				getFieldValue(const std::string& key, Uvec& value) const;
		const t_req_state&	getReqState(void) const;
		std::size_t			getBodySize(void) const;
		const std::string&	getBodyFilePath(void) const;

		Cgi* get_cgi() {
			return (_cgi);
		}
		const locationConf& getlocation() const {
            return *_loc;
        }
};


#endif