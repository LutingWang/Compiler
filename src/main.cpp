/**********************************************
    > File Name: main.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Mon Sep 23 09:22:09 2019
 **********************************************/

#include <iostream>
#include "compilerConfig.h"
#include "compiler.h"
using namespace std;

#if debug
	InputFile input(PROJECT_BASE_DIR "/test/error/def.txt");
	ofstream logger(PROJECT_BASE_DIR "/log.txt");
	ofstream err(PROJECT_BASE_DIR "/test/error/def.out");
#else
	InputFile input("testfile.txt");
	ofstream logger("log.txt");
	ofstream err("error.txt");
#endif /* debug */

int main() {
	// print compiler version info
	cout << "compiler version " << COMPILER_VERSION_MAJOR 
		<< "." << COMPILER_VERSION_MINOR << endl;

	grammar::parse();
	return 0;
}
