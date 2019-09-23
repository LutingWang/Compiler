/**********************************************
    > File Name: main.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Mon Sep 23 09:22:09 2019
 **********************************************/

#include <iostream>
#include "compilerConfig.h"
using namespace std;

int main() {
	// print compiler version info
	cout << "compiler version " 
		<< COMPILER_VERSION_MAJOR 
		<< "." 
		<< COMPILER_VERSION_MINOR 
		<< endl;
	return 0;
}
