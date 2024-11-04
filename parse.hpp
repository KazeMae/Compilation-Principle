#ifndef PARSE_HPP
#define PARSE_HPP
#include <iostream>
#include <vector>
#include <string>
#include "lexer.hpp"

namespace testCompiler {
    namespace parse {
        // 变量
        struct Variable {
            std::string name;
            int address;
            Variable(const std::string& _name, int a);
        };
        
        extern std::vector<Variable> variableList;
        extern int labelp;
        extern bool isOK;
        // 语法分析
        void program(std::vector<lexer::Word>& wordList);
        // 定义变量
        bool defVariable(const std::string& name);
        // 获取变量地址
        int getVariableAddress(const std::string& name);

    }
}
#endif