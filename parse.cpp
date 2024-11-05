#include "parse.hpp"

namespace testCompiler {
    namespace parse {
        bool isOK = true;
        int wordIndex = 0;
        std::vector<lexer::Word> wordList;

        SyntaxTree* root = new SyntaxTree();

        void parseRun(std::vector<lexer::Word>& _wordList) {
            wordList = _wordList;
            program();
        }

        SyntaxTree* addNode(SyntaxTree* p, SyntaxType type) {
            return p->addNode(type);
        }

        SyntaxTree* addNode(SyntaxTree* p, SyntaxType type, lexer::Word wrod) {
            return p->addNode(type, wrod);
        }

        lexer::Word peek() {
            return wordIndex < (int)wordList.size() ? wordList[wordIndex] : lexer::Word();
        }
        
        lexer::Word getNextWord() {
            ++ wordIndex;
            return peek();
        }

        bool checkStatementList(SyntaxTree* father) {

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