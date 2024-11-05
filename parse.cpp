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

        lexer::Word peek() {
            return wordIndex < (int)wordList.size() ? wordList[wordIndex] : lexer::Word();
        }
        
        lexer::Word getNextWord() {
            ++ wordIndex;
            return peek();
        }

        void getDeclarationList();

        void program() {
            lexer::Word nowWord = peek();
            if(nowWord.type != lexer::WordType::DELIMITER && nowWord.value != "}") {
                std::cerr<< "[ERROT PARSE] PROGRAM"
                    << "\trow:" << nowWord.row << "\tcol:" << nowWord.col
                    << "expected \'}\'" << std::endl;
                assert(nowWord.type != lexer::WordType::DELIMITER && nowWord.value != "}");
            }
            root->addNode(nowWord.value);

        }
    }
}