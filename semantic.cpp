#include "semantic.hpp"

namespace testCompiler {
    namespace semantic {
        Variable::Variable(std::string _name, int _address) : name(_name), address(_address), haveValue(false) {}
        Variable::Variable(std::string _name, int _address, int _value) : name(_name), address(_address), value(_value), haveValue(true) {}

        void Variable::setValue(int _value) {
            value = _value;
            haveValue = true;
        }
        int Variable::getValue() {
            if (haveValue)
                return value;
            else {
                std::cerr<< "[ERROR SEMANTIC] Variable " << name << " has no value." << std::endl;
                assert(haveValue);
                return -1;
            }
        }
        bool Variable::checkValue() {
            return haveValue;
        }

        bool isOK = true;
        int address = 0;
        parse::SyntaxTree* root;
        std::map<std::string, Variable*> variableTable;
        std::string middleCode;
        int labID = -1;

        int getIDaddress(std::string name) {
            if(variableTable.find(name) != variableTable.end()) {
                return variableTable[name]->address;
            }else {
                return -1;
            }
        }

        void init(parse::SyntaxTree* _root) {
            root = _root;
            address = 0;
            variableTable.clear();
            middleCode.clear();
        }

        std::string semanticRun(parse::SyntaxTree* _root) {
            init(_root);
            parse::outputSyntaxTree(std::cout, root, 0);
            std::cout<<std::endl;
            isOK = semanticAnalysis(root);
            return middleCode;
        }

        bool semanticAnalysis(parse::SyntaxTree* p) {
            if(p == nullptr || p->type != parse::PROGRAM) {
                std::cerr<< "[ERROR SEMANTIC] Program entrance not found" << std::endl;
                assert(p != nullptr && p->type == parse::PROGRAM);
                return isOK = false;
            }
            return program(p);
        }

        bool program(parse::SyntaxTree* p) {
            for(auto &i : (p->childNode)) {
                switch (i->type) {
                case parse::DECLARATION_LIST:
                    if(!declarationList(i)) return isOK = false;
                    break;
                case parse::STATEMENT_LIST:
                    if(!statementList(i)) return isOK = false;
                    break;
                case parse::Word:
                    if(i->word.value == "{" || i->word.value == "}") 
                        break;
                default:
                    std::cerr<< "[ERROR SEMANTIC] Program error\t" << (*i) << std::endl;
                    break;
                }
            }
            return true;
        }

        bool declarationList(parse::SyntaxTree* p) {
            std::string varName;
            int varValue;
            for(auto &i : (p->childNode)) {
                switch (i->type) {
                case parse::DECLARATION_STAT:
                    if(getIDaddress(i->childNode[1]->word.value) != -1) {
                        std::cerr<< "[ERROR SEMANTIC] Variable \"" << i->childNode[1]->word.value << "\" has been declared."
                            << "\trow:" << i->childNode[1]->word.row << "\tcol:" << i->childNode[1]->word.col << std::endl;
                        assert(getIDaddress(i->childNode[1]->word.value) != -1);
                        return isOK = false;
                    }
                    varName = i->childNode[1]->word.value;
                    if(i->childNode.size() == 3) {
                        variableTable[varName] = new Variable(varName, address ++);
                    } else if(i->childNode.size() == 5) {
                        varValue = std::stoi(i->childNode[3]->word.value);
                        variableTable[varName] = new Variable(varName, address ++, varValue);
                    } else {
                        std::cerr<< "[ERROR SEMANTIC] Declaration statement error"
                            << "\trow:" << i->childNode[1]->word.row << "\tcol:" << i->childNode[1]->word.col << std::endl;
                        assert(i->childNode.size() == 3 || i->childNode.size() == 5);
                        return isOK = false;
                    }
                    break;
                default:
                    std::cerr<< "[ERROR SEMANTIC] Declaration list error" << std::endl;
                    parse::outputSyntaxTree(std::cerr, i, 0);
                    assert(i->type == parse::DECLARATION_STAT);
                    break;
                }
            }
            return true;
        }

        bool statementList(parse::SyntaxTree* p) {
            for(auto &i : (p->childNode)) {
                switch (i->type) {
                case parse::STATEMENT:
                    if(!statement(i)) return isOK = false;
                    break;
                default:
                    std::cerr<< "[ERROR SEMANTIC] Statement list error" << std::endl;
                    parse::outputSyntaxTree(std::cerr, i, 0);
                    assert(i->type == parse::STATEMENT);
                    return isOK = false;
                    break;
                }
            }
            return true;
        }

