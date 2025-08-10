/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 16:48:38 by laoubaid          #+#    #+#             */
/*   Updated: 2025/08/08 16:58:06 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "configParser.hpp"


//////////////////////////////// Documentation: ////////////////////////////////
//                                                                            //
// This file contains the implementation of the configuration parser.         //
//                                                                            //
//     the configuration file is expected to have a specific syntax, which    //
// includes directives, blocks, and arguments. The parser reads the file line //
// by line, processes each line according to the syntax rules, and builds a   //
// tree structure of blocks and directives (recursively).                     //
//                                                                            //
//    The get_config() function is the entry point for the parser. It opens   //
// the configuration file, calls the parser, and returns the root block of    //
// the parsed configuration.                                                  //
//                                                                            //
//     First the parser() function takes a refrence to stream object and      //
// a vector of strings as arguments. It reads the configuration file line by  //
// line, the process_line() function takes it from here and processes each    //
// line according to the syntax rules. It checks for indentation, directives, //
// blocks, and errors. The process_directive() and process_block() functions  //
// handle the processing of directives and blocks, respectively.              //
//                                                                            //
//    The parser builds a tree structure of blocks and directives, where each //
// block can contain directives and sub-blocks.                               //
//                                                                            //
//    The syntax_error() function is used to handle syntax errors and print   //
// error messages. It returns a Block object with "Error" as name to indicate //
// that an error occurred during parsing.                                     //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////



Block   parser(std::ifstream &config, std::vector<std::string> args, int indentation = 0);

Block syntax_error(const std::string& message, const std::string& name) {
    std::cerr << "\033[31mwebserv: (syntax error) \033[0m";
    std::cerr << message << std::flush << " at block ." << name << std::endl;

    return Block("Error");
}

bool check_indentation(const std::string& line, int &indent) {
    
    if (line.find('}') != std::string::npos)
        --indent; // if the line contains a closing brace, it is valid

    std::string expected(indent * 4, ' ');
    
    return indent ? (line.rfind(expected) == 0) : true;
}

int process_directive(std::string& buff, std::vector<std::string>& new_args, Block& block, int endline) {
    if (!buff.empty()) {
        new_args.push_back(buff);
        buff.clear();
    }
    if (new_args.size() < 2) {
        syntax_error("missing directive arguments before semicolon ';'", block.name);
        return 1;
    }
    if (endline) {
        syntax_error("unwanted text after semicolon ';'", block.name);
        return 1;
    }
    block.directives.push_back(Directive(new_args));
    new_args.clear();
    return 0;
}

int process_block(std::string& buff, std::vector<std::string>& args, Block& block, int &idt, std::ifstream &cfg) {
    // std::cout << "** Processing block: " << block.name << std::endl;
    if (!buff.empty()) {
        args.push_back(buff);
        buff.clear();
    }
    if (args.empty()) {
        syntax_error("missing block name before opening brace '{'", block.name);
        return 1;
    }
    Block tmp = parser(cfg, args, idt + 1);
    if (tmp.name == "Error") {
        return 1;
    }
    block.blocks.push_back(tmp);
    args.clear();
    // std::cout << "** Finished processing block: " << block.name << std::endl;
    return 0;
}

Block process_line(std::ifstream &config, const std::string& line, Block& block, int &indentation) {
    int i = 0;
    int len = line.length();
    std::string buff;
    std::vector<std::string> new_args;

    // std::cout << ">> LINE Processing line: [" << line << "]" << std::endl;
    while (i < len) {
        if (i < len && std::isspace(line[i])) {
            while (i < len && std::isspace(line[i]))
                ++i;
            if (!buff.empty()) {
                new_args.push_back(buff);
                buff.clear();
            }
        }
        if ( line[i] == ';') {
            if (process_directive(buff, new_args, block, line[i + 1])) {
                return Block("Error");
            }
        }
        else if ( line[i] == '{'){
            if (process_block(buff, new_args, block, indentation, config))
                return Block("Error");
        }
        else if ( line[i] == '}' ) {
            return (!buff.empty()) ? syntax_error(ERR_TXT_BRC, block.name) :
                   (new_args.size() > 0) ? syntax_error(ERR_DIR_BRC, block.name) :
                   (i < len - 1) ? syntax_error(ERR_BRC_TXT, block.name) :
                   block;
        }
        else if ( line[i] == '#' ) {
            while (i < len) { i++; }
        } else { buff += line[i]; }
        i++;
        if (i >= len && !buff.empty()) {
            return syntax_error("unexpected end of line", block.name);
        }
    }
    // std::cout << "<< LINE returned block name: " << block.name << std::endl;


    /* this return linr is to make control the flow of the parser in order to match every block
     with its parent, NOTE: the creation of Block("test") is a temporary solution */
    return (block.blocks.empty()) ? Block("test") : block.blocks.back(); // return the last processed block
    // return block.blocks.back(); // return the last processed block
}


