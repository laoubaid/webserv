/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 16:48:38 by laoubaid          #+#    #+#             */
/*   Updated: 2025/07/28 22:35:20 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "configParser.hpp"

Block   parser(std::ifstream &filename, std::vector<std::string> args);

Block syntax_error(const std::string& message, const std::string& name) {
    std::cerr << "\033[31mwebserv: (syntax error) \033[0m";
    std::cerr << message << std::flush << " at block \" ." << name << std::endl;
    // return a dummy block to indicate an error

    return Block("Error");
}

Block parser(std::ifstream &config, std::vector<std::string> args) {
    Block block;
    if (args.empty()) {
        std::cerr << "\033[31mwebserv: (parser error) \033[0m";
        std::cerr << " called parser with empty args vector.\n";
        exit(1);
    }
    block.name = args[0];
    block.argument = args.size() > 1 ? args[1] : "";
    std::vector<std::string> new_args;

    std::string line;
    std::string buff;
    while (std::getline(config, line)) {
        // skip spaces
        int i = 0;
        int len = line.length();
        while (i < len) {
            if (i < len && std::isspace(line[i])) {
                while (i < len && std::isspace(line[i]))
                    ++i;
                if (!buff.empty()) {
                    new_args.push_back(buff);
                    buff.clear();
                }
            }
            if ( line[i] == ';' ) {
                if (!buff.empty()) {
                    new_args.push_back(buff);
                    buff.clear();
                }
                // this creates a directive from the vector of new_args and pushes it to the blocks vector of directives
                if (new_args.size() < 2) {
                    return syntax_error("missing directive arguments before semicolon ';'", block.name);
                }
                if (line[i + 1]) {
                    return syntax_error("unwanted text after semicolon ';'", block.name);
                }
                block.directives.push_back(Directive(new_args));
                new_args.clear();
            }
            else if ( line[i] == '{' ) {
                if (!buff.empty()) {
                    new_args.push_back(buff);
                    buff.clear();
                }
                if (new_args.empty()) {
                    return syntax_error("missing block name before opening brace '{'", block.name);
                }
                Block tmp = parser(config, new_args);
                if (tmp.name == "Error") {
                    // if the parser returned an error block, return it
                    return tmp;
                }
                block.blocks.push_back(tmp);
                new_args.clear();
            }
            else if ( line[i] == '}' ) {
                if (!buff.empty()) {
                    return syntax_error("unexpected text before closing brace '}'", block.name);
                }
                if (new_args.size() > 0) {
                    return syntax_error("unexpected directive arguments before closing brace '}'", block.name);
                }
                if (i < len - 1) {
                    return syntax_error("unexpected text after closing brace '}'", block.name);
                }
                return block;
            }
            else if ( line[i] == '#' ) {
                while (i < len) { i++; }
            // } else if (i >= len && !buff.empty()) {
            //     return syntax_error("unexpected end of line", block.name);
            } else { buff += line[i]; }
            i++;
            if (i >= len && !buff.empty()) {
                return syntax_error("unexpected end of line", block.name);
            }
        }
    }
    if (block.name == "Error") {
        std::cerr << "." << block.name;
        return block; // return the error block if it was created
    }
    if (block.name != "root") {
        return syntax_error("missing closing brace '}' at the end of the file", block.name);
    }
    return block; // this useless to avoid warnings easypeasy
}


void printTree(const Block& block, int tab = 0) {
    std::string indent(tab, '\t');
    std::cout << indent << "Block: " << block.name;
    if (!block.argument.empty()) {
        std::cout << " Argument: " << block.argument;
    }
    std::cout << std::endl;
    
    for (const auto& directive : block.directives) {
        std::cout << indent << "\tDirective: " << directive.key;
        if (!directive.values.empty()) {
            std::cout << " Values: ";
            for (const auto& value : directive.values) {
                std::cout << value << " ";
            }
        }
        std::cout << std::endl;
    }
    
    for (const auto& subBlock : block.blocks) {
        printTree(subBlock, tab + 1);
    }
}
void get_config(std::string filename) {
    // open file and pass it to parser
    std::ifstream config;
    
    filename = "./src/config/" + filename;
    config.open(filename.c_str());
    if (!config.is_open()) {
        std::cerr << "webserv: fstream::open() \"" << filename << "\" failed";
        perror(" ");
        return ; // specify what to do if the file cannot be opened -> most likely exit the program
    }
    
    // printing the successful opening of the file in green color
    std::cout << "\033[32m" << "webserv: fstream::open() \"" << filename << "\" successful" << "\033[0m" << std::endl;
    // the vector is storing the keywords in the last line "root {" -> ["root"]
    // inside parser it will be e.g "test arg {" -> ["test", "arg"] 
    // for directives it will be e.g "listen 8080;" -> ["listen", "8080"] 
    std::vector<std::string> vector = {"root"};
    Block rootBlock = parser(config, vector);
    config.close();
    std::cout << std::endl;
    printTree(rootBlock);
}