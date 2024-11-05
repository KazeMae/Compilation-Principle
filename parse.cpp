#include "parse.hpp"

namespace testCompiler {
    namespace parse {
        bool isOK = true;
        int wordIndex = 0;
        std::vector<lexer::Word> wordList;

        SyntaxTree* root = new SyntaxTree();

        SyntaxTree* parseRun(std::vector<lexer::Word>& _wordList) {
            wordList = _wordList;
            program();
            return root;
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

        bool checkTerm(SyntaxTree* father) {
            bool isSuccess = true;
            lexer::Word nowWord = peek();
            SyntaxTree* current = addNode(father, TERM);
            if(nowWord.type != lexer::WordType::NUMBER) {
                std::cerr<< "[ERROR PARSE] checkTerm"
                    << "\trow:" << nowWord.row << "\tcol:" << nowWord.col
                    << "expected NUMBER" << std::endl;
                assert(nowWord.type == lexer::WordType::NUMBER);
                return isSuccess = false;
            }
            addNode(current, Word, nowWord);
            return isSuccess;
        }

        bool checkExpression(SyntaxTree* father) {
            bool isSuccess = true;
            lexer::Word nowWord = peek();
            SyntaxTree* current = addNode(father, EXPRESSION);

            if(nowWord.type == lexer::WordType::DELIMITER) {
                addNode(current, Word, nowWord);
                nowWord = getNextWord();
                if(nowWord.type == lexer::WordType::OPERATOR && nowWord.value == "=") {
                    addNode(current, Word, nowWord);
                    isSuccess = checkAddExpression(current);
                    if(!isSuccess) return isSuccess;
                }
            }
            return isSuccess;
        }

        bool checkAddExpression(SyntaxTree* father) {
            bool isSuccess = true;
            lexer::Word nowWord = getNextWord();
            SyntaxTree* current = addNode(father, ADD_EXPRESSION);

            if(nowWord.type != lexer::WordType::DELIMITER || nowWord.type != lexer::WordType::NUMBER) {
                std::cerr<< "[ERROR PARSE] checkAddExpression"
                    << "\trow:" << nowWord.row << "\tcol:" << nowWord.col
                    << "expected DELIMITER or NUMBER" << std::endl;
                assert(nowWord.type == lexer::WordType::DELIMITER || nowWord.type == lexer::WordType::NUMBER);
                return isSuccess = false;
            }

            isSuccess = checkTerm(current);
            if(!isSuccess) return isSuccess;
            nowWord = getNextWord();

            while(nowWord.type == lexer::WordType::OPERATOR) {
                if(nowWord.value == "+") {
                    addNode(current, Word, nowWord);
                }else if(nowWord.value == "-") {
                    addNode(current, Word, nowWord);
                }else if(nowWord.value == "*") {
                    addNode(current, Word, nowWord);
                }else if(nowWord.value == "/") {
                    addNode(current, Word, nowWord);                    
                }else break;
                nowWord = getNextWord();
                isSuccess = checkTerm(current);
                if(!isSuccess) return isSuccess;
                nowWord = getNextWord();
            }
            return isSuccess;
        }

        bool checkBoolExpression(SyntaxTree* father) {
            bool isSuccess = true;
            lexer::Word nowWord = peek();
            SyntaxTree* current = addNode(father, BOOL_EXPRESSION);

            isSuccess = checkAddExpression(current);
            if(!isSuccess) return isSuccess;

            addNode(current, Word, nowWord);
            nowWord = getNextWord();
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
                }else {
                    std::cerr<< "[ERROR PARSE] checkBoolExpression"
                        << "\trow:" << nowWord.row << "\tcol:" << nowWord.col
                        << "expected CMP OPERATOR" << std::endl;
                    assert(false);
                    return isSuccess = false;
                }
            }else {
                std::cerr<< "[ERROR PARSE] checkBoolExpression"
                    << "\trow:" << nowWord.row << "\tcol:" << nowWord.col
                    << "expected CMP OPERATOR" << std::endl;
                assert(false);
                return isSuccess = false;
            }
            nowWord = getNextWord();
            isSuccess = checkAddExpression(current);
            if(!isSuccess) return isSuccess;
            return isSuccess;
        }

