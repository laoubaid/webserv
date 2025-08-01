/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   configParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 16:49:54 by laoubaid          #+#    #+#             */
/*   Updated: 2025/08/02 00:00:38 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef CONFIG_PARSER_HPP
# define CONFIG_PARSER_HPP

# include <iostream>
# include <fstream>
# include <string>
# include <vector>


# define ERR_TXT_BRC "unexpected text before closing brace '}'"
# define ERR_DIR_BRC "unexpected directive arguments before closing brace '}'"
# define ERR_BRC_TXT "unexpected text after closing brace '}'"


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
};

class Block {
    public:
        std::string name;
        std::string argument;
        std::vector<Directive> directives;
        std::vector<Block> blocks;
        std::string error_message;

        Block() = default;
        Block(const std::string& n) : name(n) {}

};

Block    get_config(std::string filename);


#endif
