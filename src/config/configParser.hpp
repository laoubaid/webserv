/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   configParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 16:49:54 by laoubaid          #+#    #+#             */
/*   Updated: 2025/07/28 19:56:12 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

class Directive {
    public:
        std::string key;
        std::vector<std::string> values;

        Directive() = default;
        Directive(const std::string& k) : key(k) {}
        Directive(const std::vector<std::string> args) : key(args[0]) {
            for (size_t i = 1; i < args.size(); ++i) {
                values.push_back(args[i]);
            }
        }
        // You can add helper methods here if needed
};

class Block {
    public:
        std::string name;
        std::string argument;
        std::vector<Directive> directives;
        std::vector<Block> blocks;

        // You can add helper methods here if needed
};

Block   parser(std::ifstream &filename, std::vector<std::string> args);
void    get_config(std::string filename);


#endif
