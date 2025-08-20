/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 16:48:38 by laoubaid          #+#    #+#             */
/*   Updated: 2025/08/19 22:58:46 by laoubaid         ###   ########.fr       */
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



Block   parser(std::ifstream &config, std::vector<std::string> args);

void rtrim(std::string &s) {
    while (!s.empty() && std::isspace(static_cast<unsigned char>(s.back()))) {
        s.pop_back();
    }
}

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

int process_directive(std::string& buff, std::vector<std::string>& new_args, Block& block, std::string& line, int idx) {
    if (!buff.empty()) {
        new_args.push_back(buff);
        buff.clear();
    }
    if (new_args.size() < 2) {
        syntax_error("missing directive arguments before semicolon ';'", block.name);
        return 1;
    }
    while (std::isspace(line[idx])) ++idx;
    if (idx != line.length() && line[idx] != '#') {
        syntax_error("unwanted text after semicolon ';'", block.name);
        return 1;
    }
    block.add_direc(Directive(new_args));
    new_args.clear();
    return 0;
}

int process_block(std::string& buff, std::vector<std::string>& args, Block& block, std::ifstream &cfg) {
    if (!buff.empty()) {
        args.push_back(buff);
        buff.clear();
    }
    if (args.empty()) {
        syntax_error("missing block name before opening brace '{'", block.name);
        return 1;
    }
    // std::cout << "** Processing block: " << args[0] << std::endl;
    Block tmp = parser(cfg, args);
    if (tmp.name == "Error") {
        return 1;
    }
    block.add_block(tmp);
    args.clear();
    // std::cout << "** Finished processing block: " << block.name << std::endl;
    return 0;
}

Block process_line(std::ifstream &config, std::string& line, Block& block) {
    int i = 0;
    int len = line.length();
    std::string buff;
    std::vector<std::string> new_args;

    std::cout << ">> LINE Processing line: '" << line << "'" << std::endl;
    while (i < len) {
        if (std::isspace(line[i]) && !buff.empty()) {
            new_args.push_back(buff);
            buff.clear();
        }
        while (std::isspace(line[i])) ++i;
        switch (line[i])
        {
            case ';':
                if (process_directive(buff, new_args, block, line, i + 1))
                    return Block("Error");
                break;
            case '{':
                if (process_block(buff, new_args, block, config))
                    return Block("Error");
                break;
            case '}':
                block.close();
                return (!buff.empty()) ? syntax_error(ERR_TXT_BRC, block.name) :
                    (new_args.size() > 0) ? syntax_error(ERR_DIR_BRC, block.name) :
                    (i < len - 1) ? syntax_error(ERR_BRC_TXT, block.name) :
                    block;
                break;
            
            case '#':
                i = len;
                break;

            default:
                buff += line[i];
                break;
        }
        ++i;
        if (i >= len && !buff.empty()) {
            return syntax_error(ERR_END_LIN, block.name);
        }
    }
    return (!block.blocks_size()) ? Block("test") : block.last_block(); // return the last processed block
}


Block parser(std::ifstream &config, std::vector<std::string> args) {
    if (args.empty())
        return syntax_error("Empty args vector", "Internal Error");
    
    Block block;
    std::string line;
    block.name = args[0];
    block.argument = args.size() > 1 ? args[1] : "";

    // std::cout << "++ Parsing block: \t\t\t\t[" << block.name << "]" << std::endl;
    while (std::getline(config, line)) {
        size_t i = 0;
        rtrim(line);
        while (std::isspace(line[i])) ++i;
        if (line.empty() || line[i] == '#' || i == line.length())
            continue; // skip empty lines and comments
        Block tmp_block = process_line(config, line, block);
        if (tmp_block.name == block.name)
            break ;
        if (tmp_block.name == "Error")
            return tmp_block;
    }
    if (block.isopen() && block.name != "root")
        return syntax_error("missing closing brace '}' at the end of the file", block.name);
    // std::cout << "-- Finished parsing block: " << block.name << std::endl;
    return block;
}

void Block::printTree(int tab) const {
    std::cout << std::string(tab * 4, ' ') << "Block: " << name;
    if (!argument.empty()) {
        std::cout << " Argument: " << argument;
    }
    if (!error_message.empty()) {
        std::cout << " Error: " << error_message;
    }
    std::cout << std::endl;

    for (const auto& directive : directives) {
        std::cout << std::string((tab + 1) * 4, ' ') << "Directive: " << directive.key;
        for (const auto& value : directive.values) {
            std::cout << " Value: " << value;
        }
        std::cout << std::endl;
    }

    for (const auto& subBlock : blocks) {
        subBlock.printTree(tab + 1);
    }

}


void syntax(Block rootBlock);

int get_config(std::string filename) {

    std::ifstream config;

    filename = "./src/config/" + filename;
    config.open(filename.c_str());
    if (!config.is_open()) {
        std::cerr << "webserv: fstream::open() \"" << filename << "\" failed";
        perror(" ");
        return 1;
        // return Block("Error"); // specify what to do if the file cannot be opened -> most likely exit the program
    }

    std::vector<std::string> vector = {"root"};
    Block rootBlock = parser(config, vector);
    config.close();
    std::cout << std::endl;
    rootBlock.printTree(0);

    syntax(rootBlock);

    
    return 0;
    // return rootBlock; // return the root block
}
