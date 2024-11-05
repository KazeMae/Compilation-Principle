#ifndef PARSE_HPP
#define PARSE_HPP
#include <iostream>
#include <vector>
#include <string>
#include "lexer.hpp"

namespace testCompiler {
    namespace parse {
        extern bool isOK;
        extern int wordIndex;
        extern std::vector<lexer::Word> wordList;
        
        enum SyntaxType{
            PROGRAM,    //程序入口
        };

        struct SyntaxTree {
            std::string data;
            std::vector<SyntaxTree*> childNode;
            bool addNode(const std::string& data) {
                SyntaxTree* child = new SyntaxTree();
                child->data = data;
                childNode.push_back(child);
                return true;
            }
        };
        extern SyntaxTree* root;
        
        void parseRun(std::vector<lexer::Word>& _wordList);
        
        lexer::Word peek();
        lexer::Word getNextWord();
        void getDeclarationList();
        void program();
    }
}
#endif