#ifndef LEXER_HPP
#define LEXER_HPP

#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include <ctype.h>

namespace testCompiler {
	namespace lexer{
		// 词法分析结果类型, UNKOWN 为出现错误
		enum WordType {
			IDENTIFIER,		// 标识符
			NUMBER,			// 数字
			KEYWORD,		// 关键字
			OPERATOR,		// 运算符
			DELIMITER,		// 分隔符
			PROMPT,			// 注释
			UNKOWN			// 未知
		};
		// 存储词法分析结果
		struct Word{
			WordType type;
			std::string value;
		};
		std::ostream &operator<<(std::ostream &os, Word& x);

		extern bool isOK;	// 词法分析是否成功
		extern std::string source;	// 源代码字符串
		extern std::vector<std::string> keyWord;	// 关键字列表
		extern int currentIndex, lineCounter, lineIndex;	// 源代码字符串索引, 行号, 行内索引
		
		// 进行词法分析
		std::vector<Word> lexerRun(const std::string& sourceCode);

		// 当前字符
		char peek();
		// 获取字符, 同时索引向后移动一位
		char getChar();
		// 跳过空白符
		void skipSpace();
		// 检查关键字
		bool isKeyWord(const std::string& word);
		// 分析标识符和关键字
		Word getIdentifierOrKeyword();
		// 分析数字
		Word getNumber();
		// 分析运算符, 分隔符和注释
		Word getOperatorOrDelimiterOrPrompt();
		// 获取词列表
		std::vector<Word> getWordList();
	}
}
#endif
