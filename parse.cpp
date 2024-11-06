#include "parse.hpp"

namespace testCompiler {
    namespace parse {
        bool isOK = true;
        int wordIndex = 0;
        std::vector<lexer::Word> wordList;

        SyntaxTree* root = new SyntaxTree();

        SyntaxTree* parseRun(std::vector<lexer::Word>& _wordList) {
            wordList = _wordList;
            isOK = program();
            return root;
        }

        std::ostream &operator<<(std::ostream &os, SyntaxTree& x) {
            os<< "Type:\t";
			switch (x.type) {
            case Word:
                os<< "Word\t" << x.word;
                break;
            case PROGRAM:
                os<< "PROGRAM";
                break;
            case DECLARATION_LIST:
                os<< "DECLARATION_LIST";
                break;
            case DECLARATION_STAT:
                os<< "DECLARATION_STAT";
                break;
            case STATEMENT_LIST:
                os<< "STATEMENT_LIST";
                break;
            case STATEMENT:
                os<< "STATEMENT";
                break;
            case IF_STAT:
                os<< "IF_STAT";
                break;
            case ASSIGNMENT_STAT:
                os<< "ASSIGNMENT_STAT";
                break;
            case ASSIGNMENT_EXPRESSION:
                os<< "ASSIGNMENT_EXPRESSION";
                break;
            case BOOL_EXPRESSION:
                os<< "BOOL_EXPRESSION";
                break;
            case ARITHMETIC_EXPRESSION:
                os<< "ARITHMETIC_EXPRESSION";
                break;
            case TERM:
                os<< "TERM";
                break;
            case FACTOR:
                os<< "FACTOR";
                break;
            case WHILE_STAT:
                os<< "WHILE_STAT";
                break;
            case FOR_STAT:
                os<< "FOR_STAT";
                break;
            case READ_STAT:
                os<< "READ_STAT";
                break;
            case WRITE_STAT:
                os<< "WRITE_STAT";
                break;
            case COMPOUND_STAT:
                os<< "COMPOUND_STAT";
                break;
			default:
				os<< "[ERROR]UNKNOWN";
				break;
			}
			return os;
		}

        void outputSyntaxTree(std::ostream& os, SyntaxTree* root, int deep) {
            for(int j = 0; j < deep; ++ j) os << "\t|";
            os<< *root << std::endl;
            for(auto i : (root->childNode)) 
                outputSyntaxTree(os, i, deep + 1);
            return;
        }

        SyntaxTree* addNode(SyntaxTree* p, SyntaxType type) {
            return p->addNode(type);
        }

        SyntaxTree* addNode(SyntaxTree* p, SyntaxType type, lexer::Word word) {
            return p->addNode(type, word);
        }

        lexer::Word peek() {
            return wordIndex < (int)wordList.size() ? wordList[wordIndex] : lexer::Word();
        }
        
        lexer::Word getNextWord() {
            ++ wordIndex;
            return peek();
        }

        bool checkFactor(SyntaxTree* father) {
            bool isSuccess = true;
            lexer::Word nowWord = peek();
            SyntaxTree* current = addNode(father, FACTOR);
            
            if(nowWord.type == lexer::WordType::DELIMITER && nowWord.value == "(") {
                addNode(current, Word, nowWord);
                nowWord = getNextWord();
                isSuccess = checkArithmeticExpression(current);
                if(!isSuccess) return isSuccess;
                nowWord = peek();
                if(nowWord.type != lexer::WordType::DELIMITER || nowWord.value != ")") {
                    std::cerr<< "[ERROR PARSE] checkFactor"
                        << "\trow:" << nowWord.row << "\tcol:" << nowWord.col
                        << "\texpected \')\'" << std::endl;
                    assert(nowWord.type == lexer::WordType::DELIMITER && nowWord.value == ")");
                    return isSuccess = false;
                }
                nowWord = getNextWord();
            }else if(nowWord.type == lexer::WordType::NUMBER || nowWord.type == lexer::WordType::IDENTIFIER) {
                addNode(current, Word, nowWord);
                nowWord = getNextWord();
            }
            return isSuccess;
        }

        bool checkTerm(SyntaxTree* father) {
            bool isSuccess = true;
            lexer::Word nowWord = peek();
            SyntaxTree* current = addNode(father, TERM);

            isSuccess = checkFactor(current);
            if(!isSuccess) return isSuccess;

            nowWord = peek();
            while(nowWord.type == lexer::WordType::OPERATOR) {
                if(nowWord.value == "*" || nowWord.value == "/") {
                    addNode(current, Word, nowWord);
                    nowWord = getNextWord();
                    isSuccess = checkFactor(current);
                    if(!isSuccess) return isSuccess;
                    nowWord = peek();
                }else break;
            }
            return isSuccess;
        }

