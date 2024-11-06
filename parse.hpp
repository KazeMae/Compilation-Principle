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
            Word,       // 词
            PROGRAM,    // 程序入口
            DECLARATION_LIST,   // 声明序列
            DECLARATION_STAT,   // 声明内容
            STATEMENT_LIST,     // 语句序列
            STATEMENT,          // 语句内容
            IF_STAT,            // IF语句
            ASSIGNMENT_STAT,    // 赋值语句
            ASSIGNMENT_EXPRESSION,  // 赋值表达式
            BOOL_EXPRESSION,    // 布尔表达式
            ARITHMETIC_EXPRESSION,     // 算数表达式
            TERM,               // 运算项
            FACTOR,             // 因子
            WHILE_STAT,         // while循环
            FOR_STAT,           // for循环
            READ_STAT,          // read语句
            WRITE_STAT,         // write语句
            COMPOUND_STAT       // 复合语句
        };

        struct SyntaxTree {
            SyntaxType type;
            lexer::Word word;
            std::vector<SyntaxTree*> childNode;

            SyntaxTree* addNode(SyntaxType type) {
                SyntaxTree* child = new SyntaxTree();
                child->type = type;
                childNode.push_back(child);
                return child;
            }

            SyntaxTree* addNode(SyntaxType type, lexer::Word word) {
                SyntaxTree* child = new SyntaxTree();
                child->type = type, child->word = word;
                childNode.push_back(child);
                return child;
            }
        };
        std::ostream &operator<<(std::ostream &os, SyntaxTree& x);
        SyntaxTree* addNode(SyntaxTree* p, SyntaxType type);
        SyntaxTree* addNode(SyntaxTree* p, SyntaxType type, lexer::Word word);
        extern SyntaxTree* root;

        void outputSyntaxTree(std::ostream& os, SyntaxTree* root, int deep);
        
        SyntaxTree* parseRun(std::vector<lexer::Word>& _wordList);
        
        lexer::Word peek();
        lexer::Word getNextWord();
        bool checkTerm(SyntaxTree* father);
        bool checkAssignmentStat(SyntaxTree* father);
        bool checkAssignmentExpression(SyntaxTree* father);
        bool checkArithmeticExpression(SyntaxTree* father);
        bool checkBoolExpression(SyntaxTree* father);
        bool checkJudge(SyntaxTree* father);
        bool checkIfStat(SyntaxTree* father);
        bool checkWhileStat(SyntaxTree* father);
        bool checkForStat(SyntaxTree* father);
        bool checkReadStat(SyntaxTree* father);
        bool checkWriteStat(SyntaxTree* father);
        bool checkCompoundStat(SyntaxTree* father);
        bool checkStatement(SyntaxTree* father);
        bool checkStatementList(SyntaxTree* father);
        bool checkDeclarationStat(SyntaxTree* father);
        bool checkDeclarationList(SyntaxTree* father);
        bool program();
    }
}
#endif