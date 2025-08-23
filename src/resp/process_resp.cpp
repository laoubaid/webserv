/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_resp.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 15:23:45 by laoubaid          #+#    #+#             */
/*   Updated: 2025/08/23 10:25:10 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTPResponse.hpp"

size_t get_file_size(std::fstream &file) {
    file.seekg(0, std::ios::end);
    size_t end_pos = static_cast<size_t>(file.tellg());
    file.seekg(0, std::ios::beg);  // reset to beginning
    return end_pos;
}

bool is_directory(const std::string& path) {
    struct stat s;
    if (stat(path.c_str(), &s) == 0)
        return S_ISDIR(s.st_mode);
    return false;
}

std::string url_decode(const std::string& str) {
    std::string result;
    char hex[3] = {0};
    for (size_t i = 0; i < str.length(); ++i) {
        if (str[i] == '%' && i + 2 < str.length()) {
            hex[0] = str[i + 1];
            hex[1] = str[i + 2];
            result += static_cast<char>(std::strtol(hex, nullptr, 16));
            i += 2;
        } else if (str[i] == '+') {
            result += ' ';
        } else {
            result += str[i];
        }
    }
    return result;
}

std::string resolve_path(const std::string& str) {
    std::stringstream ss(str);
    std::string part;
    std::vector<std::string> stack;

    while (std::getline(ss, part, '/')) {
        if (part.empty() || part == ".")
            continue;
        if (part == "..") {
            if (!stack.empty())
                stack.pop_back();
        } else {
            stack.push_back(part);
        }
    }

    std::string result;
    for (size_t i = 0; i < stack.size(); ++i) {
        result += "/" + stack[i];
    }
    return result.empty() ? "/" : result;
}

const locationConf& identifyie_location(const std::string& str, const serverConf& cfg) {
    std::stringstream ss(str);
    std::string part;
    std::vector<std::string> stack;

    while (std::getline(ss, part, '/')) {
        if (part.empty())
            continue;
        stack.push_back(part);
    }

    std::string tmp_path;
    while (stack.size()) {
        // form the path
        tmp_path.clear();
        for (size_t i = 0; i < stack.size(); ++i) {
            tmp_path += "/" + stack[i];
        }
        // check with location
        if (cfg.is_location(tmp_path)) {
            return cfg.get_location(tmp_path);
        } else {
            if (!stack.empty())
                stack.pop_back();
        }
    }
    tmp_path = "/";
    return cfg.get_location(tmp_path);
}

