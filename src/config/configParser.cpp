/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   configParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 16:48:38 by laoubaid          #+#    #+#             */
/*   Updated: 2025/09/30 00:00:00 by laoubaid         ###   ########.fr       */
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



Block   syntax(std::ifstream &config, std::vector<std::string> args);

void rtrim(std::string &s) {
    while (!s.empty() && std::isspace(static_cast<unsigned char>(s.back()))) {
        s.pop_back();
    }
}

Block Block::syntax_error(const std::string& message) {
    std::cerr << "\033[31mwebserv: (syntax error) \033[0m";
    std::cerr << message << std::flush << " at block ." << name << std::endl;

    return Block("Error");
}

int process_directive(std::string& buff, std::vector<std::string>& new_args, Block& block, std::string& line, size_t idx) {
    if (!buff.empty()) {
        new_args.push_back(buff);
        buff.clear();
    }
    if (new_args.size() < 2) {
        block.syntax_error("missing directive arguments before semicolon ';'");
        return 1;
    }
    while (std::isspace(line[idx])) ++idx;
    if (idx != line.length() && line[idx] != '#') {
        block.syntax_error("unwanted text after semicolon ';'");
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
        block.syntax_error("missing block name before opening brace '{'");
        return 1;
    }
    Block tmp = syntax(cfg, args);
    if (tmp.get_name() == "Error") {
        return 1;
    }
    block.add_block(tmp);
    args.clear();
    return 0;
}

Block process_line(std::ifstream &config, std::string& line, Block& block) {
    int i = 0;
    int len = line.length();
    std::string buff;
    std::vector<std::string> new_args;

    // std::cout << ">> LINE Processing line: '" << line << "'" << std::endl;
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
                return (!buff.empty()) ? block.syntax_error(ERR_TXT_BRC) :
                    (new_args.size() > 0) ? block.syntax_error(ERR_DIR_BRC) :
                    (i < len - 1) ? block.syntax_error(ERR_BRC_TXT) :
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
            return block.syntax_error(ERR_END_LIN);
        }
    }
    return (!block.blocks_size()) ? Block("test") : block.last_block();
}


Block syntax(std::ifstream &config, std::vector<std::string> args) {
    if (args.empty())
        return Block("Internal Error").syntax_error("Empty args vector");
    
    Block block(args);
    std::string line;

    while (std::getline(config, line)) {
        size_t i = 0;
        rtrim(line);
        while (std::isspace(line[i])) ++i;
        if (line.empty() || line[i] == '#' || i == line.length())
            continue;
        Block tmp_block = process_line(config, line, block);
        if (tmp_block.get_name() == block.get_name())
            break ;
        if (tmp_block.get_name() == "Error")
            return tmp_block;
    }
    if (block.isopen() && block.get_name() != "root")
        return block.syntax_error("Missing closing brace '}' at the end of the file");
    return block;
}

void Block::process_location(serverConf& srvr_cfg) {
    
    if (argument.empty())
        throw std::runtime_error("missing location argument!");
        
    if (blocks.size())
        throw std::runtime_error("Blocks not allowed inside location block!");
    
    locationConf lct_cfg(argument, srvr_cfg);

    std::vector<Directive>::iterator it_d;
    for (it_d = directives.begin(); it_d != directives.end(); ++it_d) {
        if ((*it_d).key == "methods") {
            lct_cfg.set_methods((*it_d).values);
        } else if ((*it_d).key == "return") {
            lct_cfg.set_redirect((*it_d).values);
        } else if ((*it_d).key == "root") {
            lct_cfg.set_root((*it_d).values);
        } else if ((*it_d).key == "index") {
            lct_cfg.set_index((*it_d).values);
        } else if ((*it_d).key == "autoindex") {
            lct_cfg.set_auto_index((*it_d).values);
        } else if ((*it_d).key == "upload_store") {
            lct_cfg.set_up_store((*it_d).values);
        } else if ((*it_d).key == "CGI") {
            lct_cfg.set_cgi((*it_d).values);
        } else {
            throw std::runtime_error("unknown directive! " + (*it_d).key);
        }
    }

    if (lct_cfg.get_redirect().first != 0) {
        srvr_cfg.redirs[lct_cfg.get_path()] = lct_cfg.get_redirect().second;
    }

    srvr_cfg.add_location(lct_cfg);
}

