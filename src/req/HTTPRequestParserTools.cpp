/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequestParserTools.cpp                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kez-zoub <kez-zoub@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/13 20:28:30 by kez-zoub          #+#    #+#             */
/*   Updated: 2025/10/28 23:24:53 by kez-zoub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTPRequestParser.hpp"

// to distinguish either the spliter is at the end or not, I will add an empty string to the array if it is at the end
std::vector<Uvec>	ft_split(const Uvec &data, const Uvec &spliter)
{
	std::vector<Uvec>	vec;
	Uvec::const_iterator	start = data.begin();
	Uvec::const_iterator	pos = data.find(spliter);
	while (pos != data.end())
	{
		vec.push_back(Uvec(start, pos));
		start = pos + spliter.size();
		pos = data.find(start, data.end(), spliter);
	}
	vec.push_back(Uvec(start, data.end()));
	return (vec);
}

Uvec	ft_trim(Uvec vec)
{
	Uvec::iterator	start = vec.begin();
	Uvec::iterator	end = vec.end();

	while (start != end && (*start == ' ' || *start == '\t'))
		start++;
	while (start != end && (*(end -1) == ' ' || *(end -1) == '\t'))
		end--;
	return (Uvec(start, end));
}

unsigned char toLowerChar(unsigned char c)
{
    return static_cast<unsigned char>(std::tolower(static_cast<int>(c)));
}

bool hexStringToUnsignedLong(const std::string& hexStr, unsigned long& result) {
    std::stringstream ss;
    ss << std::hex << hexStr; // Insert hex string into stringstream, set base to hex

    // Attempt to extract the unsigned long
    ss >> result;

    // Check for errors:
    // 1. If the entire string was consumed and conversion was successful
    // 2. If no conversion error flags are set (badbit, failbit)
    if (!ss.fail() && ss.eof()) {
        return true;
    } else {
        // Conversion failed (e.g., non-hex characters, empty string, overflow)
        ss.clear(); // Clear error flags if you intend to reuse the stream
        return false;
    }
}

bool stringToUnsignedLong(const std::string& str, unsigned long& result) {
    std::stringstream ss;
    ss << str; // Insert hex string into stringstream, set base to hex

    // Attempt to extract the unsigned long
    ss >> result;

    // Check for errors:
    // 1. If the entire string was consumed and conversion was successful
    // 2. If no conversion error flags are set (badbit, failbit)
    if (!ss.fail() && ss.eof()) {
        return true;
    } else {
        // Conversion failed (e.g., non-hex characters, empty string, overflow)
        ss.clear(); // Clear error flags if you intend to reuse the stream
        return false;
    }
}

const Uvec&		getValue(const std::map<std::string, Uvec>& map, const std::string& key)
{
	std::map<std::string, Uvec>::const_iterator	it = map.find(key);

	if (it == map.end())
		throw std::out_of_range(key +" not found");
	return (it->second);
}

std::string	ft_itos(std::size_t num)
{
	std::stringstream	oss;

	oss << num;
	return (oss.str());
}

int	ft_hextoi(const std::string& hex)
{
	std::istringstream	iss(hex);
	int num;
	iss >> std::hex >> num;
	return (num);
}

std::string	decode_url(const std::string& url)
{
	std::string	result;

	for (std::string::const_iterator it = url.begin(); it != url.end(); it++)
	{
		if (*it == '%' && std::isxdigit(*(it+1)) && std::isxdigit(*(it+2)))
		{
			it++;
			if (std::isxdigit(*it) && std::isxdigit(*(it+1)))
				result += ft_hextoi(std::string(it, it +2));
			it++;
		}
		else
			result += *it;
	}
	return (result);
}

bool	has_shebang(std::string script_path)
{
	std::ifstream	file(script_path);

	if (file.is_open())
	{
		char c1, c2;
		file.get(c1);
		file.get(c2);
		if (c1 == '#' && c2 == '!')
			return (true);
	}
	file.close();
	return (false);
}

char	*ft_strdup(const std::string& src)
{
	char	*str = new char[src.size() +1];
	std::strcpy(str, src.c_str());
	return (str);
}

std::string	get_ext(const std::string& path)
{
	std::string::const_iterator	it = path.end();

	while (it != path.begin() && *it != '.')
		it--;
	if (*it == '.')
		return (std::string(it+1, path.end()));
	else
		return (std::string(""));
}

bool fileExists(const char* filename)
{
	struct stat buffer;
    return (stat(filename, &buffer) == 0);
}