        bool statement(parse::SyntaxTree* p) {
            for(auto &i : (p->childNode)) {
                switch (i->type) {
                case parse::IF_STAT:
                    if(!ifStat(i)) return isOK = false;
                    break;
                case parse::WHILE_STAT:
                    if(!whileStat(i)) return isOK = false;
                    break;
                case parse::FOR_STAT:
                    if(!forStat(i)) return isOK = false;
                    break;
                case parse::READ_STAT:
                    if(!readStat(i)) return isOK = false;
                    break;
                case parse::WRITE_STAT:
                    if(!writeStat(i)) return isOK = false;
                    break;
                case parse::COMPOUND_STAT:
                    if(!compoundStat(i)) return isOK = false;
                    break;
                case parse::ASSIGNMENT_STAT:
                    if(!assignmentStat(i)) return isOK = false;
                    break;
                case parse::Word:
                    if(i->word.value == ";") 
                        break;
                default:
                    std::cerr<< "[ERROR SEMANTIC] Statement error" << std::endl;
                    parse::outputSyntaxTree(std::cerr, i, 0);
                    assert(false);
                    return isOK = false;
                    break;
                }
            }
            return true;
        }

        bool ifStat(parse::SyntaxTree* p) {
            int st = 0, lab1, lab2;
            for(auto &i : (p->childNode)) {
                switch (i->type) {
                case parse::BOOL_EXPRESSION:
                    if(!boolExpression(i)) return isOK = false;
                    lab1 = ++ labID;
                    middleCode += "\tBRF\t" + std::to_string(lab1) + "\n";
                    break;
                case parse::STATEMENT:
                    if(!statement(i)) return isOK = false;
                    if(st == 0) {
                        lab2 = ++ labID;
                        middleCode += "\tBRF\t" + std::to_string(lab2) + "\n";
                        middleCode += "LABEL\t" + std::to_string(lab1) + "\n";
                        ++ st;
                    }else {
                        middleCode += "LABEL\t" + std::to_string(lab2) + "\n";
                    }
                    break;
                case parse::Word:
                    if(i->word.value == "if" || i->word.value == "(" || i->word.value == ")" || i->word.value == "else")
                        break;
                default:
                    std::cerr<< "[ERROR SEMANTIC] If statement error" << std::endl;
                    parse::outputSyntaxTree(std::cerr, i, 0);
                    assert(false);
                    return isOK = false;
                    break;
                }
            }
            return true;
        }

        bool whileStat(parse::SyntaxTree* p) {
            int lab1, lab2;
            for(auto &i : (p->childNode)) {
                switch (i->type) {
                    case parse::BOOL_EXPRESSION:
                        if(!boolExpression(i)) return isOK = false;
                        lab2 = ++ labID;
                        middleCode += "\tBRF\t" + std::to_string(lab2) + "\n";
                        break;
                    case parse::STATEMENT:
                        if(!statement(i)) return isOK = false;
                        middleCode += "\tBR\t" + std::to_string(lab1) + "\n";
                        middleCode += "LABEL\t" + std::to_string(lab2) + "\n";
                        break;
                    case parse::Word:
                        if(i->word.type == lexer::KEYWORD) {
                            if(i->word.value == "while") {
                                lab1 = ++ labID;
                                middleCode += "LABEL\t" + std::to_string(lab1) + "\n";
                            }else {
                                std::cerr<< "[ERROR SEMANTIC] While statement error" << std::endl;
                                parse::outputSyntaxTree(std::cerr, i, 0);
                                assert(false);
                                return isOK = false;
                            }
                        } else if(i->word.value == "(" || i->word.value == ")");
                        else {
                            std::cerr<< "[ERROR SEMANTIC] While statement error" << std::endl;
                            parse::outputSyntaxTree(std::cerr, i, 0);
                            assert(false);
                            return isOK = false;
                        }
                        break;
                    default:
                        std::cerr<< "[ERROR SEMANTIC] While statement error" << std::endl;
                        parse::outputSyntaxTree(std::cerr, i, 0);
                        assert(false);
                        return isOK = false;
                        break;
                }
            }
            return true;
        }