        bool checkAssignmentStat(SyntaxTree* father) {
            bool isSuccess = true;
            lexer::Word nowWord = peek();
            SyntaxTree* current = addNode(father, ASSIGNMENT_STAT);

            if(nowWord.type != lexer::WordType::IDENTIFIER) {
                std::cerr<< "[ERROR PARSE] checkAssignmentStat"
                    << "\trow:" << nowWord.row << "\tcol:" << nowWord.col
                    << "\texpected IDENTIFIER" << std::endl;
                assert(nowWord.type == lexer::WordType::IDENTIFIER);
                return isSuccess = false;
            }

            isSuccess = checkAssignmentExpression(current);
            if(!isSuccess) return isSuccess;

            nowWord = peek();
            if(nowWord.type != lexer::WordType::DELIMITER || nowWord.value != ";") {
                std::cerr<< "[ERROR PARSE] checkAssignmentStat"
                    << "\trow:" << nowWord.row << "\tcol:" << nowWord.col
                    << "\texpected \';\'" << std::endl;
                assert(nowWord.type == lexer::WordType::DELIMITER && nowWord.value == ";");
                return isSuccess = false;
            }
            // nowWord = getNextWord();
            return isSuccess;
        }

        bool checkAssignmentExpression(SyntaxTree* father) {
            bool isSuccess = true;
            lexer::Word nowWord = peek();
            SyntaxTree* current = addNode(father, ASSIGNMENT_EXPRESSION);

            if(nowWord.type == lexer::WordType::IDENTIFIER) {
                addNode(current, Word, nowWord);
                nowWord = getNextWord();
                if(nowWord.type == lexer::WordType::OPERATOR && nowWord.value == "=") {
                    addNode(current, Word, nowWord);
                    nowWord = getNextWord();
                    isSuccess = checkArithmeticExpression(current);
                    if(!isSuccess) return isSuccess;
                    return isSuccess;
                }
            }
            // nowWord = getNextWord();
            return isSuccess;
        }

        bool checkArithmeticExpression(SyntaxTree* father) {
            bool isSuccess = true;
            lexer::Word nowWord = peek();
            SyntaxTree* current = addNode(father, ARITHMETIC_EXPRESSION);

            isSuccess = checkTerm(current);
            if(!isSuccess) return isSuccess;

            nowWord = peek();
            while(nowWord.type == lexer::WordType::OPERATOR) {
                if(nowWord.value == "+" || nowWord.value == "-") {
                    addNode(current, Word, nowWord);
                    nowWord = getNextWord();
                    isSuccess = checkTerm(current);
                    if(!isSuccess) return isSuccess;
                    nowWord = peek();
                }else break;
            }
            return isSuccess;
        }

        bool checkBoolExpression(SyntaxTree* father) {
            bool isSuccess = true;
            lexer::Word nowWord = peek();
            SyntaxTree* current = addNode(father, BOOL_EXPRESSION);

            isSuccess = checkArithmeticExpression(current);
            if(!isSuccess) return isSuccess;

            nowWord = peek();
            if(nowWord.type == lexer::WordType::OPERATOR) {
                if(nowWord.value == "<") {
                    addNode(current, Word, nowWord);
                }else if(nowWord.value == ">") {
                    addNode(current, Word, nowWord);
                }else if(nowWord.value == "==") {
                    addNode(current, Word, nowWord);
                }else if(nowWord.value == "<=") {
                    addNode(current, Word, nowWord);
                }else if(nowWord.value == ">=") {
                    addNode(current, Word, nowWord);
                }else if(nowWord.value == "!=") {
                    addNode(current, Word, nowWord);
                }else {
                    std::cerr<< "[ERROR PARSE] checkBoolExpression"
                        << "\trow:" << nowWord.row << "\tcol:" << nowWord.col
                        << "\texpected CMP OPERATOR" << std::endl;
                    assert(false);
                    return isSuccess = false;
                }
            }else {
                std::cerr<< "[ERROR PARSE] checkBoolExpression"
                    << "\trow:" << nowWord.row << "\tcol:" << nowWord.col
                    << "expected CMP OPERATOR" << std::endl;
                assert(nowWord.type == lexer::WordType::OPERATOR);
                return isSuccess = false;
            }
            nowWord = getNextWord();
            isSuccess = checkArithmeticExpression(current);
            if(!isSuccess) return isSuccess;
            return isSuccess;
        }

