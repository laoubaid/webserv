/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequestParser.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kez-zoub <kez-zoub@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/12 23:00:07 by kez-zoub          #+#    #+#             */
/*   Updated: 2025/05/08 19:04:11 by kez-zoub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUESTPARSER_HPP
# define HTTPREQUESTPARSER_HPP

# include "httpParsingIncludes.hpp"

class HTTPRequestParser
{
	private:
		static std::map<std::string, validatorFunc>	stdFields;

		int									parsingCode;
		t_method							method;
		std::string							requestTarget;
		std::string							HTTPVersion;
		std::map<std::string, std::string>	fields;
		std::string							body;
		bool								chunked;

		void								fieldsInit(void);
		void								processStartLine(std::string startLine);
		void								processFields(std::vector<std::string> lines);
	public:
		HTTPRequestParser(void);
		HTTPRequestParser(std::string httpRequest);
		bool	getParsingCode(void) const;
		~HTTPRequestParser(void);
};


#endif