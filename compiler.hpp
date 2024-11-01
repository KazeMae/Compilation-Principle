#ifndef COMPILER_HPP
#define COMPILER_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cassert>
#include "lexer.hpp"

namespace testCompiler {
	extern std::string sourceCodePath;
	extern std::string outputPath;
	extern std::vector<std::string> argList;
	extern int stateCompiler;
	
	void init(int argc, char** argv);
	void run();
}

#endif
