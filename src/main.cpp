/**********************************************
    > File Name: main.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Mon Sep 23 09:22:09 2019
 **********************************************/

#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include "compilerConfig.h"
#include "error.h"
#include "frontend.h"
#include "midcode/MidCode.h"
#include "mips.h"
#include "Optim.h"

#include "./files.h"
using namespace std;

// TODO: change midcode naming to avoid conflict
// TODO: scan iterations, add comment of type

int main(int argc, char* argv[]) {
	// print compiler version info
	cout << "compiler version " << COMPILER_VERSION_MAJOR 
		<< "." << COMPILER_VERSION_MINOR << endl;
    
#if judge
    assert(argc == 1);
    const string testfile_path = "testfile.txt";
    files::open();
#else
    assert(argc == 2);
    const string testfile_path = PROJECT_BASE_DIR + string(argv[1]);
    files::open(testfile_path);
#endif /* judge */
	
	cout << "grammar analysis processing ... ";
    grammar::parse(testfile_path);
	cout << "finished" << endl;

    if (error::happened) { 
		cout << "WARNING: error detected, aborting compilation" << endl;
		goto exit; 
	}

	cout << "midcode generating ... ";
    MidCode::output();
	cout << "finished" << endl;

	cout << "optimization processing ... ";
    Optim::inlineExpan();
    // Optim::commonExprElim();
    Optim::symProp();
    Optim::peephole();
	cout << "finished" << endl;

	cout << "optimized midcode generating ... ";
    MidCode::output();
	cout << "finished" << endl;

	cout << "mips code generating ... ";
    mips::init();
    mips::output();
    mips::deinit();
	cout << "finished" << endl;

exit:
    files::close();
	return 0;
}
