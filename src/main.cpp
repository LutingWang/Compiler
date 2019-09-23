/**********************************************
    > File Name: main.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Mon Sep 23 09:22:09 2019
 **********************************************/

#include <iostream>
#include "compilerConfig.h"
#include "compiler.h"
#include "lexer.h"
using namespace std;

ifstream input(PROJECT_BASE_DIR "/test/testfile1.txt");

int main() {
	// print compiler version info
	cout << "compiler version " 
		<< COMPILER_VERSION_MAJOR 
		<< "." 
		<< COMPILER_VERSION_MINOR 
		<< endl;

	ofstream testoutput(PROJECT_BASE_DIR "/test/result.txt");
	lexer::test(testoutput);
	return 0;
}