        bool checkJudge(SyntaxTree* father) {
            bool isSuccess = true;
            lexer::Word nowWord = peek();

            if(nowWord.type != lexer::WordType::DELIMITER || nowWord.value != "(") {
                std::cerr<< "[ERROR PARSE] checkJudge"
                    << "\trow:" << nowWord.row << "\tcol:" << nowWord.col
                    << "expected \'(\'" << std::endl;
                assert(nowWord.type == lexer::WordType::IDENTIFIER && nowWord.value == "(");
                return isSuccess = false;
            }
            addNode(father, Word, nowWord);
            
            nowWord = getNextWord();
            isSuccess = checkBoolExpression(father);
            if(!isSuccess) return isSuccess;
            
            nowWord = peek();
            if(nowWord.type != lexer::WordType::DELIMITER || nowWord.value != ")") {
                std::cerr<< "[ERROR PARSE] checkJudge"
                    << "\trow:" << nowWord.row << "\tcol:" << nowWord.col
                    << "expected \')\'" << std::endl;
                assert(nowWord.type == lexer::WordType::IDENTIFIER &&  nowWord.value == ")");
                return isSuccess = false;
            }
            addNode(father, Word, nowWord);
            nowWord = getNextWord();
            return isSuccess;
        }

        bool checkIfStat(SyntaxTree* father) {
            bool isSuccess = true;
            lexer::Word nowWord = peek();
            SyntaxTree* current = addNode(father, IF_STAT);
            addNode(current, Word, nowWord);

            nowWord = getNextWord();
            isSuccess = checkJudge(current);
            if(!isSuccess) return isSuccess;
            
            nowWord = peek();
            isSuccess = checkStatement(current);
            if(!isSuccess) return isSuccess;

            nowWord = getNextWord();
            if(nowWord.type == lexer::WordType::KEYWORD && nowWord.value == "else") {
                addNode(current, Word, nowWord);
                nowWord = getNextWord();
                isSuccess = checkStatement(current);
                if(!isSuccess) return isSuccess;
            }
            nowWord = peek();
            return isSuccess;
        }

        bool checkWhileStat(SyntaxTree* father) {
            bool isSuccess = true;
            lexer::Word nowWord = peek();
            SyntaxTree* current = addNode(father, WHILE_STAT);
            addNode(current, Word, nowWord);

            nowWord = getNextWord();
            isSuccess = checkJudge(current);
            if(!isSuccess) return isSuccess;
            
            nowWord = peek();
            isSuccess = checkStatement(current);
            if(!isSuccess) return isSuccess;
            // nowWord = getNextWord();
            return isSuccess;
        }

        bool checkForStat(SyntaxTree* father) {
            bool isSuccess = true;
            lexer::Word nowWord = peek();
            SyntaxTree* current = addNode(father, FOR_STAT);
            addNode(current, Word, nowWord);

            nowWord = getNextWord();
            if(nowWord.type != lexer::WordType::DELIMITER || nowWord.value != "(") {
                std::cerr<< "[ERROR PARSE] checkFor"
                    << "\trow:" << nowWord.row << "\tcol:" << nowWord.col
                    << "\texpected \'(\'" << std::endl;
                assert(nowWord.type == lexer::WordType::IDENTIFIER &&  nowWord.value == "(");
                return isSuccess = false;
            }
            addNode(current, Word, nowWord);

            nowWord = getNextWord();
            isSuccess = checkAssignmentExpression(current);
            if(!isSuccess) return isSuccess;

            nowWord = peek();
             if(nowWord.type != lexer::WordType::DELIMITER || nowWord.value != ";") {
                std::cerr<< "[ERROR PARSE] checkFor"
                    << "\trow:" << nowWord.row << "\tcol:" << nowWord.col
                    << "\texpected \';\'" << std::endl;
                assert(nowWord.type == lexer::WordType::IDENTIFIER &&  nowWord.value == ";");
                return isSuccess = false;
            }

            nowWord = getNextWord();
            isSuccess = checkBoolExpression(current);
            if(!isSuccess) return isSuccess;

            nowWord = peek();
             if(nowWord.type != lexer::WordType::DELIMITER || nowWord.value != ";") {
                std::cerr<< "[ERROR PARSE] checkFor"
                    << "\trow:" << nowWord.row << "\tcol:" << nowWord.col
                    << "\texpected \';\'" << std::endl;
                assert(nowWord.type == lexer::WordType::DELIMITER &&  nowWord.value == ";");
                return isSuccess = false;
            }

            nowWord = getNextWord();
            isSuccess = checkAssignmentExpression(current);
            if(!isSuccess) return isSuccess;

            nowWord = peek();
            if(nowWord.type != lexer::WordType::DELIMITER || nowWord.value != ")") {
                std::cerr<< "[ERROR PARSE] checkFor"
                    << "\trow:" << nowWord.row << "\tcol:" << nowWord.col
                    << "\texpected \')\'" << std::endl;
                assert(nowWord.type == lexer::WordType::DELIMITER &&  nowWord.value == ")");
                return isSuccess = false;
            }
            addNode(current, Word, nowWord);

            nowWord = getNextWord();
            isSuccess = checkStatement(current);
            if(!isSuccess) return isSuccess;
            nowWord = peek();
            return isSuccess;
        }