        bool forStat(parse::SyntaxTree* p) {
            int st = 0, lab1, lab2, lab3, lab4;
            parse::outputSyntaxTree(std::clog, p, 0);
            for(auto &i : (p->childNode)) {
                switch (i->type) {
                case parse::ASSIGNMENT_EXPRESSION:
                    if(!assignmentExpression(i)) return isOK = false;
                    middleCode += "\tPOP\n";
                    if(st == 0) {
                        ++ st;
                        lab1 = ++ labID;
                        middleCode += "LABEL\t" + std::to_string(lab1) + "\n";
                    }else if(st == 1) 
                        middleCode += "\tBR\t" + std::to_string(lab1) + "\n";
                    break;
                case parse::BOOL_EXPRESSION:
                    if(!boolExpression(i)) return isOK = false;
                    lab2 = ++ labID;
                    middleCode += "\tBRF\t" + std::to_string(lab2) + "\n";
                    lab3 = ++ labID;
                    middleCode += "\tBR\t" + std::to_string(lab3) + "\n";
                    lab4 = ++ labID;
                    middleCode += "LABEL\t" + std::to_string(lab4) + "\n";
                    break;
                case parse::STATEMENT:
                    if(!statement(i)) return isOK = false;
                    middleCode += "\tBR\t" + std::to_string(lab4) + "\n";
                    middleCode += "LABEL\t" + std::to_string(lab2) + "\n";
                    break;
                case parse::Word:
                    if(i->word.type == lexer::DELIMITER) {
                        if(i->word.value == ";");
                        else if(i->word.value == ")") {
                            middleCode += "LABEL\t" + std::to_string(lab3) + "\n";
                        } else if(i->word.value == "()");
                    }else if(i->word.value == "for");
                    else {
                        std::cerr<< "[ERROR SEMANTIC] For statement error" << std::endl;
                        parse::outputSyntaxTree(std::cerr, i, 0);
                        assert(false);
                        return isOK = false;
                    }
                    break;
                default:
                    std::cerr<< "[ERROR SEMANTIC] For statement error" << std::endl;
                    parse::outputSyntaxTree(std::cerr, i, 0);
                    assert(false);
                    return isOK = false;
                    break;
                }
            }
            return true;
        }

        bool readStat(parse::SyntaxTree* p) {
            int varAddress;
            for(auto &i : (p->childNode)) {
                switch (i->type) {
                    case parse::Word:
                        if(i->word.type == lexer::IDENTIFIER) {
                            varAddress = getIDaddress(i->word.value);
                            if(varAddress == -1) {
                                std::cerr<< "[ERROR SEMANTIC] Read statement IDENTIFIER error" << std::endl;
                                parse::outputSyntaxTree(std::cerr, i, 0);
                                assert(false);
                                return isOK = false;
                            }
                            middleCode += "\tIN\n";
                            middleCode += "\tSTO\t" + std::to_string(varAddress) + "\n";
                            middleCode += "\tPOP\n";
                        }else if((i->word.type == lexer::KEYWORD && i->word.value == "read") || i->word.value == ";"); 
                        else {
                            std::cerr<< "[ERROR SEMANTIC] Read statement IDENTIFIER type error" << std::endl;
                            parse::outputSyntaxTree(std::cerr, i, 0);
                            assert(false);
                            return isOK = false;
                        }
                        break;
                    default:
                        std::cerr<< "[ERROR SEMANTIC] Read statement error" << std::endl;
                        parse::outputSyntaxTree(std::cerr, i, 0);
                        assert(false);
                        return isOK = false;
                        break;
                }
            }
            return true;
        }

        bool writeStat(parse::SyntaxTree* p) {
            for(auto &i : (p->childNode)) {
                switch (i->type) {
                case parse::ARITHMETIC_EXPRESSION:
                    if(!arithmeticExpression(i)) return isOK = false;
                    middleCode += "\tOUT\n";
                    break;
                case parse::Word:
                    if((i->word.type == lexer::KEYWORD && i->word.value == "write") || i->word.value == ";");
                    else {
                        std::cerr<< "[ERROR SEMANTIC] Write statement error" << std::endl;
                        parse::outputSyntaxTree(std::cerr, i, 0);
                        assert(false);
                        return isOK = false;
                    }
                    break;     
                default:
                    std::cerr<< "[ERROR SEMANTIC] Write statement error" << std::endl;
                    parse::outputSyntaxTree(std::cerr, i, 0);
                    assert(false);
                    return isOK = false;
                    break;
                }
            }
            return true;
        }

