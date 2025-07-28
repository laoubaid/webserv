/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_fields.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kez-zoub <kez-zoub@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/29 23:42:33 by kez-zoub          #+#    #+#             */
/*   Updated: 2025/06/24 03:52:06 by kez-zoub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "httpParsingIncludes.hpp"

void	printvec(const Uvec &vec, int printable)
{
	if (printable)
		std::cout << std::string(vec.begin(), vec.end()) << std::endl;
	else
	{
		for (Uvec::const_iterator it = vec.begin(); it != vec.end(); it++)
		{
			std::cout << "'" << *it << "', ";
		}
		std::cout<< "[終わり]" << std::endl;
	}
}

// Uvec	strToVec(const unsigned char *str, std::size_t len)
// {
// 	return (Uvec(str, str + len));
// }

// add cases for each tested validator with the expected result (true/false)
// -----------------------------cases---------------------------------------
std::vector<std::pair<Uvec, bool>>	targetTestCases(void)
{
	std::vector<std::pair<Uvec, bool>>	cases;
	
	cases.push_back(std::make_pair(Uvec((const unsigned char *)"target", 6), true));
	cases.push_back(std::make_pair(Uvec((const unsigned char *)"/", 1), true));
	cases.push_back(std::make_pair(Uvec((const unsigned char *)"//", 2), true));
	cases.push_back(std::make_pair(Uvec((const unsigned char *)"/?", 2), true));
	cases.push_back(std::make_pair(Uvec((const unsigned char *)"/fsf/fsf?($)", 12), true));

	cases.push_back(std::make_pair(Uvec((const unsigned char *)"", 0), false));
	cases.push_back(std::make_pair(Uvec((const unsigned char *)"f/", 2), false));

	return (cases);
}

std::vector<std::pair<Uvec, bool>>	contentLengthTestCases(void)
{
	std::vector<std::pair<Uvec, bool>>	cases;
	
	cases.push_back(std::make_pair(Uvec((const unsigned char *)"content length", 14), true));
	cases.push_back(std::make_pair(Uvec((const unsigned char *)"42", 2), true));
	cases.push_back(std::make_pair(Uvec((const unsigned char *)"42, 42", 6), true));
	cases.push_back(std::make_pair(Uvec((const unsigned char *)"42  ,  42", 9), true));

	cases.push_back(std::make_pair(Uvec((const unsigned char *)"", 0), false));
	cases.push_back(std::make_pair(Uvec((const unsigned char *)"42f", 3), false));
	cases.push_back(std::make_pair(Uvec((const unsigned char *)"42,", 3), false));

	return (cases);
}

std::vector<std::pair<std::string, bool>>	acceptTestCases(void)
{
	std::vector<std::pair<std::string, bool>>	cases;
	
	cases.push_back(std::make_pair("", true));
	cases.push_back(std::make_pair("*/*", true));
	
	return (cases);
}

std::vector<std::pair<Uvec, bool>>	IPv6addressCases(void)
{
	std::vector<std::pair<Uvec, bool>>	cases;
	
	cases.push_back(std::make_pair(Uvec((const unsigned char *)"IPv6 address", 12), true));
	cases.push_back(std::make_pair(Uvec((const unsigned char *)"09af:09af:09af:09af:09af:09af:09af:09af", 39), true));
	cases.push_back(std::make_pair(Uvec((const unsigned char *)"09af:09af:09af:09af:09af:09af:255.255.255.255", 45), true));
	cases.push_back(std::make_pair(Uvec((const unsigned char *)"09af::", 6), true));
	cases.push_back(std::make_pair(Uvec((const unsigned char *)"::", 2), true));
	cases.push_back(std::make_pair(Uvec((const unsigned char *)"::255.255.255.255", 17), true));
	cases.push_back(std::make_pair(Uvec((const unsigned char *)"::09af:09af", 11), true));

	cases.push_back(std::make_pair(Uvec((const unsigned char *)"", 0), false));
	cases.push_back(std::make_pair(Uvec((const unsigned char *)"09af1::", 7), false));
	cases.push_back(std::make_pair(Uvec((const unsigned char *)"09ag::", 6), false));
	cases.push_back(std::make_pair(Uvec((const unsigned char *)"255.255.255.255", 15), false));
	cases.push_back(std::make_pair(Uvec((const unsigned char *)"09af:09af:255.255.255.255", 25), false));
	cases.push_back(std::make_pair(Uvec((const unsigned char *)"09af:09af:09af:09af:09af:09af:255.255.255.255:09af:255.255.255.255", 66), false));
	
	return (cases);
}

std::vector<std::pair<std::string, bool>>	IPvFuturesCases(void)
{
	std::vector<std::pair<std::string, bool>>	cases;
	
	cases.push_back(std::make_pair("v0f.:", true));
	
	cases.push_back(std::make_pair("", false));
	
	return (cases);
}
// -------------------------------------------------------------------------

bool	testCases(std::vector<std::pair<Uvec, bool>> cases, bool (*validator)(const Uvec &))
{
	std::vector<std::pair<Uvec, bool>>::iterator	it = cases.begin();
	int	valid = 1;

	std::cout << "----------------- testing " << std::string((*it).first.begin(), (*it).first.end()) << " -----------------\n";
	it++;
	for (; it != cases.end(); it++)
	{
		bool	implemented_ouput = validator((*it).first);
		if (implemented_ouput == (*it).second)
		{
			std::cout << GREEN_COLOR << "Case " << it - cases.begin() <<": " << std::string((*it).first.begin(), (*it).first.end()) << " is ";
			if (implemented_ouput)
				std::cout << "true";
			else
				std::cout << "false";
			std::cout << " | valid ✅" << RESET_COLOR << std::endl;
		}
		else
		{
			std::cout << RED_COLOR << "Case " << it - cases.begin() <<": " << std::string((*it).first.begin(), (*it).first.end()) << " is ";
			if (implemented_ouput)
				std::cout << "true";
			else
				std::cout << "false";
			std::cout << " | invalid ❌" << RESET_COLOR << std::endl;
			valid = 0;
		}
	}
	std::cout << "\n\n\n";
	if (valid)
		return (true);
	return (false);
}

bool	testAllFields(void)
{
	std::vector<std::pair<std::vector<std::pair<Uvec, bool>>, validatorFunc>> cases;
	std::vector<std::pair<std::vector<std::pair<Uvec, bool>>, validatorFunc>>::iterator	it;

	// add test cases (("case", true or not) , validator result true or not) testCases compare expected with validator's result
	// ---------------------------------adding cases-------------------------------------
	cases.push_back(std::make_pair(targetTestCases(), validateTarget));
	cases.push_back(std::make_pair(contentLengthTestCases(), validateContentLength));
	cases.push_back(std::make_pair(IPv6addressCases(), testIPv6address));
	// ----------------------------------------------------------------------------------


	for (it = cases.begin(); it != cases.end(); it++)
	{
		if (!testCases((*it).first, (*it).second))
			return(false);
	}
	return (true);
}