        bool checkReadStat(SyntaxTree* father) {
            bool isSuccess = true;
            lexer::Word nowWord = peek();
            SyntaxTree* current = addNode(father, READ_STAT);
            addNode(current, Word, nowWord);

            nowWord = getNextWord();
            if(nowWord.type != lexer::WordType::IDENTIFIER) {
                std::cerr<< "[ERROR PARSE] checkRead"
                    << "\trow:" << nowWord.row << "\tcol:" << nowWord.col
                    << "\texpected IDENTIFIER" << std::endl;
                assert(nowWord.type == lexer::WordType::IDENTIFIER);
                return isSuccess = false;
            }
            addNode(current, Word, nowWord);

            nowWord = getNextWord();
            if(nowWord.type != lexer::WordType::DELIMITER || nowWord.value != ";") {
                std::cerr<< "[ERROR PARSE] checkReadStat"
                    << "\trow:" << nowWord.row << "\tcol:" << nowWord.col
                    << "\texpected \';\'" << std::endl;
                assert(nowWord.type == lexer::WordType::DELIMITER && nowWord.value == ";");
                return isSuccess = false;
            }
            addNode(current, Word, nowWord);
            // nowWord = getNextWord();
            return isSuccess;
        }

        bool checkWriteStat(SyntaxTree* father) {
            bool isSuccess = true;
            lexer::Word nowWord = peek();
            SyntaxTree* current = addNode(father, WRITE_STAT);
            addNode(current, Word, nowWord);

            nowWord = getNextWord();
            isSuccess = checkArithmeticExpression(current);
            if(!isSuccess) return isSuccess;

            nowWord = peek();
            if(nowWord.type != lexer::WordType::DELIMITER || nowWord.value != ";") {
                std::cerr<< "[ERROR PARSE] checkWriteStat"
                    << "\trow:" << nowWord.row << "\tcol:" << nowWord.col
                    << "\texpected \';\'" << std::endl;
                assert(nowWord.type == lexer::WordType::DELIMITER && nowWord.value == ";");
                return isSuccess = false;
            }
            addNode(current, Word, nowWord);
            // nowWord = getNextWord();
            return isSuccess;
        }

        bool checkCompoundStat(SyntaxTree* father) {
            bool isSuccess = true;
            lexer::Word nowWord = peek();
            SyntaxTree* current = addNode(father, COMPOUND_STAT);
            addNode(current, Word, nowWord);

            nowWord = getNextWord();
            isSuccess = checkStatementList(current);

            nowWord = peek();
            if(nowWord.type != lexer::WordType::DELIMITER || nowWord.value != "}") {
                std::cerr<< "[ERROT PARSE] program"
                    << "\trow:" << nowWord.row << "\tcol:" << nowWord.col
                    << "\texpected \'}\'" << std::endl;
                assert(nowWord.type != lexer::WordType::DELIMITER && nowWord.value != "}");
                return isSuccess = false;
            }
            addNode(current, Word, nowWord);
            // nowWord = getNextWord();            
            return isSuccess;
        }

        bool checkStatement(SyntaxTree* father) {
            bool isSuccess = true;
            lexer::Word nowWord = peek();
            SyntaxTree* current = addNode(father, STATEMENT);

            if(nowWord.type == lexer::WordType::KEYWORD) {
                if(nowWord.value == "if") {
                    isSuccess = checkIfStat(current);
                    if(!isSuccess) return isSuccess;
                }else if(nowWord.value == "while") {
                    isSuccess = checkWhileStat(current);
                    if(!isSuccess) return isSuccess;
                }else if(nowWord.value == "for") {
                    isSuccess = checkForStat(current);
                    if(!isSuccess) return isSuccess;
                }else if(nowWord.value == "read") {
                    isSuccess = checkReadStat(current);
                    if(!isSuccess) return isSuccess;
                }else if(nowWord.value == "write") {
                    isSuccess = checkWriteStat(current);
                    if(!isSuccess) return isSuccess;
                }
            }else if(nowWord.type == lexer::WordType::DELIMITER) {
                if(nowWord.value == "{") {
                    isSuccess = checkCompoundStat(current);
                    if(!isSuccess) return isSuccess;
                }else if(nowWord.value == ";") {
                    return isSuccess;
                }
            }else if(nowWord.type == lexer::WordType::IDENTIFIER) {
                isSuccess = checkAssignmentStat(current);
                if(!isSuccess) return isSuccess;
            }
            return isSuccess;
        }