        bool checkJudge(SyntaxTree* father) {
            bool isSuccess = true;
            lexer::Word nowWord = getNextWord();

            if(nowWord.type != lexer::WordType::DELIMITER && nowWord.value != "(") {
                std::cerr<< "[ERROR PARSE] checkJudge"
                    << "\trow:" << nowWord.row << "\tcol:" << nowWord.col
                    << "expected \'(\'" << std::endl;
                assert(nowWord.type == lexer::WordType::IDENTIFIER &&  nowWord.value == "(");
                return isSuccess = false;
            }
            addNode(father, Word, nowWord);

            isSuccess = checkBoolExpression(father);
            if(!isSuccess) return isSuccess;
            
            nowWord = getNextWord();
            if(nowWord.type != lexer::WordType::DELIMITER && nowWord.value != ")") {
                std::cerr<< "[ERROR PARSE] checkJudge"
                    << "\trow:" << nowWord.row << "\tcol:" << nowWord.col
                    << "expected \')\'" << std::endl;
                assert(nowWord.type == lexer::WordType::IDENTIFIER &&  nowWord.value == ")");
                return isSuccess = false;
            }
            addNode(father, Word, nowWord);
            return isSuccess;
        }

        bool checkIfStat(SyntaxTree* father) {
            bool isSuccess = true;
            lexer::Word nowWord = peek();
            SyntaxTree* current = addNode(father, IF_STAT);
            addNode(current, Word, nowWord);

            isSuccess = checkJudge(current);
            if(!isSuccess) return isSuccess;
            
            nowWord = getNextWord();
            isSuccess = checkStatement(current);
            if(!isSuccess) return isSuccess;

            nowWord = getNextWord();
            if(nowWord.type == lexer::WordType::KEYWORD && nowWord.value == "else") {
                addNode(current, Word, nowWord);
                nowWord = getNextWord();
                isSuccess = checkStatement(current);
                if(!isSuccess) return isSuccess;
            }
            return isSuccess;
        }

        bool checkWhile(SyntaxTree* father) {
            bool isSuccess = true;
            lexer::Word nowWord = peek();
            SyntaxTree* current = addNode(father, WHILE_STAT);
            addNode(current, Word, nowWord);

            isSuccess = checkJudge(current);
            if(!isSuccess) return isSuccess;
            
            nowWord = getNextWord();
            isSuccess = checkStatement(current);
            if(!isSuccess) return isSuccess;
            return isSuccess;
        }

        bool checkFor(SyntaxTree* father) {
            bool isSuccess = true;
            lexer::Word nowWord = peek();
            SyntaxTree* current = addNode(father, FOR_STAT);
            addNode(current, Word, nowWord);

            nowWord = getNextWord();
            if(nowWord.type != lexer::WordType::DELIMITER && nowWord.value != "(") {
                std::cerr<< "[ERROR PARSE] checkFor"
                    << "\trow:" << nowWord.row << "\tcol:" << nowWord.col
                    << "expected \'(\'" << std::endl;
                assert(nowWord.type == lexer::WordType::IDENTIFIER &&  nowWord.value == "(");
                return isSuccess = false;
            }
            addNode(father, Word, nowWord);

            nowWord = getNextWord();
            isSuccess = checkExpression(current);
            if(!isSuccess) return isSuccess;

            nowWord = getNextWord();
             if(nowWord.type != lexer::WordType::DELIMITER && nowWord.value != ";") {
                std::cerr<< "[ERROR PARSE] checkFor"
                    << "\trow:" << nowWord.row << "\tcol:" << nowWord.col
                    << "expected \';\'" << std::endl;
                assert(nowWord.type == lexer::WordType::IDENTIFIER &&  nowWord.value == ";");
                return isSuccess = false;
            }

            nowWord = getNextWord();
            isSuccess = checkBoolExpression(current);
            if(!isSuccess) return isSuccess;

            nowWord = getNextWord();
             if(nowWord.type != lexer::WordType::DELIMITER && nowWord.value != ";") {
                std::cerr<< "[ERROR PARSE] checkFor"
                    << "\trow:" << nowWord.row << "\tcol:" << nowWord.col
                    << "expected \';\'" << std::endl;
                assert(nowWord.type == lexer::WordType::DELIMITER &&  nowWord.value == ";");
                return isSuccess = false;
            }

            nowWord = getNextWord();
            isSuccess = checkExpression(current);
            if(!isSuccess) return isSuccess;

            nowWord = getNextWord();
            if(nowWord.type != lexer::WordType::DELIMITER && nowWord.value != ")") {
                std::cerr<< "[ERROR PARSE] checkFor"
                    << "\trow:" << nowWord.row << "\tcol:" << nowWord.col
                    << "expected \')\'" << std::endl;
                assert(nowWord.type == lexer::WordType::DELIMITER &&  nowWord.value == ")");
                return isSuccess = false;
            }
            addNode(father, Word, nowWord);

            nowWord = getNextWord();
            isSuccess = checkStatement(current);
            if(!isSuccess) return isSuccess;
            return isSuccess;
        }