void Block::process_server(std::vector<serverConf>& servres) {
    serverConf srvr_cfg;

    std::vector<Directive>::iterator it_d;
    for (it_d = directives.begin(); it_d != directives.end(); ++it_d) {
        if ((*it_d).key == "listen") {
            srvr_cfg.add_listen((*it_d).values);
        } else if ((*it_d).key == "error_page") {
            srvr_cfg.add_err_page((*it_d).values);
        } else if ((*it_d).key == "client_max_body_size") {
            srvr_cfg.set_clt_max_body_size((*it_d).values);
        } else if ((*it_d).key == "root") {
            srvr_cfg.set_root((*it_d).values);
        } else if ((*it_d).key == "index") {
            srvr_cfg.set_index((*it_d).values);
        } else if ((*it_d).key == "autoindex") {
            srvr_cfg.set_index((*it_d).values);
        } else if ((*it_d).key == "recv_timeout") {
            srvr_cfg.set_timeout((*it_d).values, 0);
        } else if ((*it_d).key == "cgi_timeout") {
            srvr_cfg.set_timeout((*it_d).values, 1);
        } else if ((*it_d).key == "send_timeout") {
            srvr_cfg.set_timeout((*it_d).values, 2);
        } else if ((*it_d).key == "return") {  			//* could be changed to redir
            srvr_cfg.set_redirect((*it_d).values);
        } else {
            throw std::runtime_error("unknown directive! " + (*it_d).key);
        }
    }

	srvr_cfg.get_err_page(0);

    if (srvr_cfg.get_redirect().first != 0) {
        srvr_cfg.redirs["/"] = srvr_cfg.get_redirect().second;
    }

    srvr_cfg.set_default();

    std::vector<Block>::iterator it_b;
    for (it_b = blocks.begin(); it_b != blocks.end(); ++it_b) {
        if ((*it_b).name == "location") {
            (*it_b).process_location(srvr_cfg);
        } else {
            throw std::runtime_error("unknown Block! " + (*it_b).name);
        }
    }
    std::map<std::string, std::string>::iterator it;
    for (it = srvr_cfg.redirs.begin(); it != srvr_cfg.redirs.end(); ++it) {
        std::string path = (*it).second;
        std::set<std::string> visited;
        while (true) {
            if (visited.count(path))
                throw std::runtime_error("redirection loop detected!");
			if (srvr_cfg.is_location(path) == false)
				break ;
            visited.insert(path);
            const locationConf& lct = srvr_cfg.identifie_location(path);
            if (lct.has_redirect() == false)
                break ;
            path = lct.get_redirect().second;
        }   
    }
    servres.push_back(srvr_cfg);
}

std::vector<serverConf>* Block::parser() {
    std::vector<serverConf>* servers = new std::vector<serverConf>();

    if (directives.size())
        throw std::runtime_error("directives not allowed on config root!");
    for (std::vector<Block>::iterator it = blocks.begin(); it != blocks.end(); ++it) {
        if ((*it).name != "server") {
            throw std::runtime_error("only server blocks are allowed on config root!");
        }
        (*it).process_server(*servers);
    }
    return servers;
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

Block get_config(std::string filename) {

    std::ifstream config;

    // filename = "./conf/" + filename;  // hardcoded path?
    config.open(filename.c_str());
    if (!config.is_open()) {
        throw std::runtime_error("webserv: failed to open config file!");
    }

    std::vector<std::string> vector = {"root"};
    Block rootBlock = syntax(config, vector);
    if (rootBlock.get_name() == "Error")
        throw std::runtime_error("Error!");
    config.close();
    std::cout << std::endl;
    // rootBlock.printTree(0);
    return rootBlock;
}
