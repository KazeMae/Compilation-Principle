#ifndef LEXER_HPP
#define LEXER_HPP

#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include <ctype.h>

namespace testCompiler {
	namespace lexer{
		enum WordType {
			IDENTIFIER,		// 标识符
			NUMBER,			// 数字
			KEYWORD,		// 关键字
			OPERATOR,		// 运算符
			DELIMITER,		// 分隔符
			PROMPT,			// 注释
			UNKOWN			// 未知
		};
		
		struct Word{
			WordType type;
			std::string value;
		};
		std::ostream &operator<<(std::ostream &os, Word& x);

		extern std::string source;
		extern std::vector<std::string> keyWord; 
		extern int currentIndex, lineCounter, lineIndex;
		
		std::vector<Word> lexerRun(const std::string& sourceCode);

		char peek();
		char getChar();
		void skipSpace();
		bool isKeyWord(const std::string& word);
		Word getIdentifierOrKeyword();
		Word getNumber();
		Word getOperatorOrDelimiterOrPrompt();
		
		std::vector<Word> getWordList();
	}
}
#endif