Block parser(std::ifstream &config, std::vector<std::string> args, int indentation) {
    Block block;
    if (args.empty())
        return syntax_error("Empty args vector", "Internal Error");

    block.name = args[0];
    block.argument = args.size() > 1 ? args[1] : "";

    // std::cout << "++ Parsing block: [" << block.name << "]" << std::endl;
    std::string line;

    while (std::getline(config, line)) {
        if (line.empty() || line[0] == '#') {
            continue; // skip empty lines and comments
        }

        if (check_indentation(line, indentation) == false) {
            std::cout << "line: [" << line << "]" << indentation << std::endl;
            return syntax_error("Malformed indentation", block.name);
        }
        
        // std::cout << "[" << block.name << "] is about to process line: " << line << std::endl;
        Block tmp_block = process_line(config, line, block, indentation);
        // std::cout << "[" << block.name << "] is DONE processing line: " << line << std::endl;
        
        // std::cout << "-- block name after processing line: " << tmp_block.name << std::endl;
        if (tmp_block.name == block.name) {
            break ;
        }
    }

    indentation++;
    // std::cout << "-- Finished parsing block: " << block.name << std::endl;
    if (block.name != "root" && indentation == 0) {
        return syntax_error("missing closing brace '}' at the end of the file", block.name);
    }

    return block;
}


void printTree(const Block& block, int tab = 0) {
    std::cout << std::string(tab * 4, ' ') << "Block: " << block.name;
    if (!block.argument.empty()) {
        std::cout << " Argument: " << block.argument;
    }
    if (!block.error_message.empty()) {
        std::cout << " Error: " << block.error_message;
    }
    std::cout << std::endl;

    for (const auto& directive : block.directives) {
        std::cout << std::string((tab + 1) * 4, ' ') << "Directive: " << directive.key;
        for (const auto& value : directive.values) {
            std::cout << " Value: " << value;
        }
        std::cout << std::endl;
    }

    for (const auto& subBlock : block.blocks) {
        // std::cout << "current block:" << block.name << " sub-block: " << subBlock.name << std::endl;
        printTree(subBlock, tab + 1);
    }

}
Block get_config(std::string filename) {
    // open file and pass it to parser
    std::ifstream config;

    filename = "./src/config/" + filename;
    config.open(filename.c_str());
    if (!config.is_open()) {
        std::cerr << "webserv: fstream::open() \"" << filename << "\" failed";
        perror(" ");
        return Block("Error"); // specify what to do if the file cannot be opened -> most likely exit the program
    }

    // std::cout << "\033[32m" << "webserv: fstream::open() \"" << filename << "\" successful" << "\033[0m" << std::endl;

    std::vector<std::string> vector = {"root"};
    Block rootBlock = parser(config, vector);
    config.close();
    std::cout << std::endl;
    // printTree(rootBlock);
    return rootBlock; // return the root block
}
