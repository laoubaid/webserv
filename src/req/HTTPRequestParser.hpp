/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequestParser.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/12 23:00:07 by kez-zoub          #+#    #+#             */
/*   Updated: 2025/05/23 18:23:36 by laoubaid         ###   ########.fr       */
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
		HTTPRequestParser(std::vector <char> httpRequest);
		int	getParsingCode(void) const;
		~HTTPRequestParser(void);

        t_method getMethode(void) {
            return method;
        };
};


#endif