        bool checkStatementList(SyntaxTree* father) {
            bool isSuccess = true, isEmpty = true;
            lexer::Word nowWord = peek();
            SyntaxTree* current = addNode(father, STATEMENT_LIST);

            while(nowWord.type != lexer::WordType::DELIMITER || nowWord.value != "}") {
                isSuccess = checkStatement(current);
                if(!isSuccess) return isSuccess;
                nowWord = getNextWord();
            }
            return isSuccess;
        }
        
        bool checkDeclarationStat(SyntaxTree* father) {
            bool isSuccess = true;
            lexer::Word nowWord = peek();
            SyntaxTree* current = addNode(father, DECLARATION_STAT);
            addNode(current, Word, nowWord);
            nowWord = getNextWord();
            // var name
            if(nowWord.type != lexer::WordType::IDENTIFIER) {
                std::cerr<< "[ERROR PARSE] checkDeclarationStat"
                    << "\trow:" << nowWord.row << "\tcol:" << nowWord.col
                    << "\texpected IDENTIFIER" << std::endl;
                assert(nowWord.type == lexer::WordType::IDENTIFIER);
                return isSuccess = false;
            }
            addNode(current, Word, nowWord);
            // =
            nowWord = getNextWord();
            if(nowWord.type == lexer::WordType::OPERATOR && nowWord.value == "=") {
                addNode(current, Word, nowWord);
                // number
                nowWord = getNextWord();
                if(nowWord.type != lexer::WordType::NUMBER) {
                    std::cerr<< "[ERROR PARSE] checkDeclarationStat"
                        << "\trow:" << nowWord.row << "\tcol:" << nowWord.col
                        << "expected NUMBER" << std::endl;
                    assert(nowWord.type == lexer::WordType::NUMBER);
                    return isSuccess = false;
                }
                addNode(current, Word, nowWord);
                nowWord = getNextWord();
            }
            // ;
            if(nowWord.type != lexer::WordType::DELIMITER || nowWord.value != ";") {
                std::cerr<< "[ERROR PARSE] checkDeclarationStat"
                        << "\trow:" << nowWord.row << "\tcol:" << nowWord.col
                        << "\texpected \';\'" << std::endl;
                assert(nowWord.type == lexer::WordType::NUMBER && nowWord.value == ";");
                return isSuccess = false;
            }
            addNode(current, Word, nowWord);
            nowWord = getNextWord();
            return isSuccess;
        }

        bool checkDeclarationList(SyntaxTree* father) {
            bool isSuccess = true, isEmpty = true;
            lexer::Word nowWord = peek();
            SyntaxTree* current;

            while(nowWord.type == lexer::WordType::KEYWORD && nowWord.value == "int") {
                if(isEmpty) current = addNode(father, DECLARATION_LIST), isEmpty = false;
                isSuccess = checkDeclarationStat(current);
                if(!isSuccess) return isSuccess;
                nowWord = peek();
            }
            return isSuccess;
        }

        bool program() {
            bool isSuccess = true;
            lexer::Word nowWord = peek();
            root->type = PROGRAM;

            if(nowWord.type != lexer::WordType::DELIMITER || nowWord.value != "{") {
                std::cerr<< "[ERROT PARSE] program"
                    << "\trow:" << nowWord.row << "\tcol:" << nowWord.col
                    << "\texpected \'{\'" << std::endl;
                assert(nowWord.type != lexer::WordType::DELIMITER && nowWord.value != "{");
                return isSuccess = false;
            }
            addNode(root, Word, nowWord);
            
            nowWord = getNextWord();
            isSuccess = checkDeclarationList(root);
            if(!isSuccess) return isSuccess;

            isSuccess = checkStatementList(root);
            if(!isSuccess) return isSuccess;

            nowWord = peek();
            if(nowWord.type != lexer::WordType::DELIMITER || nowWord.value != "}") {
                std::cerr<< "[ERROT PARSE] program"
                    << "\trow:" << nowWord.row << "\tcol:" << nowWord.col
                    << "\texpected \'}\'" << std::endl;
                assert(nowWord.type != lexer::WordType::DELIMITER && nowWord.value != "}");
                return isSuccess = false;
            }
            addNode(root, Word, nowWord);
            return isSuccess;
        }
    }
}