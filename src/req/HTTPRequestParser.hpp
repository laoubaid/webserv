/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequestParser.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kez-zoub <kez-zoub@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/12 23:00:07 by kez-zoub          #+#    #+#             */
/*   Updated: 2025/07/29 00:35:46 by kez-zoub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUESTPARSER_HPP
# define HTTPREQUESTPARSER_HPP

# include "httpParsingIncludes.hpp"

class HTTPRequestParser
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
		std::string							target;
		std::map<std::string, Uvec>			fields;
		Uvec								body;
		t_req_state							req_state;

		void								processStartLine(Uvec startLine);
		void								processFields(std::vector<Uvec> lines);
		void								addField(std::string key, Uvec value);
		void								badRequest(std::string err_msg);
		void								processBody(const Uvec& raw_body);
		void	processTransferEncoding(const Uvec& transfer_encoding, const Uvec& raw_body);
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

		HTTPRequestParser(void);
		HTTPRequestParser(Uvec httpRequest);
		~HTTPRequestParser(void);
		
		void				addBody(Uvec raw_body);
		// getters
		int					getParsingCode(void) const;
		const t_method&		getMethod(void) const;
		const std::string&	getTarget(void) const;
		const std::map<std::string, Uvec>&	getFields(void) const;
		bool				getFieldValue(const std::string& key, Uvec& value) const;
		const t_req_state&	getReqState(void) const;
		const Uvec&			getBody(void) const;
};


#endif