        bool compoundStat(parse::SyntaxTree* p) {
            for(auto &i : (p->childNode)) {
                switch (i->type) {
                    case parse::STATEMENT_LIST:
                        if(!statementList(i)) return isOK = false;
                        break;
                    case parse::Word:
                        if(i->word.value == "{" || i->word.value == "}");
                        else {
                            std::cerr<< "[ERROR SEMANTIC] Compound statement error" << std::endl;
                            parse::outputSyntaxTree(std::cerr, i, 0);
                            assert(false);
                            return isOK = false;
                        }
                        break;
                    default:
                        std::cerr<< "[ERROR SEMANTIC] Compound statement error" << std::endl;
                        parse::outputSyntaxTree(std::cerr, i, 0);
                        assert(false);
                        return isOK = false;
                        break;
                }
            }
            return true;
        }

        bool assignmentStat(parse::SyntaxTree* p) {
            for(auto &i : (p->childNode)) {
                switch (i->type) {
                    case parse::ASSIGNMENT_EXPRESSION:
                        if(!assignmentExpression(i)) return isOK = false;
                        break;
                    case parse::Word:
                        if(i->word.value == ";");
                        else {
                            std::cerr<< "[ERROR SEMANTIC] Assignment statement error" << std::endl;
                            parse::outputSyntaxTree(std::cerr, i, 0);
                            assert(false);
                            return isOK = false;
                        }
                        break;
                    default:
                        std::cerr<< "[ERROR SEMANTIC] Assignment statement error"<< std::endl;
                        parse::outputSyntaxTree(std::cerr, i, 0);
                        assert(false);
                        return isOK = false;
                        break;
                }
            }
            return true;
        }

        bool assignmentExpression(parse::SyntaxTree* p) {
            int varAddress;
            for(auto &i : (p->childNode)) {
                switch (i->type) {
                    case parse::ARITHMETIC_EXPRESSION:
                        if(!arithmeticExpression(i)) return isOK = false;
                        middleCode += "\tSTO\t" + std::to_string(varAddress) + "\n";
                        break;
                    case parse::Word:
                        if(i->word.type == lexer::IDENTIFIER) {
                            varAddress = getIDaddress(i->word.value);
                            if(varAddress == -1) {
                                std::cerr<< "[ERROR SEMANTIC] Assignment expression IDENTIFIER error" << std::endl;
                                parse::outputSyntaxTree(std::cerr, i, 0);
                                assert(false);
                                return isOK = false;
                            }
                        } else if(i->word.type == lexer::OPERATOR && i->word.value == "=");
                        else {
                            std::cerr<< "[ERROR SEMANTIC] Assignment expression error" << std::endl;
                            parse::outputSyntaxTree(std::cerr, i, 0);
                            assert(false);
                            return isOK = false;
                        }
                        break;
                    default:
                        std::cerr<< "[ERROR SEMANTIC] Assignment expression error" << std::endl;
                        parse::outputSyntaxTree(std::cerr, i, 0);
                        assert(false);
                        return isOK = false;
                        break;
                }
            }
            return true;
        }

        bool boolExpression(parse::SyntaxTree* p) {
            for(auto &i : (p->childNode)) {
                switch(i->type) {
                    case parse::ARITHMETIC_EXPRESSION:
                        if(!arithmeticExpression(i)) return isOK = false;
                        break;
                    case parse::Word:
                        if(i->word.type == lexer::OPERATOR) {
                            if(i->word.value == ">") {
                                middleCode += "\tGT\n";
                            }else if(i->word.value == "<") {
                                middleCode += "\tLES\n";
                            }else if(i->word.value == ">=") {
                                middleCode += "\tGE\n";
                            }else if(i->word.value == "<=") {
                                middleCode += "\tLE\n";
                            }else if(i->word.value == "==") {
                                middleCode += "\tEQ\n";
                            }else if(i->word.value == "!=") {
                                middleCode += "\tNOTEQ\n";
                            }else {
                                std::cerr<< "[ERROR SEMANTIC] Bool expression OPERATOR error : " << std::endl;
                                parse::outputSyntaxTree(std::cerr, i, 0);
                                assert(false);
                                return isOK = false;
                            }
                        }else {
                            std::cerr<< "[ERROR SEMANTIC] Bool expression OPERATOR type error" << std::endl;
                            parse::outputSyntaxTree(std::cerr, i, 0);
                            assert(false);
                            return isOK = false;
                        }
                        break;
                    default:
                        std::cerr<< "[ERROR SEMANTIC] Bool expression type error" << std::endl;
                        parse::outputSyntaxTree(std::cerr, i, 0);
                        assert(false);
                        return isOK = false;
                        break;  
                }
            }
            return true;
        }

