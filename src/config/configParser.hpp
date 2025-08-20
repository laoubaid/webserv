/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   configParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 16:49:54 by laoubaid          #+#    #+#             */
/*   Updated: 2025/08/19 22:38:39 by laoubaid         ###   ########.fr       */
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
# define ERR_END_LIN "unexpected end of line"


class Directive {
    public:
        std::string key;
        std::vector<std::string> values;

        Directive() {};
        Directive(const std::string& k) : key(k) {}
        Directive(const std::vector<std::string> args) : key(args[0]) {
            for (size_t i = 1; i < args.size(); ++i) {
                values.push_back(args[i]);
            }
        }
};

class Block {
    private:
        int                     state;
        std::vector<Block>      blocks;
        std::vector<Directive>  directives;
    public:
        std::string name;
        std::string argument;
        std::string error_message;

        // Block() {};
        Block() : state(1) {};
        Block(const std::string& n) : name(n) {}

        bool    isopen() {
            return state;
        }
        void    close() {
            state = 0;
        }
        void    add_block(Block& obj) {
            blocks.push_back(obj);
        }
        void    add_direc(Directive obj) {
            directives.push_back(obj);
        }
        Block&    last_block() {
            return blocks.back();
        }
        size_t  blocks_size() {
            return blocks.size();
        }

        void printTree(int idt) const;

};

int   get_config(std::string filename);  // void for now


#endif
