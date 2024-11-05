#include "compiler.hpp"

namespace testCompiler {
	std::string sourceCodePath;
	std::string outputPath;
	std::vector<std::string> argList;
	int stateCompiler = 10;
	
	void init(int argc, char** argv) {
		for (int i = 0; i < argc; ++ i) argList.push_back(argv[i]);
		int state = 0;
		for(auto i : argList) {
			if(i == "-o" || i == "-O") 	state = 1;
			else if(i == "-l" || i == "-L") stateCompiler = 1;
			else if(i == "-p" || i == "-P") stateCompiler = 2;
			else if(state == 1) state = 0, outputPath = i;
			else sourceCodePath = i;
		}
		if(sourceCodePath.empty()){
			std::cerr << "[ERROR INIT] source file path empty!" << std::endl;
			assert(sourceCodePath.size() > 0);
		}
		if(outputPath.empty()) outputPath = sourceCodePath + ".out";
	}
	
	void run() {
		std::ifstream fin;
		std::ofstream fout;
		std::string sourceCode;
		// 读取源代码文件
		fin.open(sourceCodePath, std::ios::in);
		if(!fin.is_open()) {
			std::cerr << "[ERROR COMPILER] cannot open source file!" << std::endl;
			assert(fin.is_open());
		}
		for(char c; (c = fin.get()) != EOF; sourceCode += c);
		fin.close();

		// 词法分析
		auto wordList = lexer::lexerRun(sourceCode);
		if(!lexer::isOK) {
			std::cerr<< "[ERROR COMPILER] Lexer fail" << std::endl;
		}
		
		if(stateCompiler < 2) {
			fout.open(outputPath, std::ios::out);
			if(!fout.is_open()) {
				std::cerr << "[ERROR RUN] cannot open output file!" << std::endl;
				assert(fout.is_open());
			}
			for(auto i : wordList) fout<< i <<std::endl;
			return;
		}

		// 语法分析
		auto syntaxTreeRoot = parse::parseRun(wordList);
		if(!parse::isOK) {
			std::cerr<< "[ERROR COMPILER] Parse fail" << std::endl;
		}

		if(stateCompiler < 3) {
			fout.open(outputPath, std::ios::out);
			if(!fout.is_open()) {
				std::cerr << "[ERROR RUN] cannot open output file!" << std::endl;
				assert(fout.is_open());
			}
			parse::outputSyntaxTree(fout, syntaxTreeRoot, 0);
			return;
		}

		// 输出结果
		fout.open(outputPath, std::ios::out);
		if(!fout.is_open()) {
			std::cerr << "[ERROR RUN] cannot open output file!" << std::endl;
			assert(fout.is_open());
		}
		parse::outputSyntaxTree(fout, syntaxTreeRoot, 0);
		
	}
}
