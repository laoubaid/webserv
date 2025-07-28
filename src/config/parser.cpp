/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 16:48:38 by laoubaid          #+#    #+#             */
/*   Updated: 2025/07/28 19:58:13 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "configParser.hpp"

Block parser(std::ifstream &config, std::vector<std::string> args) {
    Block block;
    block.name = args[0];
    block.argument = args.size() > 1 ? args[1] : "";
    std::vector<std::string> new_args;

    std::string line;
    std::string buff;
    while (std::getline(config, line)) {
        // skip spaces
        std::cout << "Parsing line: " << line << std::endl;
        int i = 0;
        int len = line.length();
        while (i < len) {
            std::cout << "Current char: " << line[i] << std::endl;
            if (isspace(line[i]) || line[i] == '\n' || line[i] == '\r') {
                while (isspace(line[i]) || line[i] == '\n' || line[i] == '\r') {
                    i++;
                }
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
                block.directives.push_back(Directive(new_args));
                new_args.clear();
            }
            else if ( line[i] == '{' ) {
                if (!buff.empty()) {
                    new_args.push_back(buff);
                    buff.clear();
                }
                block.blocks.push_back(parser(config, new_args));
                new_args.clear();
            }
            else if ( line[i] == '}' ) {
                // end of block, return the block
                return block;
            }
            else if ( line[i] == '#' ) {
                // skip comments
                while (i < len && line[i] != '\n') {
                    i++;
                }
            } else {
                buff += line[i];
            }
            i++;
        }
    }
    
    exit(1); // if i didnt found a closing bracate, i just exit the program +++++++++ handle the error properly
    return block; // this useless to avoid warnings easypeasy
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

    std::cout << "webserv: fstream::open() \"" << filename << "\" succeeded" << std::endl;
    // the vector is storing the keywords in the last line "root {" -> ["root"]
    // inside parser it will be e.g "test arg {" -> ["test", "arg"] 
    // for directives it will be e.g "listen 8080;" -> ["listen", "8080"] 
    std::vector<std::string> vector = {"root"};
    Block rootBlock = parser(config, vector);
}