        bool checkRead(SyntaxTree* father) {
            bool isSuccess = true;
            lexer::Word nowWord = peek();
            SyntaxTree* current = addNode(father, READ_STAT);
            addNode(current, Word, nowWord);

            nowWord = getNextWord();
            if(nowWord.type != lexer::WordType::IDENTIFIER) {
                std::cerr<< "[ERROR PARSE] checkRead"
                    << "\trow:" << nowWord.row << "\tcol:" << nowWord.col
                    << "expected IDENTIFIER" << std::endl;
                assert(nowWord.type == lexer::WordType::IDENTIFIER);
                return isSuccess = false;
            }
            addNode(current, Word, nowWord);

            nowWord = getNextWord();
            if(nowWord.type != lexer::WordType::DELIMITER && nowWord.value != ";") {
                std::cerr<< "[ERROR PARSE] checkRead"
                    << "\trow:" << nowWord.row << "\tcol:" << nowWord.col
                    << "expected \';\'" << std::endl;
                assert(nowWord.type == lexer::WordType::DELIMITER && nowWord.value == ";");
                return isSuccess = false;
            }
            addNode(current, Word, nowWord);
            return isSuccess;
        }

        bool checkWrite(SyntaxTree* father) {
            bool isSuccess = true;
            lexer::Word nowWord = peek();
            SyntaxTree* current = addNode(father, WRITE_STAT);
            addNode(current, Word, nowWord);

            nowWord = getNextWord();
            if(nowWord.type != lexer::WordType::IDENTIFIER) {
                std::cerr<< "[ERROR PARSE] checkRead"
                    << "\trow:" << nowWord.row << "\tcol:" << nowWord.col
                    << "expected IDENTIFIER" << std::endl;
                assert(nowWord.type == lexer::WordType::IDENTIFIER);
                return isSuccess = false;
            }
            addNode(current, Word, nowWord);

            nowWord = getNextWord();
            if(nowWord.type != lexer::WordType::DELIMITER && nowWord.value != ";") {
                std::cerr<< "[ERROR PARSE] checkRead"
                    << "\trow:" << nowWord.row << "\tcol:" << nowWord.col
                    << "expected \';\'" << std::endl;
                assert(nowWord.type == lexer::WordType::DELIMITER && nowWord.value == ";");
                return isSuccess = false;
            }
            addNode(current, Word, nowWord);
            return isSuccess;
        }

        bool checkCompound(SyntaxTree* father) {
            bool isSuccess = true;
            lexer::Word nowWord = peek();
            SyntaxTree* current = addNode(father, STATEMENT);
            addNode(current, Word, nowWord);

            nowWord = getNextWord();
            isSuccess = checkStatementList(current);
            return isSuccess;
        }

