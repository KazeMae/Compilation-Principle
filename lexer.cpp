#include "lexer.hpp"

namespace testCompiler {
	namespace lexer{
		bool isOK = true;
		std::string source;
		// keyNum = 7
		std::vector<std::string> keyWord = {"else","for","if","int","read","while","write"}; 
		int currentIndex, lineCounter, lineIndex;
		// 进行词法分析
		std::vector<Word> lexerRun(const std::string& sourceCode) {
			source = sourceCode;
			currentIndex = 0, lineCounter = 1, lineIndex = 1;
			std::vector<Word> wordList = getWordList();
			return wordList;
		}

		// 格式化输出词法分析结果
		std::ostream &operator<< (std::ostream &os, Word& x) {
			os<< "Type:\t";
			switch (x.type) {
			case WordType::IDENTIFIER:
				os<< "IDENTIFIER\t";
				break;
			case WordType::NUMBER:
				os<< "NUMBER\t\t";
				break;
			case WordType::KEYWORD:
				os<< "KEYWORD\t\t";
				break;
			case WordType::OPERATOR:
				os<< "OPERATOR\t";
				break;
			case WordType::DELIMITER:
				os<< "DELIMITER\t";
				break;
			case WordType::PROMPT:
				os<< "PROMPT\t\t";
				break;
			case WordType::UNKOWN:
				os<< "UNKNOWN\t\t";
				break;
			default:
				os<< "[ERROR]UNKNOWN\t";
				break;
			}
			os<< "Value:\t" << x.value << "\trow:" << x.row << "\tcol:" << x.col;
			return os;
		}
		// 当前字符
		char peek() {
			return currentIndex < (int)source.length() ? source[currentIndex] : '\0';
		}
		// 下一字符
		char getChar() {
			++ lineIndex;
			if(peek() == '\n') ++ lineCounter, lineIndex = 1;
			return peek() == '\0' ? '\0' : source[currentIndex ++];
		}
		// 跳过空格字符
		void skipSpace() {
			while(peek() != '\0' && std::isspace(peek())) 
				getChar();
		}
		// 识别关键字
		bool isKeyWord(const std::string& word) {
			for(auto i :keyWord) 
				if (i == word) return true;
			return false;
		}
		// 标识符和关键字
		Word getIdentifierOrKeyword() {
			Word word;
			std::string identifier;
			if (!std::isalpha(peek())) {
				std::cerr<< "[ERROR LEXER] Identifier Or Keyword" 
					<< "\trow:"<< lineCounter << "\tcol:" << lineIndex << "\t:" << currentIndex 
					<< "\tThe word front is not alpha" << std::endl;
				isOK = false;
				identifier += getChar();
				word.type = UNKOWN, word.value = identifier, word.row = lineCounter, word.col = lineIndex;
				return word;
			}
			// 标识符
			while(std::isalpha(peek()) || std::isdigit(peek())) {
				identifier += getChar();
			}
			// 是否为关键字
			if(isKeyWord(identifier)){
				word.type = KEYWORD;
				word.value = identifier;
				word.row = lineCounter, word.col = lineIndex;
			} else {
				word.type = IDENTIFIER;
				word.value = identifier;
				word.row = lineCounter, word.col = lineIndex;
			}
			return word;
		}
		// 数字
		Word getNumber() {
			std::string number;
			Word word;
			bool isS = true;
			while(std::isdigit(peek()) || std::isalpha(peek())) {
				if(std::isalpha(peek())) isS = false;
				number += getChar();
			}
			if(!isS) {
				std::cerr<< "[ERROR LEXER] Number" 
					<< "\trow:"<< lineCounter << "\tcol:" << lineIndex << "\t:" << currentIndex 
					<< "\tThe number is not all digit" << std::endl;
				isOK = false;
				word.type = UNKOWN, word.value = number;
				word.row = lineCounter, word.col = lineIndex;
				return word;
			}
			if((int)number.length() > 1 && number.front() == '0') {
				std::cerr<< "[ERROR LEXER] Number" 
					<< "\trow:"<< lineCounter << "\tcol:" << lineIndex << "\t:" << currentIndex 
					<< "\tThe number front is 0 and The lenth is not 0" << std::endl;
				isOK = false;
				word.type = UNKOWN, word.value = number;
				word.row = lineCounter, word.col = lineIndex;
				return word;
				// assert(!((int)number.length() > 1 && number.front() == '0'));
			}
			word.type = NUMBER, word.value = number;
			word.row = lineCounter, word.col = lineIndex;
			return word;
		}
		// 运算符, 分隔符和注释
		Word getOperatorOrDelimiterOrPrompt() {
			std::string odp;
			Word word;
			// 分隔符
			if(peek() == '(' || peek() == ')' || peek() == ';' || peek() == '{' || peek() == '}') {
				odp += getChar();
				word.type = DELIMITER;
				word.value = odp;
				word.row = lineCounter, word.col = lineIndex;
				return word;
			}
			// 运算符 单符号 and 注释
			if(peek() == '+' || peek() == '-' || peek() == '*') {
				odp += getChar();
				// 注释
				if(odp.front() == '/' && peek() == '/'){
					while(peek() != '\n') {
						odp += getChar();
					}
					word.type = PROMPT;
					word.value = odp;
					word.row = lineCounter, word.col = lineIndex;
					return word;
				}
				word.type = OPERATOR;
				word.value = odp;
				word.row = lineCounter, word.col = lineIndex;
				return word;
			}
			//除 and 注释
			if(peek() == '/') {
				odp += getChar();
				// 注释
				if(peek() == '/'){
					odp += getChar();
					while(peek() != '\n') {
						odp += getChar();
					}
					word.type = PROMPT;
					word.value = odp;
					word.row = lineCounter, word.col = lineIndex;
					return word;
				}
				word.type = OPERATOR;
				word.value = odp;
				word.row = lineCounter, word.col = lineIndex;
				return word;
			}
			// 比较运算 and 比较双运算符
			if(peek() == '=' || peek() == '<' || peek() == '>' || peek() == '!'){
				odp += getChar();
				if(peek() == '=')
					odp += getChar();
				word.type = OPERATOR;
				word.value = odp;
				word.row = lineCounter, word.col = lineIndex;
				return word;
			}
			std::cerr<< "[ERROR LEXER] Operator" 
				<< "\trow:"<< lineCounter << "\tcol:" << lineIndex << "\t:" << currentIndex 
				<< "\tThe operator front is illegal" << std::endl;
			isOK = false;
			odp += getChar();
			word.type = UNKOWN, word.value = odp;
			word.row = lineCounter, word.col = lineIndex;
			// assert(false);
			return word;
		}
		// 进行词法分析
		std::vector<Word> getWordList() {
			std::vector<Word> words;
			while(currentIndex < (int)source.length()){
				skipSpace();
				if(peek() == '\0') break;
				if(std::isalpha(peek()))
					words.push_back(getIdentifierOrKeyword());
				else if(std::isdigit(peek()))
					words.push_back(getNumber());
				else 
					words.push_back(getOperatorOrDelimiterOrPrompt());
			}
			return words;
		}
	}
}
