/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequestParser.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/12 23:00:07 by kez-zoub          #+#    #+#             */
/*   Updated: 2025/09/05 15:20:27 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUESTPARSER_HPP
# define HTTPREQUESTPARSER_HPP

# include "httpParsingIncludes.hpp"

class Cgi;
class serverConf;

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

		int									parsingCode;
		t_method							method;
		t_target							target;
		std::map<std::string, Uvec>			fields;
		std::size_t							body_size;
		std::string							body_file_path;
		t_req_state							req_state;
		Cgi									*cgi;
		const serverConf					*conf_;
		int									client_fd_;
		
		void								processStartLine(Uvec startLine);
		void								processFields(std::vector<Uvec> lines);
		void								addField(std::string key, Uvec value);
		void								badRequest(std::string err_msg);
		void								processBody(const Uvec& raw_body);
		void								processTransferEncoding(const Uvec& transfer_encoding, const Uvec& raw_body);
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
			return cgi;
		}
};


#endif