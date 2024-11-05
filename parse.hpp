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
            EXPRESSION_STAT,    // 表达式序列
            EXPRESSION,         // 表达式
            BOOL_EXPRESSION,    // 布尔表达式
            ADD_EXPRESSION,     // 算数表达式
            TERM,               // 运算项
            WHILE_STAT,         // while循环
            FOR_STAT,           // for循环
            READ_STAT,          // read语句
            WRITE_STAT,         // write语句
        };

        struct SyntaxTree {
            SyntaxType type;
            lexer::Word wrod;
            std::vector<SyntaxTree*> childNode;

            SyntaxTree* addNode(SyntaxType type) {
                SyntaxTree* child = new SyntaxTree();
                child->type = type;
                childNode.push_back(child);
                return child;
            }

            SyntaxTree* addNode(SyntaxType type, lexer::Word wrod) {
                SyntaxTree* child = new SyntaxTree();
                child->type = type, child->wrod = wrod;
                childNode.push_back(child);
                return child;
            }
        };
        SyntaxTree* addNode(SyntaxTree* p, SyntaxType type);
        SyntaxTree* addNode(SyntaxTree* p, SyntaxType type, lexer::Word wrod);

        extern SyntaxTree* root;
        
        SyntaxTree* parseRun(std::vector<lexer::Word>& _wordList);
        
        lexer::Word peek();
        lexer::Word getNextWord();
        bool checkTerm(SyntaxTree* father);
        bool checkExpression(SyntaxTree* father);
        bool checkAddExpression(SyntaxTree* father);
        bool checkBoolExpression(SyntaxTree* father);
        bool checkJudge(SyntaxTree* father);
        bool checkIfStat(SyntaxTree* father);
        bool checkWhile(SyntaxTree* father);
        bool checkFor(SyntaxTree* father);
        bool checkRead(SyntaxTree* father);
        bool checkWrite(SyntaxTree* father);
        bool checkCompound(SyntaxTree* father);
        bool checkExpressionStat(SyntaxTree* father);
        bool checkStatement(SyntaxTree* father);
        bool checkStatementList(SyntaxTree* father);
        bool checkDeclarationStat(SyntaxTree* father);
        bool checkDeclarationList(SyntaxTree* father);
        bool program();
    }
}
#endif