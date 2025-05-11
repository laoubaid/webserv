#include "httpParsingIncludes.hpp"

std::vector<std::pair<std::string, bool>>	acceptTestCases(void)
{
	std::vector<std::pair<std::string, bool>>	cases;

	cases.push_back(std::make_pair("", true));
	cases.push_back(std::make_pair("*/*", true));
	
	return (cases);
}

bool	testCases(std::vector<std::pair<std::string, bool>> cases, bool (*validator)(std::string))
{
	const char* RED_COLOR = "\033[31m";
	const char* GREEN_COLOR = "\033[32m";
	const char* RESET_COLOR = "\033[0m";
	std::vector<std::pair<std::string, bool>>::iterator	it;

	for (it = cases.begin(); it != cases.end(); it++)
	{
		if (validator((*it).first) == (*it).second)
		{
			std::cout << GREEN_COLOR << "Case: " << (*it).first << " | is valid ✅" << RESET_COLOR << std::endl;
		}
		else
		{
			std::cout << GREEN_COLOR << "Case: " << (*it).first << " | is invalid ❌" << RESET_COLOR << std::endl;
			return (false);
		}
	}
	return (true);
}

bool	testAllFields(void)
{
	if (!testCases(acceptTestCases(), validateAccept))
		return (false);
	return (true);
}
