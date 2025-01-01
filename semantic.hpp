#ifndef SEMANTIC_HPP
#define SEMANTIC_HPP

#include <map>
#include <string>
#include "parse.hpp"

namespace testCompiler {
    namespace semantic {
        struct Variable {
            std::string name;
            int address;
            int value;
            bool haveValue;

            void setValue(int value);
            int getValue();
            bool checkValue();
            Variable(std::string _name, int _address);
            Variable(std::string _name, int _address, int _value);
        };
        

        extern bool isOK;
        extern int address;
        extern parse::SyntaxTree* root;
        extern std::map<std::string, Variable*> variableTable;
        extern std::string middleCode;
        extern int labID;

        int getIDaddress(std::string name);
        void init(parse::SyntaxTree* _root);
        std::string semanticRun(parse::SyntaxTree* _root);
        bool semanticAnalysis(parse::SyntaxTree* p);
        bool program(parse::SyntaxTree* p);
        bool declarationList(parse::SyntaxTree* p);
        bool statementList(parse::SyntaxTree* p);
        bool statement(parse::SyntaxTree* p);
        bool ifStat(parse::SyntaxTree* p);
        bool whileStat(parse::SyntaxTree* p);
        bool forStat(parse::SyntaxTree* p);
        bool readStat(parse::SyntaxTree* p);
        bool writeStat(parse::SyntaxTree* p);
        bool compoundStat(parse::SyntaxTree* p);
        bool assignmentStat(parse::SyntaxTree* p);
        bool assignmentExpression(parse::SyntaxTree* p);
        bool boolExpression(parse::SyntaxTree* p);
        bool arithmeticExpression(parse::SyntaxTree*p);
        bool term(parse::SyntaxTree* p);
        bool factor(parse::SyntaxTree* p);
    }
}

#endif