        bool arithmeticExpression(parse::SyntaxTree*p) {
            for(auto &i : (p->childNode)) {
                switch(p->type) {
                    case parse::ARITHMETIC_EXPRESSION:
                        if(!arithmeticExpression(i)) return isOK = false;
                        break;
                    case parse::TERM:
                        if(!term(i)) return isOK = false;
                        break;
                    case parse::Word:
                        if(i->word.type == lexer::OPERATOR) {
                            if(i->word.value == "+") {
                                middleCode += "\tADD\n";
                            }else if(i->word.value == "-") {
                                middleCode += "\tSUB\n";
                            }else {
                                std::cerr<< "[ERROR SEMANTIC] Arithmetic expression OPERATOR error" << std::endl;
                                parse::outputSyntaxTree(std::cerr, i, 0);
                                assert(false);
                                return isOK = false;
                            }
                        }else {
                            std::cerr<< "[ERROR SEMANTIC] Arithmetic expression OPERATOR type error" << std::endl;
                            parse::outputSyntaxTree(std::cerr, i, 0);
                            assert(false);
                            return isOK = false;
                        }
                        break;
                    default:
                        std::cerr<< "[ERROR SEMANTIC] Arithmetic expression error" << std::endl;
                        parse::outputSyntaxTree(std::cerr, i, 0);
                        assert(false);
                        return isOK = false;
                        break;
                }
            }
            return true;
        }

        bool term(parse::SyntaxTree* p) {

            for(auto &i : (p->childNode)) {
                parse::outputSyntaxTree(std::cerr, root, 0);
                switch (i->type) {
                    case parse::TERM:
                        if(!term(i)) return isOK = false;
                        break;
                    case parse::FACTOR:
                        if(!factor(i)) return isOK = false;
                        break;
                    case parse::Word:
                        if(i->word.type == lexer::OPERATOR) {
                            if(i->word.value == "*") {
                                middleCode += "\tMULT\n";
                            }else if(i->word.value == "/") {
                                middleCode += "\tDIV\n";
                            }else {
                                std::cerr<< "[ERROR SEMANTIC] Term OPERATOR error "<< std::endl;
                                parse::outputSyntaxTree(std::cerr, i, 0);
                                assert(false);
                                return isOK = false;
                            }
                        }else {
                            std::cerr<< "[ERROR SEMANTIC] Term OPERATOR type error " << std::endl;
                            parse::outputSyntaxTree(std::cout, p, 0);
                            parse::outputSyntaxTree(std::cout, i, 0);
                            assert(false);
                            return isOK = false;
                        }
                        break;
                    default:
                        std::cerr<< "[ERROR SEMANTIC] Term error" << std::endl;
                        parse::outputSyntaxTree(std::cerr, i, 0);
                        assert(false);
                        return isOK = false;
                        break;
                }
            }
            return true;
        }

        bool factor(parse::SyntaxTree* p) {
            for(auto &i : (p->childNode)) {
                switch (i->type) {
                    case parse::ARITHMETIC_EXPRESSION:
                        if(!arithmeticExpression(i)) return isOK = false;
                        break;
                    case parse::Word:
                        if(i->word.type == lexer::DELIMITER) {
                            if(i->word.value == "(");
                            else if(i->word.value == ")");
                            else {
                                std::cerr<< "[ERROR SEMANTIC] Factor DELIMITER error : " << i->word << std::endl;
                                assert(false);
                                return isOK = false;
                            }
                        }else if(i->word.type == lexer::NUMBER) {
                            middleCode += "\tLOADI\t" + i->word.value + "\n";
                        }else if(i->word.type == lexer::IDENTIFIER) {
                            int varAddress = getIDaddress(i->word.value);
                            if(varAddress != -1) {
                                middleCode += "\tLOAD\t" + std::to_string(varAddress) + "\n";
                            }else {
                                std::cerr<< "[ERROR SEMANTIC] Factor IDENTIFIER error : " << i->word << std::endl;
                                assert(false);
                                return isOK = false;
                            }
                        }
                    default:
                        std::cerr<< "[ERROR SEMANTIC] Factor error\t" << (*i) << std::endl;
                        assert(false);
                        return isOK = false;
                        break;
                }
            }
            return true;
        }
    }
}