        bool checkExpressionStat(SyntaxTree* father) {
            bool isSuccess = true;
            lexer::Word nowWord = peek();
            SyntaxTree* current = addNode(father, EXPRESSION_STAT);

            if(nowWord.type == lexer::WordType::DELIMITER && nowWord.value == ";") {
                addNode(current, Word, nowWord);
                nowWord = getNextWord();
                return isSuccess;
            }

            isSuccess = checkExpression(current);
            if(!isSuccess) return isSuccess;

            nowWord = getNextWord();
            if(nowWord.type == lexer::WordType::DELIMITER && nowWord.value == ";") {
                addNode(current, Word, nowWord);
                nowWord = getNextWord();
                return isSuccess;
            }else{
                std::cerr<< "[ERROR PARSE] checkExpressionStat"
                    << "\trow:" << nowWord.row << "\tcol:" << nowWord.col
                    << "expected \';\'" << std::endl;
                assert(nowWord.type == lexer::WordType::DELIMITER && nowWord.value == ";");
                return isSuccess = false;
            }
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
                    isSuccess = checkWhile(current);
                    if(!isSuccess) return isSuccess;
                }else if(nowWord.value == "for") {
                    isSuccess = checkFor(current);
                    if(!isSuccess) return isSuccess;
                }else if(nowWord.value == "read") {
                    isSuccess = checkRead(current);
                    if(!isSuccess) return isSuccess;
                }else if(nowWord.value == "write") {
                    isSuccess = checkWrite(current);
                    if(!isSuccess) return isSuccess;
                }
            }else if(nowWord.type == lexer::WordType::DELIMITER) {
                if(nowWord.value == "{") {
                    isSuccess = checkCompound(current);
                    if(!isSuccess) return isSuccess;
                }else if(nowWord.value == "(" || nowWord.value == ";") {
                    isSuccess = checkExpressionStat(current);
                    if(!isSuccess) return isSuccess;
                }
            }else if(nowWord.type == lexer::WordType::NUMBER) {
                isSuccess = checkExpressionStat(current);
                if(!isSuccess) return isSuccess;
            }else if(nowWord.type == lexer::WordType::IDENTIFIER) {
                isSuccess = checkExpressionStat(current);
                if(!isSuccess) return isSuccess;
            }
            return isSuccess;
        }

        bool checkStatementList(SyntaxTree* father) {
            bool isSuccess = true;
            lexer::Word nowWord = peek();
            SyntaxTree* current = addNode(father, STATEMENT_LIST);

            while(nowWord.type != lexer::WordType::DELIMITER || nowWord.value != "}") {
                isSuccess = checkStatement(current);
                if(!isSuccess) return isSuccess;
                nowWord = peek();
            }
        }
        
        bool checkDeclarationStat(SyntaxTree* father) {
            bool isSuccess = true;
           lexer::Word nowWord = peek();
            SyntaxTree* current = addNode(father, DECLARATION_STAT);
            addNode(current, Word, nowWord);
            // var name
            nowWord = getNextWord();
            if(nowWord.type != lexer::WordType::IDENTIFIER) {
                std::cerr<< "[ERROR PARSE] checkDeclarationStat"
                    << "\trow:" << nowWord.row << "\tcol:" << nowWord.col
                    << "expected IDENTIFIER" << std::endl;
                assert(nowWord.type == lexer::WordType::IDENTIFIER);
                return false;
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
                    return false;
                }
                addNode(current, Word, nowWord);
            }
            // ;
            nowWord = getNextWord();
            if(nowWord.type != lexer::WordType::DELIMITER || nowWord.value != ";") {
                std::cerr<< "[ERROR PARSE] checkDeclarationStat"
                        << "\trow:" << nowWord.row << "\tcol:" << nowWord.col
                        << "expected \';\'" << std::endl;
                assert(nowWord.type == lexer::WordType::NUMBER && nowWord.value == ";");
                return false;
            }
            addNode(current, Word, nowWord);

            return true;
        }

        bool checkDeclarationList(SyntaxTree* father) {
            bool isSuccess = true;
            lexer::Word nowWord = peek();
            SyntaxTree* current = addNode(father, DECLARATION_LIST);
            while(true) {
                nowWord = getNextWord();
                if(nowWord.type != lexer::WordType::KEYWORD || nowWord.value != "int") 
                    break;
                isSuccess = checkDeclarationStat(current);
                if(!isSuccess) return isSuccess;
            }
            return isSuccess;
        }

        bool program() {
            bool isSuccess = true;
            lexer::Word nowWord = peek();
            if(nowWord.type != lexer::WordType::DELIMITER || nowWord.value != "}") {
                std::cerr<< "[ERROT PARSE] PROGRAM"
                    << "\trow:" << nowWord.row << "\tcol:" << nowWord.col
                    << "expected \'}\'" << std::endl;
                assert(nowWord.type != lexer::WordType::DELIMITER && nowWord.value != "}");
            }
            addNode(root, Word, nowWord);
            
            isSuccess = checkDeclarationList(root);
            if(!isSuccess) return isSuccess;

            isSuccess = checkStatementList(root);
            return isSuccess;
        }
    }
}